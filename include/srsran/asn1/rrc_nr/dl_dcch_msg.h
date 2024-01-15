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

#include "msg_common.h"
#include "ue_cap.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

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

// CellReselectionSubPriority ::= ENUMERATED
struct cell_resel_sub_prio_opts {
  enum options { odot2, odot4, odot6, odot8, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using cell_resel_sub_prio_e = enumerated<cell_resel_sub_prio_opts>;

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

// CSI-RS-Resource-Mobility ::= SEQUENCE
struct csi_rs_res_mob_s {
  struct slot_cfg_c_ {
    struct types_opts {
      enum options { ms4, ms5, ms10, ms20, ms40, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    slot_cfg_c_() = default;
    slot_cfg_c_(const slot_cfg_c_& other);
    slot_cfg_c_& operator=(const slot_cfg_c_& other);
    ~slot_cfg_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ms4()
    {
      assert_choice_type(types::ms4, type_, "slotConfig");
      return c.get<uint8_t>();
    }
    uint8_t& ms5()
    {
      assert_choice_type(types::ms5, type_, "slotConfig");
      return c.get<uint8_t>();
    }
    uint8_t& ms10()
    {
      assert_choice_type(types::ms10, type_, "slotConfig");
      return c.get<uint8_t>();
    }
    uint8_t& ms20()
    {
      assert_choice_type(types::ms20, type_, "slotConfig");
      return c.get<uint8_t>();
    }
    uint16_t& ms40()
    {
      assert_choice_type(types::ms40, type_, "slotConfig");
      return c.get<uint16_t>();
    }
    const uint8_t& ms4() const
    {
      assert_choice_type(types::ms4, type_, "slotConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& ms5() const
    {
      assert_choice_type(types::ms5, type_, "slotConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& ms10() const
    {
      assert_choice_type(types::ms10, type_, "slotConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& ms20() const
    {
      assert_choice_type(types::ms20, type_, "slotConfig");
      return c.get<uint8_t>();
    }
    const uint16_t& ms40() const
    {
      assert_choice_type(types::ms40, type_, "slotConfig");
      return c.get<uint16_t>();
    }
    uint8_t&  set_ms4();
    uint8_t&  set_ms5();
    uint8_t&  set_ms10();
    uint8_t&  set_ms20();
    uint16_t& set_ms40();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct associated_ssb_s_ {
    uint8_t ssb_idx            = 0;
    bool    is_quasi_colocated = false;
  };
  struct freq_domain_alloc_c_ {
    struct types_opts {
      enum options { row1, row2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    freq_domain_alloc_c_() = default;
    freq_domain_alloc_c_(const freq_domain_alloc_c_& other);
    freq_domain_alloc_c_& operator=(const freq_domain_alloc_c_& other);
    ~freq_domain_alloc_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<4>& row1()
    {
      assert_choice_type(types::row1, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<4>>();
    }
    fixed_bitstring<12>& row2()
    {
      assert_choice_type(types::row2, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<12>>();
    }
    const fixed_bitstring<4>& row1() const
    {
      assert_choice_type(types::row1, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<4>>();
    }
    const fixed_bitstring<12>& row2() const
    {
      assert_choice_type(types::row2, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<12>>();
    }
    fixed_bitstring<4>&  set_row1();
    fixed_bitstring<12>& set_row2();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<12>> c;

    void destroy_();
  };
  struct slot_cfg_r17_c_ {
    struct types_opts {
      enum options { ms4, ms5, ms10, ms20, ms40, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    slot_cfg_r17_c_() = default;
    slot_cfg_r17_c_(const slot_cfg_r17_c_& other);
    slot_cfg_r17_c_& operator=(const slot_cfg_r17_c_& other);
    ~slot_cfg_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t& ms4()
    {
      assert_choice_type(types::ms4, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms5()
    {
      assert_choice_type(types::ms5, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms10()
    {
      assert_choice_type(types::ms10, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms20()
    {
      assert_choice_type(types::ms20, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms40()
    {
      assert_choice_type(types::ms40, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms4() const
    {
      assert_choice_type(types::ms4, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms5() const
    {
      assert_choice_type(types::ms5, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms10() const
    {
      assert_choice_type(types::ms10, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms20() const
    {
      assert_choice_type(types::ms20, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms40() const
    {
      assert_choice_type(types::ms40, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    uint16_t& set_ms4();
    uint16_t& set_ms5();
    uint16_t& set_ms10();
    uint16_t& set_ms20();
    uint16_t& set_ms40();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                 ext                    = false;
  bool                 associated_ssb_present = false;
  uint8_t              csi_rs_idx             = 0;
  slot_cfg_c_          slot_cfg;
  associated_ssb_s_    associated_ssb;
  freq_domain_alloc_c_ freq_domain_alloc;
  uint8_t              first_ofdm_symbol_in_time_domain = 0;
  uint16_t             seq_generation_cfg               = 0;
  // ...
  // group 0
  copy_ptr<slot_cfg_r17_c_> slot_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighbourCellInfo-r17 ::= SEQUENCE
struct neighbour_cell_info_r17_s {
  epoch_time_r17_s     epoch_time_r17;
  ephemeris_info_r17_c ephemeris_info_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RAN-VisibleParameters-r17 ::= SEQUENCE
struct ran_visible_params_r17_s {
  struct ran_visible_periodicity_r17_opts {
    enum options { ms120, ms240, ms480, ms640, ms1024, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using ran_visible_periodicity_r17_e_ = enumerated<ran_visible_periodicity_r17_opts>;

  // member variables
  bool                           ext                                                = false;
  bool                           ran_visible_periodicity_r17_present                = false;
  bool                           nof_buffer_level_entries_r17_present               = false;
  bool                           report_playout_delay_for_media_startup_r17_present = false;
  ran_visible_periodicity_r17_e_ ran_visible_periodicity_r17;
  uint8_t                        nof_buffer_level_entries_r17               = 1;
  bool                           report_playout_delay_for_media_startup_r17 = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RSSI-PeriodicityAndOffset-r16 ::= CHOICE
struct rssi_periodicity_and_offset_r16_c {
  struct types_opts {
    enum options { sl10, sl20, sl40, sl80, sl160, sl320, s1640, /*...*/ nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  rssi_periodicity_and_offset_r16_c() = default;
  rssi_periodicity_and_offset_r16_c(const rssi_periodicity_and_offset_r16_c& other);
  rssi_periodicity_and_offset_r16_c& operator=(const rssi_periodicity_and_offset_r16_c& other);
  ~rssi_periodicity_and_offset_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& sl10()
  {
    assert_choice_type(types::sl10, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl20()
  {
    assert_choice_type(types::sl20, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl40()
  {
    assert_choice_type(types::sl40, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl80()
  {
    assert_choice_type(types::sl80, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl160()
  {
    assert_choice_type(types::sl160, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint16_t& sl320()
  {
    assert_choice_type(types::sl320, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint16_t& s1640()
  {
    assert_choice_type(types::s1640, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint8_t& sl10() const
  {
    assert_choice_type(types::sl10, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl20() const
  {
    assert_choice_type(types::sl20, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl40() const
  {
    assert_choice_type(types::sl40, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl80() const
  {
    assert_choice_type(types::sl80, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl160() const
  {
    assert_choice_type(types::sl160, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint16_t& sl320() const
  {
    assert_choice_type(types::sl320, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& s1640() const
  {
    assert_choice_type(types::s1640, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint8_t&  set_sl10();
  uint8_t&  set_sl20();
  uint8_t&  set_sl40();
  uint8_t&  set_sl80();
  uint8_t&  set_sl160();
  uint16_t& set_sl320();
  uint16_t& set_s1640();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// SSB-PositionQCL-Cell-r17 ::= SEQUENCE
struct ssb_position_qcl_cell_r17_s {
  uint16_t                        pci_r17 = 0;
  ssb_position_qcl_relation_r17_e ssb_position_qcl_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-PositionQCL-CellsToAddMod-r16 ::= SEQUENCE
struct ssb_position_qcl_cells_to_add_mod_r16_s {
  uint16_t                        pci_r16 = 0;
  ssb_position_qcl_relation_r16_e ssb_position_qcl_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BFD-RelaxationReportingConfig-r17 ::= SEQUENCE
struct bfd_relaxation_report_cfg_r17_s {
  struct bfd_relaxtion_report_prohibit_timer_opts {
    enum options {
      s0,
      s0dot5,
      s1,
      s2,
      s5,
      s10,
      s20,
      s30,
      s60,
      s90,
      s120,
      s300,
      s600,
      infinity,
      spare2,
      spare1,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using bfd_relaxtion_report_prohibit_timer_e_ = enumerated<bfd_relaxtion_report_prohibit_timer_opts>;

  // member variables
  bfd_relaxtion_report_prohibit_timer_e_ bfd_relaxtion_report_prohibit_timer;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-RS-CellMobility ::= SEQUENCE
struct csi_rs_cell_mob_s {
  struct csi_rs_meas_bw_s_ {
    struct nrof_prbs_opts {
      enum options { size24, size48, size96, size192, size264, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using nrof_prbs_e_ = enumerated<nrof_prbs_opts>;

    // member variables
    nrof_prbs_e_ nrof_prbs;
    uint16_t     start_prb = 0;
  };
  struct density_opts {
    enum options { d1, d3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using density_e_             = enumerated<density_opts>;
  using csi_rs_res_list_mob_l_ = dyn_array<csi_rs_res_mob_s>;

  // member variables
  bool                   density_present = false;
  uint16_t               cell_id         = 0;
  csi_rs_meas_bw_s_      csi_rs_meas_bw;
  density_e_             density;
  csi_rs_res_list_mob_l_ csi_rs_res_list_mob;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CandidateServingFreqListNR-r16 ::= SEQUENCE (SIZE (1..128)) OF INTEGER (0..3279165)
using candidate_serving_freq_list_nr_r16_l = dyn_array<uint32_t>;

// CondReconfigToAddMod-r16 ::= SEQUENCE
struct cond_recfg_to_add_mod_r16_s {
  using cond_execution_cond_r16_l_ = bounded_array<uint8_t, 2>;

  // member variables
  bool                       ext               = false;
  uint8_t                    cond_recfg_id_r16 = 1;
  cond_execution_cond_r16_l_ cond_execution_cond_r16;
  dyn_octstring              cond_rrc_recfg_r16;
  // ...
  // group 0
  dyn_octstring cond_execution_cond_scg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ExcessDelay-DRB-IdentityInfo-r17 ::= SEQUENCE
struct excess_delay_drb_id_info_r17_s {
  using drb_id_list_l_ = bounded_array<uint8_t, 29>;
  struct delay_thres_opts {
    enum options {
      ms0dot25,
      ms0dot5,
      ms1,
      ms2,
      ms4,
      ms5,
      ms10,
      ms20,
      ms30,
      ms40,
      ms50,
      ms60,
      ms70,
      ms80,
      ms90,
      ms100,
      ms150,
      ms300,
      ms500,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using delay_thres_e_ = enumerated<delay_thres_opts>;

  // member variables
  drb_id_list_l_ drb_id_list;
  delay_thres_e_ delay_thres;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IAB-IP-Usage-r16 ::= ENUMERATED
struct iab_ip_usage_r16_opts {
  enum options { f1_c, f1_u, non_f1, spare, nulltype } value;

  const char* to_string() const;
};
using iab_ip_usage_r16_e = enumerated<iab_ip_usage_r16_opts>;

// MUSIM-Gap-r17 ::= SEQUENCE
struct mu_si_m_gap_r17_s {
  uint8_t                musim_gap_id_r17 = 0;
  mu_si_m_gap_info_r17_s musim_gap_info_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MUSIM-GapAssistanceConfig-r17 ::= SEQUENCE
struct mu_si_m_gap_assist_cfg_r17_s {
  struct musim_gap_prohibit_timer_r17_opts {
    enum options {
      s0,
      s0dot1,
      s0dot2,
      s0dot3,
      s0dot4,
      s0dot5,
      s1,
      s2,
      s3,
      s4,
      s5,
      s6,
      s7,
      s8,
      s9,
      s10,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using musim_gap_prohibit_timer_r17_e_ = enumerated<musim_gap_prohibit_timer_r17_opts>;

  // member variables
  musim_gap_prohibit_timer_r17_e_ musim_gap_prohibit_timer_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MUSIM-LeaveAssistanceConfig-r17 ::= SEQUENCE
struct mu_si_m_leave_assist_cfg_r17_s {
  struct musim_leave_without_resp_timer_r17_opts {
    enum options { ms10, ms20, ms40, ms60, ms80, ms100, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using musim_leave_without_resp_timer_r17_e_ = enumerated<musim_leave_without_resp_timer_r17_opts>;

  // member variables
  musim_leave_without_resp_timer_r17_e_ musim_leave_without_resp_timer_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasConfigAppLayer-r17 ::= SEQUENCE
struct meas_cfg_app_layer_r17_s {
  struct service_type_r17_opts {
    enum options { streaming, mtsi, vr, spare5, spare4, spare3, spare2, spare1, nulltype } value;

    const char* to_string() const;
  };
  using service_type_r17_e_ = enumerated<service_type_r17_opts>;

  // member variables
  bool                                      ext                                  = false;
  bool                                      service_type_r17_present             = false;
  bool                                      pause_report_r17_present             = false;
  bool                                      tx_of_session_start_stop_r17_present = false;
  bool                                      ran_visible_params_r17_present       = false;
  uint8_t                                   meas_cfg_app_layer_id_r17            = 0;
  bounded_octstring<1, 8000>                meas_cfg_app_layer_container_r17;
  service_type_r17_e_                       service_type_r17;
  bool                                      pause_report_r17             = false;
  bool                                      tx_of_session_start_stop_r17 = false;
  setup_release_c<ran_visible_params_r17_s> ran_visible_params_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCI-List ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..1007)
using pci_list_l = bounded_array<uint16_t, 32>;

// PropDelayDiffReportConfig-r17 ::= SEQUENCE
struct prop_delay_diff_report_cfg_r17_s {
  struct thresh_prop_delay_diff_r17_opts {
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
  using thresh_prop_delay_diff_r17_e_ = enumerated<thresh_prop_delay_diff_r17_opts>;
  using neigh_cell_info_list_r17_l_   = dyn_array<neighbour_cell_info_r17_s>;

  // member variables
  bool                          thresh_prop_delay_diff_r17_present = false;
  thresh_prop_delay_diff_r17_e_ thresh_prop_delay_diff_r17;
  neigh_cell_info_list_r17_l_   neigh_cell_info_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Q-OffsetRangeList ::= SEQUENCE
struct q_offset_range_list_s {
  bool             rsrp_offset_ssb_present    = false;
  bool             rsrq_offset_ssb_present    = false;
  bool             sinr_offset_ssb_present    = false;
  bool             rsrp_offset_csi_rs_present = false;
  bool             rsrq_offset_csi_rs_present = false;
  bool             sinr_offset_csi_rs_present = false;
  q_offset_range_e rsrp_offset_ssb;
  q_offset_range_e rsrq_offset_ssb;
  q_offset_range_e sinr_offset_ssb;
  q_offset_range_e rsrp_offset_csi_rs;
  q_offset_range_e rsrq_offset_csi_rs;
  q_offset_range_e sinr_offset_csi_rs;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLM-RelaxationReportingConfig-r17 ::= SEQUENCE
struct rlm_relaxation_report_cfg_r17_s {
  struct rlm_relaxtion_report_prohibit_timer_opts {
    enum options {
      s0,
      s0dot5,
      s1,
      s2,
      s5,
      s10,
      s20,
      s30,
      s60,
      s90,
      s120,
      s300,
      s600,
      infinity,
      spare2,
      spare1,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using rlm_relaxtion_report_prohibit_timer_e_ = enumerated<rlm_relaxtion_report_prohibit_timer_opts>;

  // member variables
  rlm_relaxtion_report_prohibit_timer_e_ rlm_relaxtion_report_prohibit_timer;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRM-MeasRelaxationReportingConfig-r17 ::= SEQUENCE
struct rrm_meas_relaxation_report_cfg_r17_s {
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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RSSI-ResourceConfigCLI-r16 ::= SEQUENCE
struct rssi_res_cfg_cli_r16_s {
  bool                              ext                           = false;
  bool                              ref_serv_cell_idx_r16_present = false;
  uint8_t                           rssi_res_id_r16               = 0;
  subcarrier_spacing_e              rssi_scs_r16;
  uint16_t                          start_prb_r16      = 0;
  uint16_t                          nrof_prbs_r16      = 4;
  uint8_t                           start_position_r16 = 0;
  uint8_t                           nrof_symbols_r16   = 1;
  rssi_periodicity_and_offset_r16_c rssi_periodicity_and_offset_r16;
  uint8_t                           ref_serv_cell_idx_r16 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-DeactivationPreferenceConfig-r17 ::= SEQUENCE
struct scg_deactivation_pref_cfg_r17_s {
  struct scg_deactivation_pref_prohibit_timer_r17_opts {
    enum options { s0, s1, s2, s4, s8, s10, s15, s30, s60, s120, s180, s240, s300, s600, s900, s1800, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using scg_deactivation_pref_prohibit_timer_r17_e_ = enumerated<scg_deactivation_pref_prohibit_timer_r17_opts>;

  // member variables
  scg_deactivation_pref_prohibit_timer_r17_e_ scg_deactivation_pref_prohibit_timer_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-RemoteUE-ToAddMod-r17 ::= SEQUENCE
struct sl_remote_ue_to_add_mod_r17_s {
  bool                ext                           = false;
  bool                sl_srap_cfg_relay_r17_present = false;
  fixed_bitstring<24> sl_l2_id_remote_r17;
  sl_srap_cfg_r17_s   sl_srap_cfg_relay_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-ResourceConfigCLI-r16 ::= SEQUENCE
struct srs_res_cfg_cli_r16_s {
  bool                 ext                           = false;
  bool                 ref_serv_cell_idx_r16_present = false;
  srs_res_s            srs_res_r16;
  subcarrier_spacing_e srs_scs_r16;
  uint8_t              ref_serv_cell_idx_r16 = 0;
  uint8_t              ref_bwp_r16           = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-PositionQCL-CellList-r17 ::= SEQUENCE (SIZE (1..32)) OF SSB-PositionQCL-Cell-r17
using ssb_position_qcl_cell_list_r17_l = dyn_array<ssb_position_qcl_cell_r17_s>;

// SSB-PositionQCL-CellsToAddModList-r16 ::= SEQUENCE (SIZE (1..32)) OF SSB-PositionQCL-CellsToAddMod-r16
using ssb_position_qcl_cells_to_add_mod_list_r16_l = dyn_array<ssb_position_qcl_cells_to_add_mod_r16_s>;

// SuccessHO-Config-r17 ::= SEQUENCE
struct success_ho_cfg_r17_s {
  struct thres_percentage_t304_r17_opts {
    enum options { p40, p60, p80, spare5, spare4, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using thres_percentage_t304_r17_e_ = enumerated<thres_percentage_t304_r17_opts>;
  struct thres_percentage_t310_r17_opts {
    enum options { p40, p60, p80, spare5, spare4, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using thres_percentage_t310_r17_e_ = enumerated<thres_percentage_t310_r17_opts>;
  struct thres_percentage_t312_r17_opts {
    enum options { p20, p40, p60, p80, spare4, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using thres_percentage_t312_r17_e_ = enumerated<thres_percentage_t312_r17_opts>;

  // member variables
  bool                         ext                                 = false;
  bool                         thres_percentage_t304_r17_present   = false;
  bool                         thres_percentage_t310_r17_present   = false;
  bool                         thres_percentage_t312_r17_present   = false;
  bool                         source_daps_fail_report_r17_present = false;
  thres_percentage_t304_r17_e_ thres_percentage_t304_r17;
  thres_percentage_t310_r17_e_ thres_percentage_t310_r17;
  thres_percentage_t312_r17_e_ thres_percentage_t312_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AppLayerMeasConfig-r17 ::= SEQUENCE
struct app_layer_meas_cfg_r17_s {
  using meas_cfg_app_layer_to_add_mod_list_r17_l_ = dyn_array<meas_cfg_app_layer_r17_s>;
  using meas_cfg_app_layer_to_release_list_r17_l_ = bounded_array<uint8_t, 16>;

  // member variables
  bool                                      ext                         = false;
  bool                                      rrc_seg_allowed_r17_present = false;
  meas_cfg_app_layer_to_add_mod_list_r17_l_ meas_cfg_app_layer_to_add_mod_list_r17;
  meas_cfg_app_layer_to_release_list_r17_l_ meas_cfg_app_layer_to_release_list_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BAP-RoutingID-r16 ::= SEQUENCE
struct bap_routing_id_r16_s {
  fixed_bitstring<10> bap_address_r16;
  fixed_bitstring<10> bap_path_id_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BT-NameList-r16 ::= SEQUENCE (SIZE (1..4)) OF OCTET STRING (SIZE (1..248))
using bt_name_list_r16_l = bounded_array<bounded_octstring<1, 248>, 4>;

// CSI-RS-ResourceConfigMobility ::= SEQUENCE
struct csi_rs_res_cfg_mob_s {
  using csi_rs_cell_list_mob_l_ = dyn_array<csi_rs_cell_mob_s>;

  // member variables
  bool                    ext = false;
  subcarrier_spacing_e    subcarrier_spacing;
  csi_rs_cell_list_mob_l_ csi_rs_cell_list_mob;
  // ...
  // group 0
  bool    ref_serv_cell_idx_present = false;
  uint8_t ref_serv_cell_idx         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellsToAddMod ::= SEQUENCE
struct cells_to_add_mod_s {
  uint16_t              pci = 0;
  q_offset_range_list_s cell_individual_offset;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellsToAddModExt-v1710 ::= SEQUENCE
struct cells_to_add_mod_ext_v1710_s {
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
  bool                       ntn_polarization_dl_r17_present = false;
  bool                       ntn_polarization_ul_r17_present = false;
  ntn_polarization_dl_r17_e_ ntn_polarization_dl_r17;
  ntn_polarization_ul_r17_e_ ntn_polarization_ul_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellsToAddModUTRA-FDD-r16 ::= SEQUENCE
struct cells_to_add_mod_utra_fdd_r16_s {
  uint8_t  cell_idx_utra_fdd_r16 = 1;
  uint16_t pci_r16               = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CondReconfigToAddModList-r16 ::= SEQUENCE (SIZE (1..8)) OF CondReconfigToAddMod-r16
using cond_recfg_to_add_mod_list_r16_l = dyn_array<cond_recfg_to_add_mod_r16_s>;

// CondReconfigToRemoveList-r16 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..8)
using cond_recfg_to_rem_list_r16_l = bounded_array<uint8_t, 8>;

// DRX-PreferenceConfig-r16 ::= SEQUENCE
struct drx_pref_cfg_r16_s {
  struct drx_pref_prohibit_timer_r16_opts {
    enum options { s0, s0dot5, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s20, s30, spare2, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using drx_pref_prohibit_timer_r16_e_ = enumerated<drx_pref_prohibit_timer_r16_opts>;

  // member variables
  drx_pref_prohibit_timer_r16_e_ drx_pref_prohibit_timer_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IAB-IP-AddressConfiguration-r16 ::= SEQUENCE
struct iab_ip_address_cfg_r16_s {
  bool                 ext                                  = false;
  bool                 iab_ip_address_r16_present           = false;
  bool                 iab_ip_usage_r16_present             = false;
  bool                 iab_donor_du_bap_address_r16_present = false;
  uint8_t              iab_ip_address_idx_r16               = 1;
  iab_ip_address_r16_c iab_ip_address_r16;
  iab_ip_usage_r16_e   iab_ip_usage_r16;
  fixed_bitstring<10>  iab_donor_du_bap_address_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IDC-AssistanceConfig-r16 ::= SEQUENCE
struct idc_assist_cfg_r16_s {
  bool                                 ext = false;
  candidate_serving_freq_list_nr_r16_l candidate_serving_freq_list_nr_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MUSIM-GapConfig-r17 ::= SEQUENCE
struct mu_si_m_gap_cfg_r17_s {
  using musim_gap_to_release_list_r17_l_ = bounded_array<uint8_t, 3>;
  using musim_gap_to_add_mod_list_r17_l_ = dyn_array<mu_si_m_gap_r17_s>;

  // member variables
  bool                             ext                             = false;
  bool                             musim_aperiodic_gap_r17_present = false;
  musim_gap_to_release_list_r17_l_ musim_gap_to_release_list_r17;
  musim_gap_to_add_mod_list_r17_l_ musim_gap_to_add_mod_list_r17;
  mu_si_m_gap_info_r17_s           musim_aperiodic_gap_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MaxBW-PreferenceConfig-r16 ::= SEQUENCE
struct max_bw_pref_cfg_r16_s {
  struct max_bw_pref_prohibit_timer_r16_opts {
    enum options { s0, s0dot5, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s20, s30, spare2, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using max_bw_pref_prohibit_timer_r16_e_ = enumerated<max_bw_pref_prohibit_timer_r16_opts>;

  // member variables
  max_bw_pref_prohibit_timer_r16_e_ max_bw_pref_prohibit_timer_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MaxCC-PreferenceConfig-r16 ::= SEQUENCE
struct max_cc_pref_cfg_r16_s {
  struct max_cc_pref_prohibit_timer_r16_opts {
    enum options { s0, s0dot5, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s20, s30, spare2, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using max_cc_pref_prohibit_timer_r16_e_ = enumerated<max_cc_pref_prohibit_timer_r16_opts>;

  // member variables
  max_cc_pref_prohibit_timer_r16_e_ max_cc_pref_prohibit_timer_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MaxMIMO-LayerPreferenceConfig-r16 ::= SEQUENCE
struct max_mimo_layer_pref_cfg_r16_s {
  struct max_mimo_layer_pref_prohibit_timer_r16_opts {
    enum options { s0, s0dot5, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s20, s30, spare2, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using max_mimo_layer_pref_prohibit_timer_r16_e_ = enumerated<max_mimo_layer_pref_prohibit_timer_r16_opts>;

  // member variables
  max_mimo_layer_pref_prohibit_timer_r16_e_ max_mimo_layer_pref_prohibit_timer_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasRSSI-ReportConfig-r16 ::= SEQUENCE
struct meas_rssi_report_cfg_r16_s {
  bool    ch_occupancy_thres_r16_present = false;
  uint8_t ch_occupancy_thres_r16         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasReportQuantity ::= SEQUENCE
struct meas_report_quant_s {
  bool rsrp = false;
  bool rsrq = false;
  bool sinr = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasReportQuantity-r16 ::= SEQUENCE
struct meas_report_quant_r16_s {
  bool ext     = false;
  bool cbr_r16 = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasReportQuantityCLI-r16 ::= ENUMERATED
struct meas_report_quant_cli_r16_opts {
  enum options { srs_rsrp, cli_rssi, nulltype } value;

  const char* to_string() const;
};
using meas_report_quant_cli_r16_e = enumerated<meas_report_quant_cli_r16_opts>;

// MeasReportQuantityUTRA-FDD-r16 ::= SEQUENCE
struct meas_report_quant_utra_fdd_r16_s {
  bool cpich_rs_cp = false;
  bool cpich_ec_n0 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasTriggerQuantityCLI-r16 ::= CHOICE
struct meas_trigger_quant_cli_r16_c {
  struct types_opts {
    enum options { srs_rsrp_r16, cli_rssi_r16, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  meas_trigger_quant_cli_r16_c() = default;
  meas_trigger_quant_cli_r16_c(const meas_trigger_quant_cli_r16_c& other);
  meas_trigger_quant_cli_r16_c& operator=(const meas_trigger_quant_cli_r16_c& other);
  ~meas_trigger_quant_cli_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& srs_rsrp_r16()
  {
    assert_choice_type(types::srs_rsrp_r16, type_, "MeasTriggerQuantityCLI-r16");
    return c.get<uint8_t>();
  }
  uint8_t& cli_rssi_r16()
  {
    assert_choice_type(types::cli_rssi_r16, type_, "MeasTriggerQuantityCLI-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& srs_rsrp_r16() const
  {
    assert_choice_type(types::srs_rsrp_r16, type_, "MeasTriggerQuantityCLI-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& cli_rssi_r16() const
  {
    assert_choice_type(types::cli_rssi_r16, type_, "MeasTriggerQuantityCLI-r16");
    return c.get<uint8_t>();
  }
  uint8_t& set_srs_rsrp_r16();
  uint8_t& set_cli_rssi_r16();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// MeasTriggerQuantityEUTRA ::= CHOICE
struct meas_trigger_quant_eutra_c {
  struct types_opts {
    enum options { rsrp, rsrq, sinr, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  meas_trigger_quant_eutra_c() = default;
  meas_trigger_quant_eutra_c(const meas_trigger_quant_eutra_c& other);
  meas_trigger_quant_eutra_c& operator=(const meas_trigger_quant_eutra_c& other);
  ~meas_trigger_quant_eutra_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& rsrp()
  {
    assert_choice_type(types::rsrp, type_, "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  uint8_t& rsrq()
  {
    assert_choice_type(types::rsrq, type_, "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  uint8_t& sinr()
  {
    assert_choice_type(types::sinr, type_, "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  const uint8_t& rsrp() const
  {
    assert_choice_type(types::rsrp, type_, "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  const uint8_t& rsrq() const
  {
    assert_choice_type(types::rsrq, type_, "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  const uint8_t& sinr() const
  {
    assert_choice_type(types::sinr, type_, "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  uint8_t& set_rsrp();
  uint8_t& set_rsrq();
  uint8_t& set_sinr();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// MeasTriggerQuantityUTRA-FDD-r16 ::= CHOICE
struct meas_trigger_quant_utra_fdd_r16_c {
  struct types_opts {
    enum options { utra_fdd_rs_cp_r16, utra_fdd_ec_n0_r16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  meas_trigger_quant_utra_fdd_r16_c() = default;
  meas_trigger_quant_utra_fdd_r16_c(const meas_trigger_quant_utra_fdd_r16_c& other);
  meas_trigger_quant_utra_fdd_r16_c& operator=(const meas_trigger_quant_utra_fdd_r16_c& other);
  ~meas_trigger_quant_utra_fdd_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  int8_t& utra_fdd_rs_cp_r16()
  {
    assert_choice_type(types::utra_fdd_rs_cp_r16, type_, "MeasTriggerQuantityUTRA-FDD-r16");
    return c.get<int8_t>();
  }
  uint8_t& utra_fdd_ec_n0_r16()
  {
    assert_choice_type(types::utra_fdd_ec_n0_r16, type_, "MeasTriggerQuantityUTRA-FDD-r16");
    return c.get<uint8_t>();
  }
  const int8_t& utra_fdd_rs_cp_r16() const
  {
    assert_choice_type(types::utra_fdd_rs_cp_r16, type_, "MeasTriggerQuantityUTRA-FDD-r16");
    return c.get<int8_t>();
  }
  const uint8_t& utra_fdd_ec_n0_r16() const
  {
    assert_choice_type(types::utra_fdd_ec_n0_r16, type_, "MeasTriggerQuantityUTRA-FDD-r16");
    return c.get<uint8_t>();
  }
  int8_t&  set_utra_fdd_rs_cp_r16();
  uint8_t& set_utra_fdd_ec_n0_r16();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// MinSchedulingOffsetPreferenceConfig-r16 ::= SEQUENCE
struct min_sched_offset_pref_cfg_r16_s {
  struct min_sched_offset_pref_prohibit_timer_r16_opts {
    enum options { s0, s0dot5, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s20, s30, spare2, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using min_sched_offset_pref_prohibit_timer_r16_e_ = enumerated<min_sched_offset_pref_prohibit_timer_r16_opts>;

  // member variables
  min_sched_offset_pref_prohibit_timer_r16_e_ min_sched_offset_pref_prohibit_timer_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeedForGapNCSG-ConfigEUTRA-r17 ::= SEQUENCE
struct need_for_gap_ncsg_cfg_eutra_r17_s {
  using requested_target_band_filt_ncsg_eutra_r17_l_ = dyn_array<uint16_t>;

  // member variables
  requested_target_band_filt_ncsg_eutra_r17_l_ requested_target_band_filt_ncsg_eutra_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeedForGapNCSG-ConfigNR-r17 ::= SEQUENCE
struct need_for_gap_ncsg_cfg_nr_r17_s {
  using requested_target_band_filt_ncsg_nr_r17_l_ = dyn_array<uint16_t>;

  // member variables
  requested_target_band_filt_ncsg_nr_r17_l_ requested_target_band_filt_ncsg_nr_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OtherConfig-v1700 ::= SEQUENCE
struct other_cfg_v1700_s {
  bool                                                  ul_gap_fr2_pref_cfg_r17_present            = false;
  bool                                                  musim_gap_assist_cfg_r17_present           = false;
  bool                                                  musim_leave_assist_cfg_r17_present         = false;
  bool                                                  success_ho_cfg_r17_present                 = false;
  bool                                                  max_bw_pref_cfg_fr2_2_r17_present          = false;
  bool                                                  max_mimo_layer_pref_cfg_fr2_2_r17_present  = false;
  bool                                                  min_sched_offset_pref_cfg_ext_r17_present  = false;
  bool                                                  rlm_relaxation_report_cfg_r17_present      = false;
  bool                                                  bfd_relaxation_report_cfg_r17_present      = false;
  bool                                                  scg_deactivation_pref_cfg_r17_present      = false;
  bool                                                  rrm_meas_relaxation_report_cfg_r17_present = false;
  bool                                                  prop_delay_diff_report_cfg_r17_present     = false;
  setup_release_c<mu_si_m_gap_assist_cfg_r17_s>         musim_gap_assist_cfg_r17;
  setup_release_c<mu_si_m_leave_assist_cfg_r17_s>       musim_leave_assist_cfg_r17;
  setup_release_c<success_ho_cfg_r17_s>                 success_ho_cfg_r17;
  setup_release_c<rlm_relaxation_report_cfg_r17_s>      rlm_relaxation_report_cfg_r17;
  setup_release_c<bfd_relaxation_report_cfg_r17_s>      bfd_relaxation_report_cfg_r17;
  setup_release_c<scg_deactivation_pref_cfg_r17_s>      scg_deactivation_pref_cfg_r17;
  setup_release_c<rrm_meas_relaxation_report_cfg_r17_s> rrm_meas_relaxation_report_cfg_r17;
  setup_release_c<prop_delay_diff_report_cfg_r17_s>     prop_delay_diff_report_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RSSI-ResourceListConfigCLI-r16 ::= SEQUENCE (SIZE (1..64)) OF RSSI-ResourceConfigCLI-r16
using rssi_res_list_cfg_cli_r16_l = dyn_array<rssi_res_cfg_cli_r16_s>;

// ReleasePreferenceConfig-r16 ::= SEQUENCE
struct release_pref_cfg_r16_s {
  struct release_pref_prohibit_timer_r16_opts {
    enum options { s0, s0dot5, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s20, s30, infinity, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using release_pref_prohibit_timer_r16_e_ = enumerated<release_pref_prohibit_timer_r16_opts>;

  // member variables
  bool                               connected_report_present = false;
  release_pref_prohibit_timer_r16_e_ release_pref_prohibit_timer_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RxTxReportInterval-r17 ::= ENUMERATED
struct rx_tx_report_interv_r17_opts {
  enum options {
    ms80,
    ms120,
    ms160,
    ms240,
    ms320,
    ms480,
    ms640,
    ms1024,
    ms1280,
    ms2048,
    ms2560,
    ms5120,
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
using rx_tx_report_interv_r17_e = enumerated<rx_tx_report_interv_r17_opts>;

// SL-L2RelayUE-Config-r17 ::= SEQUENCE
struct sl_l2_relay_ue_cfg_r17_s {
  using sl_remote_ue_to_add_mod_list_r17_l_ = dyn_array<sl_remote_ue_to_add_mod_r17_s>;
  using sl_remote_ue_to_release_list_r17_l_ = bounded_array<fixed_bitstring<24>, 32>;

  // member variables
  bool                                ext = false;
  sl_remote_ue_to_add_mod_list_r17_l_ sl_remote_ue_to_add_mod_list_r17;
  sl_remote_ue_to_release_list_r17_l_ sl_remote_ue_to_release_list_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TimeOffsetEUTRA-r16 ::= ENUMERATED
struct sl_time_offset_eutra_r16_opts {
  enum options {
    ms0,
    ms0dot25,
    ms0dot5,
    ms0dot625,
    ms0dot75,
    ms1,
    ms1dot25,
    ms1dot5,
    ms1dot75,
    ms2,
    ms2dot5,
    ms3,
    ms4,
    ms5,
    ms6,
    ms8,
    ms10,
    ms20,
    nulltype
  } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using sl_time_offset_eutra_r16_e = enumerated<sl_time_offset_eutra_r16_opts>;

// SRS-ResourceListConfigCLI-r16 ::= SEQUENCE (SIZE (1..32)) OF SRS-ResourceConfigCLI-r16
using srs_res_list_cfg_cli_r16_l = dyn_array<srs_res_cfg_cli_r16_s>;

// SSB-ConfigMobility ::= SEQUENCE
struct ssb_cfg_mob_s {
  bool                              ext                    = false;
  bool                              ssb_to_measure_present = false;
  bool                              ss_rssi_meas_present   = false;
  setup_release_c<ssb_to_measure_c> ssb_to_measure;
  bool                              derive_ssb_idx_from_cell = false;
  ss_rssi_meas_s                    ss_rssi_meas;
  // ...
  // group 0
  bool                                                   ssb_position_qcl_common_r16_present = false;
  ssb_position_qcl_relation_r16_e                        ssb_position_qcl_common_r16;
  copy_ptr<ssb_position_qcl_cells_to_add_mod_list_r16_l> ssb_position_qcl_cells_to_add_mod_list_r16;
  copy_ptr<pci_list_l>                                   ssb_position_qcl_cells_to_rem_list_r16;
  // group 1
  bool                            derive_ssb_idx_from_cell_inter_r17_present = false;
  bool                            ssb_position_qcl_common_r17_present        = false;
  uint8_t                         derive_ssb_idx_from_cell_inter_r17         = 0;
  ssb_position_qcl_relation_r17_e ssb_position_qcl_common_r17;
  copy_ptr<setup_release_c<dyn_seq_of<ssb_position_qcl_cell_r17_s, 1, 32>>> ssb_position_qcl_cells_r17;
  // group 2
  copy_ptr<pci_list_l> cca_cells_to_add_mod_list_r17;
  copy_ptr<pci_list_l> cca_cells_to_rem_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-MTC3-r16 ::= SEQUENCE
struct ssb_mtc3_r16_s {
  struct periodicity_and_offset_r16_c_ {
    struct types_opts {
      enum options {
        sf5_r16,
        sf10_r16,
        sf20_r16,
        sf40_r16,
        sf80_r16,
        sf160_r16,
        sf320_r16,
        sf640_r16,
        sf1280_r16,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    periodicity_and_offset_r16_c_() = default;
    periodicity_and_offset_r16_c_(const periodicity_and_offset_r16_c_& other);
    periodicity_and_offset_r16_c_& operator=(const periodicity_and_offset_r16_c_& other);
    ~periodicity_and_offset_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sf5_r16()
    {
      assert_choice_type(types::sf5_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    uint8_t& sf10_r16()
    {
      assert_choice_type(types::sf10_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    uint8_t& sf20_r16()
    {
      assert_choice_type(types::sf20_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    uint8_t& sf40_r16()
    {
      assert_choice_type(types::sf40_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    uint8_t& sf80_r16()
    {
      assert_choice_type(types::sf80_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    uint8_t& sf160_r16()
    {
      assert_choice_type(types::sf160_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    uint16_t& sf320_r16()
    {
      assert_choice_type(types::sf320_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint16_t>();
    }
    uint16_t& sf640_r16()
    {
      assert_choice_type(types::sf640_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint16_t>();
    }
    uint16_t& sf1280_r16()
    {
      assert_choice_type(types::sf1280_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint16_t>();
    }
    const uint8_t& sf5_r16() const
    {
      assert_choice_type(types::sf5_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& sf10_r16() const
    {
      assert_choice_type(types::sf10_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& sf20_r16() const
    {
      assert_choice_type(types::sf20_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& sf40_r16() const
    {
      assert_choice_type(types::sf40_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& sf80_r16() const
    {
      assert_choice_type(types::sf80_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& sf160_r16() const
    {
      assert_choice_type(types::sf160_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    const uint16_t& sf320_r16() const
    {
      assert_choice_type(types::sf320_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint16_t>();
    }
    const uint16_t& sf640_r16() const
    {
      assert_choice_type(types::sf640_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint16_t>();
    }
    const uint16_t& sf1280_r16() const
    {
      assert_choice_type(types::sf1280_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint16_t>();
    }
    uint8_t&  set_sf5_r16();
    uint8_t&  set_sf10_r16();
    uint8_t&  set_sf20_r16();
    uint8_t&  set_sf40_r16();
    uint8_t&  set_sf80_r16();
    uint8_t&  set_sf160_r16();
    uint16_t& set_sf320_r16();
    uint16_t& set_sf640_r16();
    uint16_t& set_sf1280_r16();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct dur_r16_opts {
    enum options { sf1, sf2, sf3, sf4, sf5, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using dur_r16_e_      = enumerated<dur_r16_opts>;
  using pci_list_r16_l_ = dyn_array<uint16_t>;

  // member variables
  bool                              ssb_to_measure_r16_present = false;
  periodicity_and_offset_r16_c_     periodicity_and_offset_r16;
  dur_r16_e_                        dur_r16;
  pci_list_r16_l_                   pci_list_r16;
  setup_release_c<ssb_to_measure_c> ssb_to_measure_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Sensor-NameList-r16 ::= SEQUENCE
struct sensor_name_list_r16_s {
  bool meas_uncom_bar_pre_r16_present = false;
  bool meas_ue_speed_present          = false;
  bool meas_ue_orientation_present    = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-TxTEG-RequestUL-TDOA-Config-r17 ::= CHOICE
struct ue_tx_teg_request_ul_tdoa_cfg_r17_c {
  struct periodic_report_r17_opts {
    enum options { ms160, ms320, ms1280, ms2560, ms61440, ms81920, ms368640, ms737280, nulltype } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  using periodic_report_r17_e_ = enumerated<periodic_report_r17_opts>;
  struct types_opts {
    enum options { one_shot_r17, periodic_report_r17, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ue_tx_teg_request_ul_tdoa_cfg_r17_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  periodic_report_r17_e_& periodic_report_r17()
  {
    assert_choice_type(types::periodic_report_r17, type_, "UE-TxTEG-RequestUL-TDOA-Config-r17");
    return c;
  }
  const periodic_report_r17_e_& periodic_report_r17() const
  {
    assert_choice_type(types::periodic_report_r17, type_, "UE-TxTEG-RequestUL-TDOA-Config-r17");
    return c;
  }
  void                    set_one_shot_r17();
  periodic_report_r17_e_& set_periodic_report_r17();

private:
  types                  type_;
  periodic_report_r17_e_ c;
};

// UL-DelayValueConfig-r16 ::= SEQUENCE
struct ul_delay_value_cfg_r16_s {
  using delay_drb_list_r16_l_ = bounded_array<uint8_t, 29>;

  // member variables
  delay_drb_list_r16_l_ delay_drb_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-ExcessDelayConfig-r17 ::= SEQUENCE
struct ul_excess_delay_cfg_r17_s {
  using excess_delay_drb_list_r17_l_ = dyn_array<excess_delay_drb_id_info_r17_s>;

  // member variables
  excess_delay_drb_list_r17_l_ excess_delay_drb_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-GapFR2-Config-r17 ::= SEQUENCE
struct ul_gap_fr2_cfg_r17_s {
  struct ugl_r17_opts {
    enum options { ms0dot125, ms0dot25, ms0dot5, ms1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using ugl_r17_e_ = enumerated<ugl_r17_opts>;
  struct ugrp_r17_opts {
    enum options { ms5, ms20, ms40, ms160, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ugrp_r17_e_ = enumerated<ugrp_r17_opts>;

  // member variables
  bool        ref_fr2_serv_cell_async_ca_r17_present = false;
  uint8_t     gap_offset_r17                         = 0;
  ugl_r17_e_  ugl_r17;
  ugrp_r17_e_ ugrp_r17;
  uint8_t     ref_fr2_serv_cell_async_ca_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ValidityCellList ::= SEQUENCE (SIZE (1..8)) OF PCI-Range
using validity_cell_list_l = dyn_array<pci_range_s>;

// WLAN-NameList-r16 ::= SEQUENCE (SIZE (1..4)) OF OCTET STRING (SIZE (1..32))
using wlan_name_list_r16_l = bounded_array<bounded_octstring<1, 32>, 4>;

// BAP-Config-r16 ::= SEQUENCE
struct bap_cfg_r16_s {
  struct flow_ctrl_feedback_type_r16_opts {
    enum options { per_bh_rlc_ch, per_routing_id, both, nulltype } value;

    const char* to_string() const;
  };
  using flow_ctrl_feedback_type_r16_e_ = enumerated<flow_ctrl_feedback_type_r16_opts>;

  // member variables
  bool                           ext                                   = false;
  bool                           bap_address_r16_present               = false;
  bool                           default_ul_bap_routing_id_r16_present = false;
  bool                           default_ul_bh_rlc_ch_r16_present      = false;
  bool                           flow_ctrl_feedback_type_r16_present   = false;
  fixed_bitstring<10>            bap_address_r16;
  bap_routing_id_r16_s           default_ul_bap_routing_id_r16;
  fixed_bitstring<16>            default_ul_bh_rlc_ch_r16;
  flow_ctrl_feedback_type_r16_e_ flow_ctrl_feedback_type_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CLI-EventTriggerConfig-r16 ::= SEQUENCE
struct cli_event_trigger_cfg_r16_s {
  struct event_id_r16_c_ {
    struct event_i1_r16_s_ {
      meas_trigger_quant_cli_r16_c i1_thres_r16;
      bool                         report_on_leave_r16 = false;
      uint8_t                      hysteresis_r16      = 0;
      time_to_trigger_e            time_to_trigger_r16;
    };
    struct types_opts {
      enum options { event_i1_r16, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    types       type() const { return types::event_i1_r16; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    event_i1_r16_s_&       event_i1_r16() { return c; }
    const event_i1_r16_s_& event_i1_r16() const { return c; }

  private:
    event_i1_r16_s_ c;
  };
  struct report_amount_r16_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_r16_e_ = enumerated<report_amount_r16_opts>;

  // member variables
  bool                 ext = false;
  event_id_r16_c_      event_id_r16;
  report_interv_e      report_interv_r16;
  report_amount_r16_e_ report_amount_r16;
  uint8_t              max_report_cli_r16 = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CLI-PeriodicalReportConfig-r16 ::= SEQUENCE
struct cli_periodical_report_cfg_r16_s {
  struct report_amount_r16_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_r16_e_ = enumerated<report_amount_r16_opts>;

  // member variables
  bool                        ext = false;
  report_interv_e             report_interv_r16;
  report_amount_r16_e_        report_amount_r16;
  meas_report_quant_cli_r16_e report_quant_cli_r16;
  uint8_t                     max_report_cli_r16 = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CLI-ResourceConfig-r16 ::= SEQUENCE
struct cli_res_cfg_r16_s {
  bool                                                       srs_res_cfg_r16_present  = false;
  bool                                                       rssi_res_cfg_r16_present = false;
  setup_release_c<dyn_seq_of<srs_res_cfg_cli_r16_s, 1, 32>>  srs_res_cfg_r16;
  setup_release_c<dyn_seq_of<rssi_res_cfg_cli_r16_s, 1, 64>> rssi_res_cfg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellsToAddModList ::= SEQUENCE (SIZE (1..32)) OF CellsToAddMod
using cells_to_add_mod_list_l = dyn_array<cells_to_add_mod_s>;

// CellsToAddModListExt-v1710 ::= SEQUENCE (SIZE (1..32)) OF CellsToAddModExt-v1710
using cells_to_add_mod_list_ext_v1710_l = dyn_array<cells_to_add_mod_ext_v1710_s>;

// CellsToAddModListUTRA-FDD-r16 ::= SEQUENCE (SIZE (1..32)) OF CellsToAddModUTRA-FDD-r16
using cells_to_add_mod_list_utra_fdd_r16_l = dyn_array<cells_to_add_mod_utra_fdd_r16_s>;

// ConditionalReconfiguration-r16 ::= SEQUENCE
struct conditional_recfg_r16_s {
  bool                             ext                            = false;
  bool                             attempt_cond_recfg_r16_present = false;
  cond_recfg_to_rem_list_r16_l     cond_recfg_to_rem_list_r16;
  cond_recfg_to_add_mod_list_r16_l cond_recfg_to_add_mod_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-Cell ::= SEQUENCE
struct eutra_cell_s {
  uint8_t                cell_idx_eutra = 1;
  uint16_t               pci            = 0;
  eutra_q_offset_range_e cell_individual_offset;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-CellIndexList ::= SEQUENCE (SIZE (1..32)) OF INTEGER (1..32)
using eutra_cell_idx_list_l = bounded_array<uint8_t, 32>;

// EUTRA-ExcludedCell ::= SEQUENCE
struct eutra_excluded_cell_s {
  uint8_t           cell_idx_eutra = 1;
  eutra_pci_range_s pci_range;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTriggerConfig ::= SEQUENCE
struct event_trigger_cfg_s {
  struct event_id_c_ {
    struct event_a1_s_ {
      meas_trigger_quant_c a1_thres;
      bool                 report_on_leave = false;
      uint8_t              hysteresis      = 0;
      time_to_trigger_e    time_to_trigger;
    };
    struct event_a2_s_ {
      meas_trigger_quant_c a2_thres;
      bool                 report_on_leave = false;
      uint8_t              hysteresis      = 0;
      time_to_trigger_e    time_to_trigger;
    };
    struct event_a3_s_ {
      meas_trigger_quant_offset_c a3_offset;
      bool                        report_on_leave = false;
      uint8_t                     hysteresis      = 0;
      time_to_trigger_e           time_to_trigger;
      bool                        use_allowed_cell_list = false;
    };
    struct event_a4_s_ {
      meas_trigger_quant_c a4_thres;
      bool                 report_on_leave = false;
      uint8_t              hysteresis      = 0;
      time_to_trigger_e    time_to_trigger;
      bool                 use_allowed_cell_list = false;
    };
    struct event_a5_s_ {
      meas_trigger_quant_c a5_thres1;
      meas_trigger_quant_c a5_thres2;
      bool                 report_on_leave = false;
      uint8_t              hysteresis      = 0;
      time_to_trigger_e    time_to_trigger;
      bool                 use_allowed_cell_list = false;
    };
    struct event_a6_s_ {
      meas_trigger_quant_offset_c a6_offset;
      bool                        report_on_leave = false;
      uint8_t                     hysteresis      = 0;
      time_to_trigger_e           time_to_trigger;
      bool                        use_allowed_cell_list = false;
    };
    struct event_x1_r17_s_ {
      sl_meas_trigger_quant_r16_c x1_thres1_relay_r17;
      meas_trigger_quant_c        x1_thres2_r17;
      bool                        report_on_leave_r17 = false;
      uint8_t                     hysteresis_r17      = 0;
      time_to_trigger_e           time_to_trigger_r17;
      bool                        use_allowed_cell_list_r17 = false;
    };
    struct event_x2_r17_s_ {
      sl_meas_trigger_quant_r16_c x2_thres_relay_r17;
      bool                        report_on_leave_r17 = false;
      uint8_t                     hysteresis_r17      = 0;
      time_to_trigger_e           time_to_trigger_r17;
    };
    struct event_d1_r17_s_ {
      uint16_t          distance_thresh_from_ref1_r17 = 1;
      uint16_t          distance_thresh_from_ref2_r17 = 1;
      dyn_octstring     ref_location1_r17;
      dyn_octstring     ref_location2_r17;
      bool              report_on_leave_r17     = false;
      uint16_t          hysteresis_location_r17 = 0;
      time_to_trigger_e time_to_trigger_r17;
    };
    struct types_opts {
      enum options {
        event_a1,
        event_a2,
        event_a3,
        event_a4,
        event_a5,
        event_a6,
        // ...
        event_x1_r17,
        event_x2_r17,
        event_d1_r17,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true, 3>;

    // choice methods
    event_id_c_() = default;
    event_id_c_(const event_id_c_& other);
    event_id_c_& operator=(const event_id_c_& other);
    ~event_id_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    event_a1_s_& event_a1()
    {
      assert_choice_type(types::event_a1, type_, "eventId");
      return c.get<event_a1_s_>();
    }
    event_a2_s_& event_a2()
    {
      assert_choice_type(types::event_a2, type_, "eventId");
      return c.get<event_a2_s_>();
    }
    event_a3_s_& event_a3()
    {
      assert_choice_type(types::event_a3, type_, "eventId");
      return c.get<event_a3_s_>();
    }
    event_a4_s_& event_a4()
    {
      assert_choice_type(types::event_a4, type_, "eventId");
      return c.get<event_a4_s_>();
    }
    event_a5_s_& event_a5()
    {
      assert_choice_type(types::event_a5, type_, "eventId");
      return c.get<event_a5_s_>();
    }
    event_a6_s_& event_a6()
    {
      assert_choice_type(types::event_a6, type_, "eventId");
      return c.get<event_a6_s_>();
    }
    event_x1_r17_s_& event_x1_r17()
    {
      assert_choice_type(types::event_x1_r17, type_, "eventId");
      return c.get<event_x1_r17_s_>();
    }
    event_x2_r17_s_& event_x2_r17()
    {
      assert_choice_type(types::event_x2_r17, type_, "eventId");
      return c.get<event_x2_r17_s_>();
    }
    event_d1_r17_s_& event_d1_r17()
    {
      assert_choice_type(types::event_d1_r17, type_, "eventId");
      return c.get<event_d1_r17_s_>();
    }
    const event_a1_s_& event_a1() const
    {
      assert_choice_type(types::event_a1, type_, "eventId");
      return c.get<event_a1_s_>();
    }
    const event_a2_s_& event_a2() const
    {
      assert_choice_type(types::event_a2, type_, "eventId");
      return c.get<event_a2_s_>();
    }
    const event_a3_s_& event_a3() const
    {
      assert_choice_type(types::event_a3, type_, "eventId");
      return c.get<event_a3_s_>();
    }
    const event_a4_s_& event_a4() const
    {
      assert_choice_type(types::event_a4, type_, "eventId");
      return c.get<event_a4_s_>();
    }
    const event_a5_s_& event_a5() const
    {
      assert_choice_type(types::event_a5, type_, "eventId");
      return c.get<event_a5_s_>();
    }
    const event_a6_s_& event_a6() const
    {
      assert_choice_type(types::event_a6, type_, "eventId");
      return c.get<event_a6_s_>();
    }
    const event_x1_r17_s_& event_x1_r17() const
    {
      assert_choice_type(types::event_x1_r17, type_, "eventId");
      return c.get<event_x1_r17_s_>();
    }
    const event_x2_r17_s_& event_x2_r17() const
    {
      assert_choice_type(types::event_x2_r17, type_, "eventId");
      return c.get<event_x2_r17_s_>();
    }
    const event_d1_r17_s_& event_d1_r17() const
    {
      assert_choice_type(types::event_d1_r17, type_, "eventId");
      return c.get<event_d1_r17_s_>();
    }
    event_a1_s_&     set_event_a1();
    event_a2_s_&     set_event_a2();
    event_a3_s_&     set_event_a3();
    event_a4_s_&     set_event_a4();
    event_a5_s_&     set_event_a5();
    event_a6_s_&     set_event_a6();
    event_x1_r17_s_& set_event_x1_r17();
    event_x2_r17_s_& set_event_x2_r17();
    event_d1_r17_s_& set_event_d1_r17();

  private:
    types type_;
    choice_buffer_t<event_a1_s_,
                    event_a2_s_,
                    event_a3_s_,
                    event_a4_s_,
                    event_a5_s_,
                    event_a6_s_,
                    event_d1_r17_s_,
                    event_x1_r17_s_,
                    event_x2_r17_s_>
        c;

    void destroy_();
  };
  struct report_amount_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_e_ = enumerated<report_amount_opts>;

  // member variables
  bool                ext                                 = false;
  bool                report_quant_rs_idxes_present       = false;
  bool                max_nrof_rs_idxes_to_report_present = false;
  bool                report_add_neigh_meas_present       = false;
  event_id_c_         event_id;
  nr_rs_type_e        rs_type;
  report_interv_e     report_interv;
  report_amount_e_    report_amount;
  meas_report_quant_s report_quant_cell;
  uint8_t             max_report_cells = 1;
  meas_report_quant_s report_quant_rs_idxes;
  uint8_t             max_nrof_rs_idxes_to_report = 1;
  bool                include_beam_meass          = false;
  // ...
  // group 0
  bool                                 use_t312_r16_present                     = false;
  bool                                 include_common_location_info_r16_present = false;
  copy_ptr<meas_rssi_report_cfg_r16_s> meas_rssi_report_cfg_r16;
  bool                                 use_t312_r16 = false;
  copy_ptr<setup_release_c<dyn_seq_of<bounded_octstring<1, 248>, 1, 4>>> include_bt_meas_r16;
  copy_ptr<setup_release_c<dyn_seq_of<bounded_octstring<1, 32>, 1, 4>>>  include_wlan_meas_r16;
  copy_ptr<setup_release_c<sensor_name_list_r16_s>>                      include_sensor_meas_r16;
  // group 1
  bool                                 coarse_location_request_r17_present = false;
  copy_ptr<sl_meas_report_quant_r16_c> report_quant_relay_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTriggerConfigInterRAT ::= SEQUENCE
struct event_trigger_cfg_inter_rat_s {
  struct event_id_c_ {
    struct event_b1_s_ {
      bool                       ext = false;
      meas_trigger_quant_eutra_c b1_thres_eutra;
      bool                       report_on_leave = false;
      uint8_t                    hysteresis      = 0;
      time_to_trigger_e          time_to_trigger;
      // ...
    };
    struct event_b2_s_ {
      bool                       ext = false;
      meas_trigger_quant_c       b2_thres1;
      meas_trigger_quant_eutra_c b2_thres2_eutra;
      bool                       report_on_leave = false;
      uint8_t                    hysteresis      = 0;
      time_to_trigger_e          time_to_trigger;
      // ...
    };
    struct event_b1_utra_fdd_r16_s_ {
      bool                              ext = false;
      meas_trigger_quant_utra_fdd_r16_c b1_thres_utra_fdd_r16;
      bool                              report_on_leave_r16 = false;
      uint8_t                           hysteresis_r16      = 0;
      time_to_trigger_e                 time_to_trigger_r16;
      // ...
    };
    struct event_b2_utra_fdd_r16_s_ {
      bool                              ext = false;
      meas_trigger_quant_c              b2_thres1_r16;
      meas_trigger_quant_utra_fdd_r16_c b2_thres2_utra_fdd_r16;
      bool                              report_on_leave_r16 = false;
      uint8_t                           hysteresis_r16      = 0;
      time_to_trigger_e                 time_to_trigger_r16;
      // ...
    };
    struct event_y1_relay_r17_s_ {
      bool                        ext = false;
      meas_trigger_quant_c        y1_thres1_r17;
      sl_meas_trigger_quant_r16_c y1_thres2_relay_r17;
      bool                        report_on_leave_r17 = false;
      uint8_t                     hysteresis_r17      = 0;
      time_to_trigger_e           time_to_trigger_r17;
      // ...
    };
    struct event_y2_relay_r17_s_ {
      bool                        ext = false;
      sl_meas_trigger_quant_r16_c y2_thres_relay_r17;
      bool                        report_on_leave_r17 = false;
      uint8_t                     hysteresis_r17      = 0;
      time_to_trigger_e           time_to_trigger_r17;
      // ...
    };
    struct types_opts {
      enum options {
        event_b1,
        event_b2,
        // ...
        event_b1_utra_fdd_r16,
        event_b2_utra_fdd_r16,
        event_y1_relay_r17,
        event_y2_relay_r17,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true, 4>;

    // choice methods
    event_id_c_() = default;
    event_id_c_(const event_id_c_& other);
    event_id_c_& operator=(const event_id_c_& other);
    ~event_id_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    event_b1_s_& event_b1()
    {
      assert_choice_type(types::event_b1, type_, "eventId");
      return c.get<event_b1_s_>();
    }
    event_b2_s_& event_b2()
    {
      assert_choice_type(types::event_b2, type_, "eventId");
      return c.get<event_b2_s_>();
    }
    event_b1_utra_fdd_r16_s_& event_b1_utra_fdd_r16()
    {
      assert_choice_type(types::event_b1_utra_fdd_r16, type_, "eventId");
      return c.get<event_b1_utra_fdd_r16_s_>();
    }
    event_b2_utra_fdd_r16_s_& event_b2_utra_fdd_r16()
    {
      assert_choice_type(types::event_b2_utra_fdd_r16, type_, "eventId");
      return c.get<event_b2_utra_fdd_r16_s_>();
    }
    event_y1_relay_r17_s_& event_y1_relay_r17()
    {
      assert_choice_type(types::event_y1_relay_r17, type_, "eventId");
      return c.get<event_y1_relay_r17_s_>();
    }
    event_y2_relay_r17_s_& event_y2_relay_r17()
    {
      assert_choice_type(types::event_y2_relay_r17, type_, "eventId");
      return c.get<event_y2_relay_r17_s_>();
    }
    const event_b1_s_& event_b1() const
    {
      assert_choice_type(types::event_b1, type_, "eventId");
      return c.get<event_b1_s_>();
    }
    const event_b2_s_& event_b2() const
    {
      assert_choice_type(types::event_b2, type_, "eventId");
      return c.get<event_b2_s_>();
    }
    const event_b1_utra_fdd_r16_s_& event_b1_utra_fdd_r16() const
    {
      assert_choice_type(types::event_b1_utra_fdd_r16, type_, "eventId");
      return c.get<event_b1_utra_fdd_r16_s_>();
    }
    const event_b2_utra_fdd_r16_s_& event_b2_utra_fdd_r16() const
    {
      assert_choice_type(types::event_b2_utra_fdd_r16, type_, "eventId");
      return c.get<event_b2_utra_fdd_r16_s_>();
    }
    const event_y1_relay_r17_s_& event_y1_relay_r17() const
    {
      assert_choice_type(types::event_y1_relay_r17, type_, "eventId");
      return c.get<event_y1_relay_r17_s_>();
    }
    const event_y2_relay_r17_s_& event_y2_relay_r17() const
    {
      assert_choice_type(types::event_y2_relay_r17, type_, "eventId");
      return c.get<event_y2_relay_r17_s_>();
    }
    event_b1_s_&              set_event_b1();
    event_b2_s_&              set_event_b2();
    event_b1_utra_fdd_r16_s_& set_event_b1_utra_fdd_r16();
    event_b2_utra_fdd_r16_s_& set_event_b2_utra_fdd_r16();
    event_y1_relay_r17_s_&    set_event_y1_relay_r17();
    event_y2_relay_r17_s_&    set_event_y2_relay_r17();

  private:
    types type_;
    choice_buffer_t<event_b1_s_,
                    event_b1_utra_fdd_r16_s_,
                    event_b2_s_,
                    event_b2_utra_fdd_r16_s_,
                    event_y1_relay_r17_s_,
                    event_y2_relay_r17_s_>
        c;

    void destroy_();
  };
  struct report_amount_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_e_ = enumerated<report_amount_opts>;

  // member variables
  bool                ext = false;
  event_id_c_         event_id;
  nr_rs_type_e        rs_type;
  report_interv_e     report_interv;
  report_amount_e_    report_amount;
  meas_report_quant_s report_quant;
  uint8_t             max_report_cells = 1;
  // ...
  // group 0
  copy_ptr<meas_report_quant_utra_fdd_r16_s> report_quant_utra_fdd_r16;
  // group 1
  bool include_common_location_info_r16_present = false;
  copy_ptr<setup_release_c<dyn_seq_of<bounded_octstring<1, 248>, 1, 4>>> include_bt_meas_r16;
  copy_ptr<setup_release_c<dyn_seq_of<bounded_octstring<1, 32>, 1, 4>>>  include_wlan_meas_r16;
  copy_ptr<setup_release_c<sensor_name_list_r16_s>>                      include_sensor_meas_r16;
  // group 2
  copy_ptr<sl_meas_report_quant_r16_c> report_quant_relay_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTriggerConfigNR-SL-r16 ::= SEQUENCE
struct event_trigger_cfg_nr_sl_r16_s {
  struct event_id_r16_c_ {
    struct event_c1_s_ {
      uint8_t           c1_thres_r16   = 0;
      uint8_t           hysteresis_r16 = 0;
      time_to_trigger_e time_to_trigger_r16;
    };
    struct event_c2_r16_s_ {
      uint8_t           c2_thres_r16   = 0;
      uint8_t           hysteresis_r16 = 0;
      time_to_trigger_e time_to_trigger_r16;
    };
    struct types_opts {
      enum options { event_c1, event_c2_r16, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    event_id_r16_c_() = default;
    event_id_r16_c_(const event_id_r16_c_& other);
    event_id_r16_c_& operator=(const event_id_r16_c_& other);
    ~event_id_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    event_c1_s_& event_c1()
    {
      assert_choice_type(types::event_c1, type_, "eventId-r16");
      return c.get<event_c1_s_>();
    }
    event_c2_r16_s_& event_c2_r16()
    {
      assert_choice_type(types::event_c2_r16, type_, "eventId-r16");
      return c.get<event_c2_r16_s_>();
    }
    const event_c1_s_& event_c1() const
    {
      assert_choice_type(types::event_c1, type_, "eventId-r16");
      return c.get<event_c1_s_>();
    }
    const event_c2_r16_s_& event_c2_r16() const
    {
      assert_choice_type(types::event_c2_r16, type_, "eventId-r16");
      return c.get<event_c2_r16_s_>();
    }
    event_c1_s_&     set_event_c1();
    event_c2_r16_s_& set_event_c2_r16();

  private:
    types                                         type_;
    choice_buffer_t<event_c1_s_, event_c2_r16_s_> c;

    void destroy_();
  };
  struct report_amount_r16_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_r16_e_ = enumerated<report_amount_r16_opts>;

  // member variables
  bool                    ext = false;
  event_id_r16_c_         event_id_r16;
  report_interv_e         report_interv_r16;
  report_amount_r16_e_    report_amount_r16;
  meas_report_quant_r16_s report_quant_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FilterConfig ::= SEQUENCE
struct filt_cfg_s {
  bool        filt_coef_rsrp_present    = false;
  bool        filt_coef_rsrq_present    = false;
  bool        filt_coef_rs_sinr_present = false;
  filt_coef_e filt_coef_rsrp;
  filt_coef_e filt_coef_rsrq;
  filt_coef_e filt_coef_rs_sinr;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IAB-IP-AddressConfigurationList-r16 ::= SEQUENCE
struct iab_ip_address_cfg_list_r16_s {
  using iab_ip_address_to_add_mod_list_r16_l_ = dyn_array<iab_ip_address_cfg_r16_s>;
  using iab_ip_address_to_release_list_r16_l_ = bounded_array<uint8_t, 32>;

  // member variables
  bool                                  ext = false;
  iab_ip_address_to_add_mod_list_r16_l_ iab_ip_address_to_add_mod_list_r16;
  iab_ip_address_to_release_list_r16_l_ iab_ip_address_to_release_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeedForGapsConfigNR-r16 ::= SEQUENCE
struct need_for_gaps_cfg_nr_r16_s {
  using requested_target_band_filt_nr_r16_l_ = dyn_array<uint16_t>;

  // member variables
  requested_target_band_filt_nr_r16_l_ requested_target_band_filt_nr_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OnDemandSIB-Request-r16 ::= SEQUENCE
struct on_demand_sib_request_r16_s {
  struct on_demand_sib_request_prohibit_timer_r16_opts {
    enum options { s0, s0dot5, s1, s2, s5, s10, s20, s30, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using on_demand_sib_request_prohibit_timer_r16_e_ = enumerated<on_demand_sib_request_prohibit_timer_r16_opts>;

  // member variables
  on_demand_sib_request_prohibit_timer_r16_e_ on_demand_sib_request_prohibit_timer_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OtherConfig-v1610 ::= SEQUENCE
struct other_cfg_v1610_s {
  bool                                                         idc_assist_cfg_r16_present            = false;
  bool                                                         drx_pref_cfg_r16_present              = false;
  bool                                                         max_bw_pref_cfg_r16_present           = false;
  bool                                                         max_cc_pref_cfg_r16_present           = false;
  bool                                                         max_mimo_layer_pref_cfg_r16_present   = false;
  bool                                                         min_sched_offset_pref_cfg_r16_present = false;
  bool                                                         release_pref_cfg_r16_present          = false;
  bool                                                         ref_time_pref_report_r16_present      = false;
  bool                                                         bt_name_list_r16_present              = false;
  bool                                                         wlan_name_list_r16_present            = false;
  bool                                                         sensor_name_list_r16_present          = false;
  bool                                                         obtain_common_location_r16_present    = false;
  bool                                                         sl_assist_cfg_nr_r16_present          = false;
  setup_release_c<idc_assist_cfg_r16_s>                        idc_assist_cfg_r16;
  setup_release_c<drx_pref_cfg_r16_s>                          drx_pref_cfg_r16;
  setup_release_c<max_bw_pref_cfg_r16_s>                       max_bw_pref_cfg_r16;
  setup_release_c<max_cc_pref_cfg_r16_s>                       max_cc_pref_cfg_r16;
  setup_release_c<max_mimo_layer_pref_cfg_r16_s>               max_mimo_layer_pref_cfg_r16;
  setup_release_c<min_sched_offset_pref_cfg_r16_s>             min_sched_offset_pref_cfg_r16;
  setup_release_c<release_pref_cfg_r16_s>                      release_pref_cfg_r16;
  setup_release_c<dyn_seq_of<bounded_octstring<1, 248>, 1, 4>> bt_name_list_r16;
  setup_release_c<dyn_seq_of<bounded_octstring<1, 32>, 1, 4>>  wlan_name_list_r16;
  setup_release_c<sensor_name_list_r16_s>                      sensor_name_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCI-RangeElement ::= SEQUENCE
struct pci_range_elem_s {
  uint8_t     pci_range_idx = 1;
  pci_range_s pci_range;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCI-RangeIndexList ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..8)
using pci_range_idx_list_l = bounded_array<uint8_t, 8>;

// PeriodicalReportConfig ::= SEQUENCE
struct periodical_report_cfg_s {
  struct report_amount_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_e_ = enumerated<report_amount_opts>;

  // member variables
  bool                ext                                 = false;
  bool                report_quant_rs_idxes_present       = false;
  bool                max_nrof_rs_idxes_to_report_present = false;
  nr_rs_type_e        rs_type;
  report_interv_e     report_interv;
  report_amount_e_    report_amount;
  meas_report_quant_s report_quant_cell;
  uint8_t             max_report_cells = 1;
  meas_report_quant_s report_quant_rs_idxes;
  uint8_t             max_nrof_rs_idxes_to_report = 1;
  bool                include_beam_meass          = false;
  bool                use_allowed_cell_list       = false;
  // ...
  // group 0
  bool                                 include_common_location_info_r16_present = false;
  bool                                 report_add_neigh_meas_r16_present        = false;
  copy_ptr<meas_rssi_report_cfg_r16_s> meas_rssi_report_cfg_r16;
  copy_ptr<setup_release_c<dyn_seq_of<bounded_octstring<1, 248>, 1, 4>>> include_bt_meas_r16;
  copy_ptr<setup_release_c<dyn_seq_of<bounded_octstring<1, 32>, 1, 4>>>  include_wlan_meas_r16;
  copy_ptr<setup_release_c<sensor_name_list_r16_s>>                      include_sensor_meas_r16;
  copy_ptr<setup_release_c<ul_delay_value_cfg_r16_s>>                    ul_delay_value_cfg_r16;
  // group 1
  bool                                                 coarse_location_request_r17_present = false;
  copy_ptr<setup_release_c<ul_excess_delay_cfg_r17_s>> ul_excess_delay_cfg_r17;
  copy_ptr<sl_meas_report_quant_r16_c>                 report_quant_relay_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PeriodicalReportConfigInterRAT ::= SEQUENCE
struct periodical_report_cfg_inter_rat_s {
  struct report_amount_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_e_ = enumerated<report_amount_opts>;

  // member variables
  bool                ext = false;
  report_interv_e     report_interv;
  report_amount_e_    report_amount;
  meas_report_quant_s report_quant;
  uint8_t             max_report_cells = 1;
  // ...
  // group 0
  copy_ptr<meas_report_quant_utra_fdd_r16_s> report_quant_utra_fdd_r16;
  // group 1
  bool include_common_location_info_r16_present = false;
  copy_ptr<setup_release_c<dyn_seq_of<bounded_octstring<1, 248>, 1, 4>>> include_bt_meas_r16;
  copy_ptr<setup_release_c<dyn_seq_of<bounded_octstring<1, 32>, 1, 4>>>  include_wlan_meas_r16;
  copy_ptr<setup_release_c<sensor_name_list_r16_s>>                      include_sensor_meas_r16;
  // group 2
  copy_ptr<sl_meas_report_quant_r16_c> report_quant_relay_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PeriodicalReportConfigNR-SL-r16 ::= SEQUENCE
struct periodical_report_cfg_nr_sl_r16_s {
  struct report_amount_r16_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_r16_e_ = enumerated<report_amount_r16_opts>;

  // member variables
  bool                    ext = false;
  report_interv_e         report_interv_r16;
  report_amount_r16_e_    report_amount_r16;
  meas_report_quant_r16_s report_quant_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RAN-AreaConfig ::= SEQUENCE
struct ran_area_cfg_s {
  using ran_area_code_list_l_ = bounded_array<uint16_t, 32>;

  // member variables
  fixed_bitstring<24>   tac;
  ran_area_code_list_l_ ran_area_code_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RMTC-Config-r16 ::= SEQUENCE
struct rmtc_cfg_r16_s {
  struct rmtc_periodicity_r16_opts {
    enum options { ms40, ms80, ms160, ms320, ms640, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using rmtc_periodicity_r16_e_ = enumerated<rmtc_periodicity_r16_opts>;
  struct meas_dur_symbols_r16_opts {
    enum options { sym1, sym14or12, sym28or24, sym42or36, sym70or60, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using meas_dur_symbols_r16_e_ = enumerated<meas_dur_symbols_r16_opts>;
  struct ref_scs_cp_r16_opts {
    enum options { khz15, khz30, khz60_n_cp, khz60_e_cp, nulltype } value;

    const char* to_string() const;
  };
  using ref_scs_cp_r16_e_ = enumerated<ref_scs_cp_r16_opts>;
  struct rmtc_bw_r17_opts {
    enum options { mhz100, mhz400, mhz800, mhz1600, mhz2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using rmtc_bw_r17_e_ = enumerated<rmtc_bw_r17_opts>;
  struct meas_dur_symbols_v1700_opts {
    enum options { sym140, sym560, sym1120, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using meas_dur_symbols_v1700_e_ = enumerated<meas_dur_symbols_v1700_opts>;
  struct ref_scs_cp_v1700_opts {
    enum options { khz120, khz480, khz960, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using ref_scs_cp_v1700_e_ = enumerated<ref_scs_cp_v1700_opts>;
  struct tci_state_info_r17_s_ {
    bool    ref_serv_cell_id_r17_present = false;
    uint8_t tci_state_id_r17             = 0;
    uint8_t ref_serv_cell_id_r17         = 0;
  };

  // member variables
  bool                    ext                        = false;
  bool                    rmtc_sf_offset_r16_present = false;
  rmtc_periodicity_r16_e_ rmtc_periodicity_r16;
  uint16_t                rmtc_sf_offset_r16 = 0;
  meas_dur_symbols_r16_e_ meas_dur_symbols_r16;
  uint32_t                rmtc_freq_r16 = 0;
  ref_scs_cp_r16_e_       ref_scs_cp_r16;
  // ...
  // group 0
  bool                            rmtc_bw_r17_present            = false;
  bool                            meas_dur_symbols_v1700_present = false;
  bool                            ref_scs_cp_v1700_present       = false;
  rmtc_bw_r17_e_                  rmtc_bw_r17;
  meas_dur_symbols_v1700_e_       meas_dur_symbols_v1700;
  ref_scs_cp_v1700_e_             ref_scs_cp_v1700;
  copy_ptr<tci_state_info_r17_s_> tci_state_info_r17;
  // group 1
  bool    ref_bwp_id_r17_present = false;
  uint8_t ref_bwp_id_r17         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfiguration-v1700-IEs ::= SEQUENCE
struct rrc_recfg_v1700_ies_s {
  bool                                                 other_cfg_v1700_present                   = false;
  bool                                                 sl_l2_relay_ue_cfg_r17_present            = false;
  bool                                                 sl_l2_remote_ue_cfg_r17_present           = false;
  bool                                                 need_for_gap_ncsg_cfg_nr_r17_present      = false;
  bool                                                 need_for_gap_ncsg_cfg_eutra_r17_present   = false;
  bool                                                 musim_gap_cfg_r17_present                 = false;
  bool                                                 ul_gap_fr2_cfg_r17_present                = false;
  bool                                                 scg_state_r17_present                     = false;
  bool                                                 app_layer_meas_cfg_r17_present            = false;
  bool                                                 ue_tx_teg_request_ul_tdoa_cfg_r17_present = false;
  bool                                                 non_crit_ext_present                      = false;
  other_cfg_v1700_s                                    other_cfg_v1700;
  setup_release_c<sl_l2_relay_ue_cfg_r17_s>            sl_l2_relay_ue_cfg_r17;
  setup_release_c<sl_l2_remote_ue_cfg_r17_s>           sl_l2_remote_ue_cfg_r17;
  dyn_octstring                                        ded_paging_delivery_r17;
  setup_release_c<need_for_gap_ncsg_cfg_nr_r17_s>      need_for_gap_ncsg_cfg_nr_r17;
  setup_release_c<need_for_gap_ncsg_cfg_eutra_r17_s>   need_for_gap_ncsg_cfg_eutra_r17;
  setup_release_c<mu_si_m_gap_cfg_r17_s>               musim_gap_cfg_r17;
  setup_release_c<ul_gap_fr2_cfg_r17_s>                ul_gap_fr2_cfg_r17;
  app_layer_meas_cfg_r17_s                             app_layer_meas_cfg_r17;
  setup_release_c<ue_tx_teg_request_ul_tdoa_cfg_r17_c> ue_tx_teg_request_ul_tdoa_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReferenceSignalConfig ::= SEQUENCE
struct ref_sig_cfg_s {
  bool                                  ssb_cfg_mob_present        = false;
  bool                                  csi_rs_res_cfg_mob_present = false;
  ssb_cfg_mob_s                         ssb_cfg_mob;
  setup_release_c<csi_rs_res_cfg_mob_s> csi_rs_res_cfg_mob;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportCGI ::= SEQUENCE
struct report_cgi_s {
  bool     ext                          = false;
  uint16_t cell_for_which_to_report_cgi = 0;
  // ...
  // group 0
  bool use_autonomous_gaps_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportCGI-EUTRA ::= SEQUENCE
struct report_cgi_eutra_s {
  bool     ext                          = false;
  uint16_t cell_for_which_to_report_cgi = 0;
  // ...
  // group 0
  bool use_autonomous_gaps_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportSFTD-EUTRA ::= SEQUENCE
struct report_sftd_eutra_s {
  bool ext              = false;
  bool report_sftd_meas = false;
  bool report_rsrp      = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportSFTD-NR ::= SEQUENCE
struct report_sftd_nr_s {
  using cells_for_which_to_report_sftd_l_ = bounded_array<uint16_t, 3>;

  // member variables
  bool ext              = false;
  bool report_sftd_meas = false;
  bool report_rsrp      = false;
  // ...
  // group 0
  bool                                        report_sftd_neigh_meas_present = false;
  bool                                        drx_sftd_neigh_meas_present    = false;
  copy_ptr<cells_for_which_to_report_sftd_l_> cells_for_which_to_report_sftd;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RxTxPeriodical-r17 ::= SEQUENCE
struct rx_tx_periodical_r17_s {
  struct report_amount_r17_opts {
    enum options { r1, infinity, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_r17_e_ = enumerated<report_amount_r17_opts>;

  // member variables
  bool                      ext                             = false;
  bool                      rx_tx_report_interv_r17_present = false;
  rx_tx_report_interv_r17_e rx_tx_report_interv_r17;
  report_amount_r17_e_      report_amount_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-ConfigDedicatedEUTRA-Info-r16 ::= SEQUENCE
struct sl_cfg_ded_eutra_info_r16_s {
  using sl_time_offset_eutra_list_r16_l_ = std::array<sl_time_offset_eutra_r16_e, 8>;

  // member variables
  bool                             sl_time_offset_eutra_list_r16_present = false;
  dyn_octstring                    sl_cfg_ded_eutra_r16;
  sl_time_offset_eutra_list_r16_l_ sl_time_offset_eutra_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-MTC2 ::= SEQUENCE
struct ssb_mtc2_s {
  using pci_list_l_ = dyn_array<uint16_t>;
  struct periodicity_opts {
    enum options { sf5, sf10, sf20, sf40, sf80, spare3, spare2, spare1, nulltype } value;
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

// SSB-MTC3List-r16 ::= SEQUENCE (SIZE (1..4)) OF SSB-MTC3-r16
using ssb_mtc3_list_r16_l = dyn_array<ssb_mtc3_r16_s>;

// SliceInfoDedicated-r17 ::= SEQUENCE
struct slice_info_ded_r17_s {
  bool                  nsag_cell_resel_prio_r17_present     = false;
  bool                  nsag_cell_resel_sub_prio_r17_present = false;
  nsag_id_info_r17_s    nsag_id_info_r17;
  uint8_t               nsag_cell_resel_prio_r17 = 0;
  cell_resel_sub_prio_e nsag_cell_resel_sub_prio_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// T312-r16 ::= ENUMERATED
struct t312_r16_opts {
  enum options { ms0, ms50, ms100, ms200, ms300, ms400, ms500, ms1000, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using t312_r16_e = enumerated<t312_r16_opts>;

// T316-r16 ::= ENUMERATED
struct t316_r16_opts {
  enum options { ms50, ms100, ms200, ms300, ms400, ms500, ms600, ms1000, ms1500, ms2000, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using t316_r16_e = enumerated<t316_r16_opts>;

// TrackingAreaIdentity-r16 ::= SEQUENCE
struct tracking_area_id_r16_s {
  plmn_id_s           plmn_id_r16;
  fixed_bitstring<24> tac_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Tx-PoolMeasList-r16 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..16)
using tx_pool_meas_list_r16_l = bounded_array<uint8_t, 8>;

// UTRA-FDD-CellIndexList-r16 ::= SEQUENCE (SIZE (1..32)) OF INTEGER (1..32)
using utra_fdd_cell_idx_list_r16_l = bounded_array<uint8_t, 32>;

// UTRA-FDD-Q-OffsetRange-r16 ::= ENUMERATED
struct utra_fdd_q_offset_range_r16_opts {
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
using utra_fdd_q_offset_range_r16_e = enumerated<utra_fdd_q_offset_range_r16_opts>;

// ValidityArea-r16 ::= SEQUENCE
struct validity_area_r16_s {
  uint32_t             carrier_freq_r16 = 0;
  validity_cell_list_l validity_cell_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellGlobalIdList-r16 ::= SEQUENCE (SIZE (1..32)) OF CGI-Info-Logging-r16
using cell_global_id_list_r16_l = dyn_array<cgi_info_logging_r16_s>;

// MRDC-SecondaryCellGroupConfig ::= SEQUENCE
struct mrdc_secondary_cell_group_cfg_s {
  struct mrdc_secondary_cell_group_c_ {
    struct types_opts {
      enum options { nr_scg, eutra_scg, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    mrdc_secondary_cell_group_c_() = default;
    mrdc_secondary_cell_group_c_(const mrdc_secondary_cell_group_c_& other);
    mrdc_secondary_cell_group_c_& operator=(const mrdc_secondary_cell_group_c_& other);
    ~mrdc_secondary_cell_group_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dyn_octstring& nr_scg()
    {
      assert_choice_type(types::nr_scg, type_, "mrdc-SecondaryCellGroup");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& eutra_scg()
    {
      assert_choice_type(types::eutra_scg, type_, "mrdc-SecondaryCellGroup");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& nr_scg() const
    {
      assert_choice_type(types::nr_scg, type_, "mrdc-SecondaryCellGroup");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& eutra_scg() const
    {
      assert_choice_type(types::eutra_scg, type_, "mrdc-SecondaryCellGroup");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& set_nr_scg();
    dyn_octstring& set_eutra_scg();

  private:
    types                          type_;
    choice_buffer_t<dyn_octstring> c;

    void destroy_();
  };

  // member variables
  bool                         mrdc_release_and_add_present = false;
  mrdc_secondary_cell_group_c_ mrdc_secondary_cell_group;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasGapSharingScheme ::= ENUMERATED
struct meas_gap_sharing_scheme_opts {
  enum options { scheme00, scheme01, scheme10, scheme11, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using meas_gap_sharing_scheme_e = enumerated<meas_gap_sharing_scheme_opts>;

// MeasObjectCLI-r16 ::= SEQUENCE
struct meas_obj_cli_r16_s {
  bool              ext = false;
  cli_res_cfg_r16_s cli_res_cfg_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectEUTRA ::= SEQUENCE
struct meas_obj_eutra_s {
  using cells_to_add_mod_list_eutran_l_          = dyn_array<eutra_cell_s>;
  using excluded_cells_to_add_mod_list_eutran_l_ = dyn_array<eutra_excluded_cell_s>;

  // member variables
  bool                                     ext                          = false;
  bool                                     eutra_q_offset_range_present = false;
  uint32_t                                 carrier_freq                 = 0;
  eutra_allowed_meas_bw_e                  allowed_meas_bw;
  eutra_cell_idx_list_l                    cells_to_rem_list_eutran;
  cells_to_add_mod_list_eutran_l_          cells_to_add_mod_list_eutran;
  eutra_cell_idx_list_l                    excluded_cells_to_rem_list_eutran;
  excluded_cells_to_add_mod_list_eutran_l_ excluded_cells_to_add_mod_list_eutran;
  bool                                     eutra_presence_ant_port1 = false;
  eutra_q_offset_range_e                   eutra_q_offset_range;
  bool                                     wideband_rsrq_meas = false;
  // ...
  // group 0
  bool    associated_meas_gap_r17_present = false;
  uint8_t associated_meas_gap_r17         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectNR ::= SEQUENCE
struct meas_obj_nr_s {
  using excluded_cells_to_add_mod_list_l_ = dyn_array<pci_range_elem_s>;
  using allowed_cells_to_add_mod_list_l_  = dyn_array<pci_range_elem_s>;
  struct meas_cycle_scell_opts {
    enum options { sf160, sf256, sf320, sf512, sf640, sf1024, sf1280, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using meas_cycle_scell_e_ = enumerated<meas_cycle_scell_opts>;
  struct meas_cycle_pscell_r17_opts {
    enum options { ms160, ms256, ms320, ms512, ms640, ms1024, ms1280, spare1, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using meas_cycle_pscell_r17_e_ = enumerated<meas_cycle_pscell_r17_opts>;

  // member variables
  bool                              ext                                        = false;
  bool                              ssb_freq_present                           = false;
  bool                              ssb_subcarrier_spacing_present             = false;
  bool                              smtc1_present                              = false;
  bool                              smtc2_present                              = false;
  bool                              ref_freq_csi_rs_present                    = false;
  bool                              abs_thresh_ss_blocks_consolidation_present = false;
  bool                              abs_thresh_csi_rs_consolidation_present    = false;
  bool                              nrof_ss_blocks_to_average_present          = false;
  bool                              nrof_csi_rs_res_to_average_present         = false;
  uint32_t                          ssb_freq                                   = 0;
  subcarrier_spacing_e              ssb_subcarrier_spacing;
  ssb_mtc_s                         smtc1;
  ssb_mtc2_s                        smtc2;
  uint32_t                          ref_freq_csi_rs = 0;
  ref_sig_cfg_s                     ref_sig_cfg;
  thres_nr_s                        abs_thresh_ss_blocks_consolidation;
  thres_nr_s                        abs_thresh_csi_rs_consolidation;
  uint8_t                           nrof_ss_blocks_to_average  = 2;
  uint8_t                           nrof_csi_rs_res_to_average = 2;
  uint8_t                           quant_cfg_idx              = 1;
  q_offset_range_list_s             offset_mo;
  pci_list_l                        cells_to_rem_list;
  cells_to_add_mod_list_l           cells_to_add_mod_list;
  pci_range_idx_list_l              excluded_cells_to_rem_list;
  excluded_cells_to_add_mod_list_l_ excluded_cells_to_add_mod_list;
  pci_range_idx_list_l              allowed_cells_to_rem_list;
  allowed_cells_to_add_mod_list_l_  allowed_cells_to_add_mod_list;
  // ...
  // group 0
  bool                freq_band_ind_nr_present = false;
  bool                meas_cycle_scell_present = false;
  uint16_t            freq_band_ind_nr         = 1;
  meas_cycle_scell_e_ meas_cycle_scell;
  // group 1
  copy_ptr<ssb_mtc3_list_r16_l>             smtc3list_r16;
  copy_ptr<setup_release_c<rmtc_cfg_r16_s>> rmtc_cfg_r16;
  copy_ptr<setup_release_c<t312_r16_e>>     t312_r16;
  // group 2
  bool                                        associated_meas_gap_ssb_r17_present    = false;
  bool                                        associated_meas_gap_csi_rs_r17_present = false;
  bool                                        meas_cycle_pscell_r17_present          = false;
  uint8_t                                     associated_meas_gap_ssb_r17            = 1;
  uint8_t                                     associated_meas_gap_csi_rs_r17         = 1;
  copy_ptr<ssb_mtc4_list_r17_l>               smtc4list_r17;
  meas_cycle_pscell_r17_e_                    meas_cycle_pscell_r17;
  copy_ptr<cells_to_add_mod_list_ext_v1710_l> cells_to_add_mod_list_ext_v1710;
  // group 3
  bool    associated_meas_gap_ssb2_v1720_present    = false;
  bool    associated_meas_gap_csi_rs2_v1720_present = false;
  uint8_t associated_meas_gap_ssb2_v1720            = 1;
  uint8_t associated_meas_gap_csi_rs2_v1720         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectNR-SL-r16 ::= SEQUENCE
struct meas_obj_nr_sl_r16_s {
  tx_pool_meas_list_r16_l tx_pool_meas_to_rem_list_r16;
  tx_pool_meas_list_r16_l tx_pool_meas_to_add_mod_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectRxTxDiff-r17 ::= SEQUENCE
struct meas_obj_rx_tx_diff_r17_s {
  struct dl_ref_r17_c_ {
    struct types_opts {
      enum options { prs_ref_r17, csi_rs_ref_r17, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    dl_ref_r17_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    void set_prs_ref_r17();
    void set_csi_rs_ref_r17();

  private:
    types type_;
  };

  // member variables
  bool          ext                = false;
  bool          dl_ref_r17_present = false;
  dl_ref_r17_c_ dl_ref_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectUTRA-FDD-r16 ::= SEQUENCE
struct meas_obj_utra_fdd_r16_s {
  bool                                 ext                                 = false;
  bool                                 utra_fdd_q_offset_range_r16_present = false;
  uint16_t                             carrier_freq_r16                    = 0;
  utra_fdd_q_offset_range_r16_e        utra_fdd_q_offset_range_r16;
  utra_fdd_cell_idx_list_r16_l         cells_to_rem_list_r16;
  cells_to_add_mod_list_utra_fdd_r16_l cells_to_add_mod_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OverheatingAssistanceConfig ::= SEQUENCE
struct overheat_assist_cfg_s {
  struct overheat_ind_prohibit_timer_opts {
    enum options {
      s0,
      s0dot5,
      s1,
      s2,
      s5,
      s10,
      s20,
      s30,
      s60,
      s90,
      s120,
      s300,
      s600,
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
  using overheat_ind_prohibit_timer_e_ = enumerated<overheat_ind_prohibit_timer_opts>;

  // member variables
  overheat_ind_prohibit_timer_e_ overheat_ind_prohibit_timer;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-RAN-AreaCell ::= SEQUENCE
struct plmn_ran_area_cell_s {
  using ran_area_cells_l_ = bounded_array<fixed_bitstring<36>, 32>;

  // member variables
  bool              plmn_id_present = false;
  plmn_id_s         plmn_id;
  ran_area_cells_l_ ran_area_cells;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-RAN-AreaConfig ::= SEQUENCE
struct plmn_ran_area_cfg_s {
  using ran_area_l_ = dyn_array<ran_area_cfg_s>;

  // member variables
  bool        plmn_id_present = false;
  plmn_id_s   plmn_id;
  ran_area_l_ ran_area;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PosGapConfig-r17 ::= SEQUENCE
struct pos_gap_cfg_r17_s {
  struct mgl_r17_opts {
    enum options { ms1dot5, ms3, ms3dot5, ms4, ms5dot5, ms6, ms10, ms20, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using mgl_r17_e_ = enumerated<mgl_r17_opts>;
  struct mgrp_r17_opts {
    enum options { ms20, ms40, ms80, ms160, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mgrp_r17_e_ = enumerated<mgrp_r17_opts>;
  struct mgta_r17_opts {
    enum options { ms0, ms0dot25, ms0dot5, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using mgta_r17_e_ = enumerated<mgta_r17_opts>;
  struct gap_type_r17_opts {
    enum options { per_ue, per_fr1, per_fr2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using gap_type_r17_e_ = enumerated<gap_type_r17_opts>;

  // member variables
  bool            ext                         = false;
  uint8_t         meas_pos_pre_cfg_gap_id_r17 = 1;
  uint8_t         gap_offset_r17              = 0;
  mgl_r17_e_      mgl_r17;
  mgrp_r17_e_     mgrp_r17;
  mgta_r17_e_     mgta_r17;
  gap_type_r17_e_ gap_type_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QuantityConfigRS ::= SEQUENCE
struct quant_cfg_rs_s {
  filt_cfg_s ssb_filt_cfg;
  filt_cfg_s csi_rs_filt_cfg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfiguration-v1610-IEs ::= SEQUENCE
struct rrc_recfg_v1610_ies_s {
  bool                                         other_cfg_v1610_present             = false;
  bool                                         bap_cfg_r16_present                 = false;
  bool                                         iab_ip_address_cfg_list_r16_present = false;
  bool                                         conditional_recfg_r16_present       = false;
  bool                                         daps_source_release_r16_present     = false;
  bool                                         t316_r16_present                    = false;
  bool                                         need_for_gaps_cfg_nr_r16_present    = false;
  bool                                         on_demand_sib_request_r16_present   = false;
  bool                                         sl_cfg_ded_nr_r16_present           = false;
  bool                                         sl_cfg_ded_eutra_info_r16_present   = false;
  bool                                         target_cell_smtc_scg_r16_present    = false;
  bool                                         non_crit_ext_present                = false;
  other_cfg_v1610_s                            other_cfg_v1610;
  setup_release_c<bap_cfg_r16_s>               bap_cfg_r16;
  iab_ip_address_cfg_list_r16_s                iab_ip_address_cfg_list_r16;
  conditional_recfg_r16_s                      conditional_recfg_r16;
  setup_release_c<t316_r16_e>                  t316_r16;
  setup_release_c<need_for_gaps_cfg_nr_r16_s>  need_for_gaps_cfg_nr_r16;
  setup_release_c<on_demand_sib_request_r16_s> on_demand_sib_request_r16;
  dyn_octstring                                ded_pos_sys_info_delivery_r16;
  setup_release_c<sl_cfg_ded_nr_r16_s>         sl_cfg_ded_nr_r16;
  setup_release_c<sl_cfg_ded_eutra_info_r16_s> sl_cfg_ded_eutra_info_r16;
  ssb_mtc_s                                    target_cell_smtc_scg_r16;
  rrc_recfg_v1700_ies_s                        non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCRelease-v1710-IEs ::= SEQUENCE
struct rrc_release_v1710_ies_s {
  bool no_last_cell_upd_r17_present = false;
  bool non_crit_ext_present         = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportConfigInterRAT ::= SEQUENCE
struct report_cfg_inter_rat_s {
  struct report_type_c_ {
    struct types_opts {
      enum options { periodical, event_triggered, report_cgi, /*...*/ report_sftd, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true, 1>;

    // choice methods
    report_type_c_() = default;
    report_type_c_(const report_type_c_& other);
    report_type_c_& operator=(const report_type_c_& other);
    ~report_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    periodical_report_cfg_inter_rat_s& periodical()
    {
      assert_choice_type(types::periodical, type_, "reportType");
      return c.get<periodical_report_cfg_inter_rat_s>();
    }
    event_trigger_cfg_inter_rat_s& event_triggered()
    {
      assert_choice_type(types::event_triggered, type_, "reportType");
      return c.get<event_trigger_cfg_inter_rat_s>();
    }
    report_cgi_eutra_s& report_cgi()
    {
      assert_choice_type(types::report_cgi, type_, "reportType");
      return c.get<report_cgi_eutra_s>();
    }
    report_sftd_eutra_s& report_sftd()
    {
      assert_choice_type(types::report_sftd, type_, "reportType");
      return c.get<report_sftd_eutra_s>();
    }
    const periodical_report_cfg_inter_rat_s& periodical() const
    {
      assert_choice_type(types::periodical, type_, "reportType");
      return c.get<periodical_report_cfg_inter_rat_s>();
    }
    const event_trigger_cfg_inter_rat_s& event_triggered() const
    {
      assert_choice_type(types::event_triggered, type_, "reportType");
      return c.get<event_trigger_cfg_inter_rat_s>();
    }
    const report_cgi_eutra_s& report_cgi() const
    {
      assert_choice_type(types::report_cgi, type_, "reportType");
      return c.get<report_cgi_eutra_s>();
    }
    const report_sftd_eutra_s& report_sftd() const
    {
      assert_choice_type(types::report_sftd, type_, "reportType");
      return c.get<report_sftd_eutra_s>();
    }
    periodical_report_cfg_inter_rat_s& set_periodical();
    event_trigger_cfg_inter_rat_s&     set_event_triggered();
    report_cgi_eutra_s&                set_report_cgi();
    report_sftd_eutra_s&               set_report_sftd();

  private:
    types type_;
    choice_buffer_t<event_trigger_cfg_inter_rat_s,
                    periodical_report_cfg_inter_rat_s,
                    report_cgi_eutra_s,
                    report_sftd_eutra_s>
        c;

    void destroy_();
  };

  // member variables
  report_type_c_ report_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportConfigNR ::= SEQUENCE
struct report_cfg_nr_s {
  struct report_type_c_ {
    struct types_opts {
      enum options {
        periodical,
        event_triggered,
        // ...
        report_cgi,
        report_sftd,
        cond_trigger_cfg_r16,
        cli_periodical_r16,
        cli_event_triggered_r16,
        rx_tx_periodical_r17,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true, 6>;

    // choice methods
    report_type_c_() = default;
    report_type_c_(const report_type_c_& other);
    report_type_c_& operator=(const report_type_c_& other);
    ~report_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    periodical_report_cfg_s& periodical()
    {
      assert_choice_type(types::periodical, type_, "reportType");
      return c.get<periodical_report_cfg_s>();
    }
    event_trigger_cfg_s& event_triggered()
    {
      assert_choice_type(types::event_triggered, type_, "reportType");
      return c.get<event_trigger_cfg_s>();
    }
    report_cgi_s& report_cgi()
    {
      assert_choice_type(types::report_cgi, type_, "reportType");
      return c.get<report_cgi_s>();
    }
    report_sftd_nr_s& report_sftd()
    {
      assert_choice_type(types::report_sftd, type_, "reportType");
      return c.get<report_sftd_nr_s>();
    }
    cond_trigger_cfg_r16_s& cond_trigger_cfg_r16()
    {
      assert_choice_type(types::cond_trigger_cfg_r16, type_, "reportType");
      return c.get<cond_trigger_cfg_r16_s>();
    }
    cli_periodical_report_cfg_r16_s& cli_periodical_r16()
    {
      assert_choice_type(types::cli_periodical_r16, type_, "reportType");
      return c.get<cli_periodical_report_cfg_r16_s>();
    }
    cli_event_trigger_cfg_r16_s& cli_event_triggered_r16()
    {
      assert_choice_type(types::cli_event_triggered_r16, type_, "reportType");
      return c.get<cli_event_trigger_cfg_r16_s>();
    }
    rx_tx_periodical_r17_s& rx_tx_periodical_r17()
    {
      assert_choice_type(types::rx_tx_periodical_r17, type_, "reportType");
      return c.get<rx_tx_periodical_r17_s>();
    }
    const periodical_report_cfg_s& periodical() const
    {
      assert_choice_type(types::periodical, type_, "reportType");
      return c.get<periodical_report_cfg_s>();
    }
    const event_trigger_cfg_s& event_triggered() const
    {
      assert_choice_type(types::event_triggered, type_, "reportType");
      return c.get<event_trigger_cfg_s>();
    }
    const report_cgi_s& report_cgi() const
    {
      assert_choice_type(types::report_cgi, type_, "reportType");
      return c.get<report_cgi_s>();
    }
    const report_sftd_nr_s& report_sftd() const
    {
      assert_choice_type(types::report_sftd, type_, "reportType");
      return c.get<report_sftd_nr_s>();
    }
    const cond_trigger_cfg_r16_s& cond_trigger_cfg_r16() const
    {
      assert_choice_type(types::cond_trigger_cfg_r16, type_, "reportType");
      return c.get<cond_trigger_cfg_r16_s>();
    }
    const cli_periodical_report_cfg_r16_s& cli_periodical_r16() const
    {
      assert_choice_type(types::cli_periodical_r16, type_, "reportType");
      return c.get<cli_periodical_report_cfg_r16_s>();
    }
    const cli_event_trigger_cfg_r16_s& cli_event_triggered_r16() const
    {
      assert_choice_type(types::cli_event_triggered_r16, type_, "reportType");
      return c.get<cli_event_trigger_cfg_r16_s>();
    }
    const rx_tx_periodical_r17_s& rx_tx_periodical_r17() const
    {
      assert_choice_type(types::rx_tx_periodical_r17, type_, "reportType");
      return c.get<rx_tx_periodical_r17_s>();
    }
    periodical_report_cfg_s&         set_periodical();
    event_trigger_cfg_s&             set_event_triggered();
    report_cgi_s&                    set_report_cgi();
    report_sftd_nr_s&                set_report_sftd();
    cond_trigger_cfg_r16_s&          set_cond_trigger_cfg_r16();
    cli_periodical_report_cfg_r16_s& set_cli_periodical_r16();
    cli_event_trigger_cfg_r16_s&     set_cli_event_triggered_r16();
    rx_tx_periodical_r17_s&          set_rx_tx_periodical_r17();

  private:
    types type_;
    choice_buffer_t<cli_event_trigger_cfg_r16_s,
                    cli_periodical_report_cfg_r16_s,
                    cond_trigger_cfg_r16_s,
                    event_trigger_cfg_s,
                    periodical_report_cfg_s,
                    report_cgi_s,
                    report_sftd_nr_s,
                    rx_tx_periodical_r17_s>
        c;

    void destroy_();
  };

  // member variables
  report_type_c_ report_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportConfigNR-SL-r16 ::= SEQUENCE
struct report_cfg_nr_sl_r16_s {
  struct report_type_r16_c_ {
    struct types_opts {
      enum options { periodical_r16, event_triggered_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    report_type_r16_c_() = default;
    report_type_r16_c_(const report_type_r16_c_& other);
    report_type_r16_c_& operator=(const report_type_r16_c_& other);
    ~report_type_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    periodical_report_cfg_nr_sl_r16_s& periodical_r16()
    {
      assert_choice_type(types::periodical_r16, type_, "reportType-r16");
      return c.get<periodical_report_cfg_nr_sl_r16_s>();
    }
    event_trigger_cfg_nr_sl_r16_s& event_triggered_r16()
    {
      assert_choice_type(types::event_triggered_r16, type_, "reportType-r16");
      return c.get<event_trigger_cfg_nr_sl_r16_s>();
    }
    const periodical_report_cfg_nr_sl_r16_s& periodical_r16() const
    {
      assert_choice_type(types::periodical_r16, type_, "reportType-r16");
      return c.get<periodical_report_cfg_nr_sl_r16_s>();
    }
    const event_trigger_cfg_nr_sl_r16_s& event_triggered_r16() const
    {
      assert_choice_type(types::event_triggered_r16, type_, "reportType-r16");
      return c.get<event_trigger_cfg_nr_sl_r16_s>();
    }
    periodical_report_cfg_nr_sl_r16_s& set_periodical_r16();
    event_trigger_cfg_nr_sl_r16_s&     set_event_triggered_r16();

  private:
    types                                                                             type_;
    choice_buffer_t<event_trigger_cfg_nr_sl_r16_s, periodical_report_cfg_nr_sl_r16_s> c;

    void destroy_();
  };

  // member variables
  report_type_r16_c_ report_type_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SliceInfoListDedicated-r17 ::= SEQUENCE (SIZE (1..8)) OF SliceInfoDedicated-r17
using slice_info_list_ded_r17_l = dyn_array<slice_info_ded_r17_s>;

// TrackingAreaCodeList-r16 ::= SEQUENCE (SIZE (1..8)) OF BIT STRING (SIZE (24))
using tac_list_r16_l = bounded_array<fixed_bitstring<24>, 8>;

// TrackingAreaIdentityList-r16 ::= SEQUENCE (SIZE (1..8)) OF TrackingAreaIdentity-r16
using tracking_area_id_list_r16_l = dyn_array<tracking_area_id_r16_s>;

// ValidityAreaList-r16 ::= SEQUENCE (SIZE (1..8)) OF ValidityArea-r16
using validity_area_list_r16_l = dyn_array<validity_area_r16_s>;

// AreaConfig-r16 ::= CHOICE
struct area_cfg_r16_c {
  struct types_opts {
    enum options { cell_global_id_list_r16, tac_list_r16, tracking_area_id_list_r16, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  area_cfg_r16_c() = default;
  area_cfg_r16_c(const area_cfg_r16_c& other);
  area_cfg_r16_c& operator=(const area_cfg_r16_c& other);
  ~area_cfg_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cell_global_id_list_r16_l& cell_global_id_list_r16()
  {
    assert_choice_type(types::cell_global_id_list_r16, type_, "AreaConfig-r16");
    return c.get<cell_global_id_list_r16_l>();
  }
  tac_list_r16_l& tac_list_r16()
  {
    assert_choice_type(types::tac_list_r16, type_, "AreaConfig-r16");
    return c.get<tac_list_r16_l>();
  }
  tracking_area_id_list_r16_l& tracking_area_id_list_r16()
  {
    assert_choice_type(types::tracking_area_id_list_r16, type_, "AreaConfig-r16");
    return c.get<tracking_area_id_list_r16_l>();
  }
  const cell_global_id_list_r16_l& cell_global_id_list_r16() const
  {
    assert_choice_type(types::cell_global_id_list_r16, type_, "AreaConfig-r16");
    return c.get<cell_global_id_list_r16_l>();
  }
  const tac_list_r16_l& tac_list_r16() const
  {
    assert_choice_type(types::tac_list_r16, type_, "AreaConfig-r16");
    return c.get<tac_list_r16_l>();
  }
  const tracking_area_id_list_r16_l& tracking_area_id_list_r16() const
  {
    assert_choice_type(types::tracking_area_id_list_r16, type_, "AreaConfig-r16");
    return c.get<tracking_area_id_list_r16_l>();
  }
  cell_global_id_list_r16_l&   set_cell_global_id_list_r16();
  tac_list_r16_l&              set_tac_list_r16();
  tracking_area_id_list_r16_l& set_tracking_area_id_list_r16();

private:
  types                                                                                   type_;
  choice_buffer_t<cell_global_id_list_r16_l, tac_list_r16_l, tracking_area_id_list_r16_l> c;

  void destroy_();
};

// FilterConfigCLI-r16 ::= SEQUENCE
struct filt_cfg_cli_r16_s {
  bool        filt_coef_srs_rsrp_r16_present = false;
  bool        filt_coef_cli_rssi_r16_present = false;
  filt_coef_e filt_coef_srs_rsrp_r16;
  filt_coef_e filt_coef_cli_rssi_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqPriorityDedicatedSlicing-r17 ::= SEQUENCE
struct freq_prio_ded_slicing_r17_s {
  uint32_t                  dl_explicit_carrier_freq_r17 = 0;
  slice_info_list_ded_r17_l slice_info_list_ded_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqPriorityEUTRA ::= SEQUENCE
struct freq_prio_eutra_s {
  bool                  cell_resel_sub_prio_present = false;
  uint32_t              carrier_freq                = 0;
  uint8_t               cell_resel_prio             = 0;
  cell_resel_sub_prio_e cell_resel_sub_prio;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqPriorityNR ::= SEQUENCE
struct freq_prio_nr_s {
  bool                  cell_resel_sub_prio_present = false;
  uint32_t              carrier_freq                = 0;
  uint8_t               cell_resel_prio             = 0;
  cell_resel_sub_prio_e cell_resel_sub_prio;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GapConfig ::= SEQUENCE
struct gap_cfg_s {
  struct mgl_opts {
    enum options { ms1dot5, ms3, ms3dot5, ms4, ms5dot5, ms6, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using mgl_e_ = enumerated<mgl_opts>;
  struct mgrp_opts {
    enum options { ms20, ms40, ms80, ms160, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mgrp_e_ = enumerated<mgrp_opts>;
  struct mgta_opts {
    enum options { ms0, ms0dot25, ms0dot5, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using mgta_e_ = enumerated<mgta_opts>;
  struct ref_serv_cell_ind_opts {
    enum options { pcell, pscell, mcg_fr2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ref_serv_cell_ind_e_ = enumerated<ref_serv_cell_ind_opts>;
  struct mgl_r16_opts {
    enum options { ms10, ms20, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mgl_r16_e_ = enumerated<mgl_r16_opts>;

  // member variables
  bool    ext        = false;
  uint8_t gap_offset = 0;
  mgl_e_  mgl;
  mgrp_e_ mgrp;
  mgta_e_ mgta;
  // ...
  // group 0
  bool                 ref_serv_cell_ind_present = false;
  ref_serv_cell_ind_e_ ref_serv_cell_ind;
  // group 1
  bool       ref_fr2_serv_cell_async_ca_r16_present = false;
  bool       mgl_r16_present                        = false;
  uint8_t    ref_fr2_serv_cell_async_ca_r16         = 0;
  mgl_r16_e_ mgl_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GapConfig-r17 ::= SEQUENCE
struct gap_cfg_r17_s {
  struct gap_type_r17_opts {
    enum options { per_ue, per_fr1, per_fr2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using gap_type_r17_e_ = enumerated<gap_type_r17_opts>;
  struct mgl_r17_opts {
    enum options { ms1, ms1dot5, ms2, ms3, ms3dot5, ms4, ms5, ms5dot5, ms6, ms10, ms20, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using mgl_r17_e_ = enumerated<mgl_r17_opts>;
  struct mgrp_r17_opts {
    enum options { ms20, ms40, ms80, ms160, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mgrp_r17_e_ = enumerated<mgrp_r17_opts>;
  struct mgta_r17_opts {
    enum options { ms0, ms0dot25, ms0dot5, ms0dot75, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using mgta_r17_e_ = enumerated<mgta_r17_opts>;
  struct ref_serv_cell_ind_r17_opts {
    enum options { pcell, pscell, mcg_fr2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ref_serv_cell_ind_r17_e_ = enumerated<ref_serv_cell_ind_r17_opts>;

  // member variables
  bool                      ext                                    = false;
  bool                      ref_serv_cell_ind_r17_present          = false;
  bool                      ref_fr2_serv_cell_async_ca_r17_present = false;
  bool                      pre_cfg_ind_r17_present                = false;
  bool                      ncsg_ind_r17_present                   = false;
  bool                      gap_assoc_prs_r17_present              = false;
  bool                      gap_sharing_r17_present                = false;
  bool                      gap_prio_r17_present                   = false;
  uint8_t                   meas_gap_id_r17                        = 1;
  gap_type_r17_e_           gap_type_r17;
  uint8_t                   gap_offset_r17 = 0;
  mgl_r17_e_                mgl_r17;
  mgrp_r17_e_               mgrp_r17;
  mgta_r17_e_               mgta_r17;
  ref_serv_cell_ind_r17_e_  ref_serv_cell_ind_r17;
  uint8_t                   ref_fr2_serv_cell_async_ca_r17 = 0;
  meas_gap_sharing_scheme_e gap_sharing_r17;
  uint8_t                   gap_prio_r17 = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqTargetInfo-r16 ::= SEQUENCE
struct inter_freq_target_info_r16_s {
  using cell_list_r16_l_ = bounded_array<uint16_t, 32>;

  // member variables
  uint32_t         dl_carrier_freq_r16 = 0;
  cell_list_r16_l_ cell_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasIdToAddMod ::= SEQUENCE
struct meas_id_to_add_mod_s {
  uint8_t meas_id       = 1;
  uint8_t meas_obj_id   = 1;
  uint8_t report_cfg_id = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasIdleConfigDedicated-r16 ::= SEQUENCE
struct meas_idle_cfg_ded_r16_s {
  using meas_idle_carrier_list_nr_r16_l_    = dyn_array<meas_idle_carrier_nr_r16_s>;
  using meas_idle_carrier_list_eutra_r16_l_ = dyn_array<meas_idle_carrier_eutra_r16_s>;
  struct meas_idle_dur_r16_opts {
    enum options { sec10, sec30, sec60, sec120, sec180, sec240, sec300, spare, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using meas_idle_dur_r16_e_ = enumerated<meas_idle_dur_r16_opts>;

  // member variables
  bool                                ext = false;
  meas_idle_carrier_list_nr_r16_l_    meas_idle_carrier_list_nr_r16;
  meas_idle_carrier_list_eutra_r16_l_ meas_idle_carrier_list_eutra_r16;
  meas_idle_dur_r16_e_                meas_idle_dur_r16;
  validity_area_list_r16_l            validity_area_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectToAddMod ::= SEQUENCE
struct meas_obj_to_add_mod_s {
  struct meas_obj_c_ {
    struct types_opts {
      enum options {
        meas_obj_nr,
        // ...
        meas_obj_eutra,
        meas_obj_utra_fdd_r16,
        meas_obj_nr_sl_r16,
        meas_obj_cli_r16,
        meas_obj_rx_tx_diff_r17,
        meas_obj_relay_r17,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true, 6>;

    // choice methods
    meas_obj_c_() = default;
    meas_obj_c_(const meas_obj_c_& other);
    meas_obj_c_& operator=(const meas_obj_c_& other);
    ~meas_obj_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    meas_obj_nr_s& meas_obj_nr()
    {
      assert_choice_type(types::meas_obj_nr, type_, "measObject");
      return c.get<meas_obj_nr_s>();
    }
    meas_obj_eutra_s& meas_obj_eutra()
    {
      assert_choice_type(types::meas_obj_eutra, type_, "measObject");
      return c.get<meas_obj_eutra_s>();
    }
    meas_obj_utra_fdd_r16_s& meas_obj_utra_fdd_r16()
    {
      assert_choice_type(types::meas_obj_utra_fdd_r16, type_, "measObject");
      return c.get<meas_obj_utra_fdd_r16_s>();
    }
    meas_obj_nr_sl_r16_s& meas_obj_nr_sl_r16()
    {
      assert_choice_type(types::meas_obj_nr_sl_r16, type_, "measObject");
      return c.get<meas_obj_nr_sl_r16_s>();
    }
    meas_obj_cli_r16_s& meas_obj_cli_r16()
    {
      assert_choice_type(types::meas_obj_cli_r16, type_, "measObject");
      return c.get<meas_obj_cli_r16_s>();
    }
    meas_obj_rx_tx_diff_r17_s& meas_obj_rx_tx_diff_r17()
    {
      assert_choice_type(types::meas_obj_rx_tx_diff_r17, type_, "measObject");
      return c.get<meas_obj_rx_tx_diff_r17_s>();
    }
    sl_meas_obj_r16_s& meas_obj_relay_r17()
    {
      assert_choice_type(types::meas_obj_relay_r17, type_, "measObject");
      return c.get<sl_meas_obj_r16_s>();
    }
    const meas_obj_nr_s& meas_obj_nr() const
    {
      assert_choice_type(types::meas_obj_nr, type_, "measObject");
      return c.get<meas_obj_nr_s>();
    }
    const meas_obj_eutra_s& meas_obj_eutra() const
    {
      assert_choice_type(types::meas_obj_eutra, type_, "measObject");
      return c.get<meas_obj_eutra_s>();
    }
    const meas_obj_utra_fdd_r16_s& meas_obj_utra_fdd_r16() const
    {
      assert_choice_type(types::meas_obj_utra_fdd_r16, type_, "measObject");
      return c.get<meas_obj_utra_fdd_r16_s>();
    }
    const meas_obj_nr_sl_r16_s& meas_obj_nr_sl_r16() const
    {
      assert_choice_type(types::meas_obj_nr_sl_r16, type_, "measObject");
      return c.get<meas_obj_nr_sl_r16_s>();
    }
    const meas_obj_cli_r16_s& meas_obj_cli_r16() const
    {
      assert_choice_type(types::meas_obj_cli_r16, type_, "measObject");
      return c.get<meas_obj_cli_r16_s>();
    }
    const meas_obj_rx_tx_diff_r17_s& meas_obj_rx_tx_diff_r17() const
    {
      assert_choice_type(types::meas_obj_rx_tx_diff_r17, type_, "measObject");
      return c.get<meas_obj_rx_tx_diff_r17_s>();
    }
    const sl_meas_obj_r16_s& meas_obj_relay_r17() const
    {
      assert_choice_type(types::meas_obj_relay_r17, type_, "measObject");
      return c.get<sl_meas_obj_r16_s>();
    }
    meas_obj_nr_s&             set_meas_obj_nr();
    meas_obj_eutra_s&          set_meas_obj_eutra();
    meas_obj_utra_fdd_r16_s&   set_meas_obj_utra_fdd_r16();
    meas_obj_nr_sl_r16_s&      set_meas_obj_nr_sl_r16();
    meas_obj_cli_r16_s&        set_meas_obj_cli_r16();
    meas_obj_rx_tx_diff_r17_s& set_meas_obj_rx_tx_diff_r17();
    sl_meas_obj_r16_s&         set_meas_obj_relay_r17();

  private:
    types type_;
    choice_buffer_t<meas_obj_cli_r16_s,
                    meas_obj_eutra_s,
                    meas_obj_nr_s,
                    meas_obj_nr_sl_r16_s,
                    meas_obj_rx_tx_diff_r17_s,
                    meas_obj_utra_fdd_r16_s,
                    sl_meas_obj_r16_s>
        c;

    void destroy_();
  };

  // member variables
  uint8_t     meas_obj_id = 1;
  meas_obj_c_ meas_obj;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OtherConfig-v1540 ::= SEQUENCE
struct other_cfg_v1540_s {
  bool                                   ext                         = false;
  bool                                   overheat_assist_cfg_present = false;
  setup_release_c<overheat_assist_cfg_s> overheat_assist_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-RAN-AreaCellList ::= SEQUENCE (SIZE (1..8)) OF PLMN-RAN-AreaCell
using plmn_ran_area_cell_list_l = dyn_array<plmn_ran_area_cell_s>;

// PLMN-RAN-AreaConfigList ::= SEQUENCE (SIZE (1..8)) OF PLMN-RAN-AreaConfig
using plmn_ran_area_cfg_list_l = dyn_array<plmn_ran_area_cfg_s>;

// PosMeasGapPreConfigToAddModList-r17 ::= SEQUENCE (SIZE (1..16)) OF PosGapConfig-r17
using pos_meas_gap_pre_cfg_to_add_mod_list_r17_l = dyn_array<pos_gap_cfg_r17_s>;

// PosMeasGapPreConfigToReleaseList-r17 ::= SEQUENCE (SIZE (1..16)) OF INTEGER (1..16)
using pos_meas_gap_pre_cfg_to_release_list_r17_l = bounded_array<uint8_t, 16>;

// QuantityConfigNR ::= SEQUENCE
struct quant_cfg_nr_s {
  bool           quant_cfg_rs_idx_present = false;
  quant_cfg_rs_s quant_cfg_cell;
  quant_cfg_rs_s quant_cfg_rs_idx;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QuantityConfigUTRA-FDD-r16 ::= SEQUENCE
struct quant_cfg_utra_fdd_r16_s {
  bool        filt_coef_rs_cp_r16_present = false;
  bool        filt_coef_ec_no_r16_present = false;
  filt_coef_e filt_coef_rs_cp_r16;
  filt_coef_e filt_coef_ec_no_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfiguration-v1560-IEs ::= SEQUENCE
struct rrc_recfg_v1560_ies_s {
  bool                                             mrdc_secondary_cell_group_cfg_present = false;
  bool                                             sk_counter_present                    = false;
  bool                                             non_crit_ext_present                  = false;
  setup_release_c<mrdc_secondary_cell_group_cfg_s> mrdc_secondary_cell_group_cfg;
  dyn_octstring                                    radio_bearer_cfg2;
  uint32_t                                         sk_counter = 0;
  rrc_recfg_v1610_ies_s                            non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCRelease-v1650-IEs ::= SEQUENCE
struct rrc_release_v1650_ies_s {
  bool                    mps_prio_ind_r16_present = false;
  bool                    non_crit_ext_present     = false;
  rrc_release_v1710_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResume-v1700-IEs ::= SEQUENCE
struct rrc_resume_v1700_ies_s {
  bool                                               sl_cfg_ded_nr_r17_present               = false;
  bool                                               sl_l2_remote_ue_cfg_r17_present         = false;
  bool                                               need_for_gap_ncsg_cfg_nr_r17_present    = false;
  bool                                               need_for_gap_ncsg_cfg_eutra_r17_present = false;
  bool                                               scg_state_r17_present                   = false;
  bool                                               app_layer_meas_cfg_r17_present          = false;
  bool                                               non_crit_ext_present                    = false;
  setup_release_c<sl_cfg_ded_nr_r16_s>               sl_cfg_ded_nr_r17;
  setup_release_c<sl_l2_remote_ue_cfg_r17_s>         sl_l2_remote_ue_cfg_r17;
  setup_release_c<need_for_gap_ncsg_cfg_nr_r17_s>    need_for_gap_ncsg_cfg_nr_r17;
  setup_release_c<need_for_gap_ncsg_cfg_eutra_r17_s> need_for_gap_ncsg_cfg_eutra_r17;
  app_layer_meas_cfg_r17_s                           app_layer_meas_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportConfigToAddMod ::= SEQUENCE
struct report_cfg_to_add_mod_s {
  struct report_cfg_c_ {
    struct types_opts {
      enum options { report_cfg_nr, /*...*/ report_cfg_inter_rat, report_cfg_nr_sl_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true, 2>;

    // choice methods
    report_cfg_c_() = default;
    report_cfg_c_(const report_cfg_c_& other);
    report_cfg_c_& operator=(const report_cfg_c_& other);
    ~report_cfg_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    report_cfg_nr_s& report_cfg_nr()
    {
      assert_choice_type(types::report_cfg_nr, type_, "reportConfig");
      return c.get<report_cfg_nr_s>();
    }
    report_cfg_inter_rat_s& report_cfg_inter_rat()
    {
      assert_choice_type(types::report_cfg_inter_rat, type_, "reportConfig");
      return c.get<report_cfg_inter_rat_s>();
    }
    report_cfg_nr_sl_r16_s& report_cfg_nr_sl_r16()
    {
      assert_choice_type(types::report_cfg_nr_sl_r16, type_, "reportConfig");
      return c.get<report_cfg_nr_sl_r16_s>();
    }
    const report_cfg_nr_s& report_cfg_nr() const
    {
      assert_choice_type(types::report_cfg_nr, type_, "reportConfig");
      return c.get<report_cfg_nr_s>();
    }
    const report_cfg_inter_rat_s& report_cfg_inter_rat() const
    {
      assert_choice_type(types::report_cfg_inter_rat, type_, "reportConfig");
      return c.get<report_cfg_inter_rat_s>();
    }
    const report_cfg_nr_sl_r16_s& report_cfg_nr_sl_r16() const
    {
      assert_choice_type(types::report_cfg_nr_sl_r16, type_, "reportConfig");
      return c.get<report_cfg_nr_sl_r16_s>();
    }
    report_cfg_nr_s&        set_report_cfg_nr();
    report_cfg_inter_rat_s& set_report_cfg_inter_rat();
    report_cfg_nr_sl_r16_s& set_report_cfg_nr_sl_r16();

  private:
    types                                                                            type_;
    choice_buffer_t<report_cfg_inter_rat_s, report_cfg_nr_s, report_cfg_nr_sl_r16_s> c;

    void destroy_();
  };

  // member variables
  uint8_t       report_cfg_id = 1;
  report_cfg_c_ report_cfg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaConfiguration-v1700 ::= SEQUENCE
struct area_cfg_v1700_s {
  using inter_freq_target_list_r17_l_ = dyn_array<inter_freq_target_info_r16_s>;

  // member variables
  bool                          area_cfg_r17_present = false;
  area_cfg_r16_c                area_cfg_r17;
  inter_freq_target_list_r17_l_ inter_freq_target_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierInfoNR ::= SEQUENCE
struct carrier_info_nr_s {
  bool                 ext          = false;
  bool                 smtc_present = false;
  uint32_t             carrier_freq = 0;
  subcarrier_spacing_e ssb_subcarrier_spacing;
  ssb_mtc_s            smtc;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransfer-v1700-IEs ::= SEQUENCE
struct dl_info_transfer_v1700_ies_s {
  struct ta_pdc_r17_opts {
    enum options { activ, deactiv, nulltype } value;

    const char* to_string() const;
  };
  using ta_pdc_r17_e_ = enumerated<ta_pdc_r17_opts>;

  // member variables
  bool                  rx_tx_time_diff_gnb_r17_present = false;
  bool                  ta_pdc_r17_present              = false;
  bool                  sib9_fallback_r17_present       = false;
  bool                  non_crit_ext_present            = false;
  dyn_octstring         ded_info_f1c_r17;
  rx_tx_time_diff_r17_s rx_tx_time_diff_gnb_r17;
  ta_pdc_r17_e_         ta_pdc_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-CountMSB-Info ::= SEQUENCE
struct drb_count_msb_info_s {
  uint8_t  drb_id       = 1;
  uint32_t count_msb_ul = 0;
  uint32_t count_msb_dl = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventType-r16 ::= CHOICE
struct event_type_r16_c {
  struct event_l1_s_ {
    meas_trigger_quant_c l1_thres;
    uint8_t              hysteresis = 0;
    time_to_trigger_e    time_to_trigger;
  };
  struct types_opts {
    enum options { out_of_coverage, event_l1, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  event_type_r16_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  event_l1_s_& event_l1()
  {
    assert_choice_type(types::event_l1, type_, "EventType-r16");
    return c;
  }
  const event_l1_s_& event_l1() const
  {
    assert_choice_type(types::event_l1, type_, "EventType-r16");
    return c;
  }
  void         set_out_of_coverage();
  event_l1_s_& set_event_l1();

private:
  types       type_;
  event_l1_s_ c;
};

// ExtendedPagingCycle-r17 ::= ENUMERATED
struct extended_paging_cycle_r17_opts {
  enum options { rf256, rf512, rf1024, spare1, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using extended_paging_cycle_r17_e = enumerated<extended_paging_cycle_r17_opts>;

// FreqPriorityListDedicatedSlicing-r17 ::= SEQUENCE (SIZE (1..8)) OF FreqPriorityDedicatedSlicing-r17
using freq_prio_list_ded_slicing_r17_l = dyn_array<freq_prio_ded_slicing_r17_s>;

// FreqPriorityListEUTRA ::= SEQUENCE (SIZE (1..8)) OF FreqPriorityEUTRA
using freq_prio_list_eutra_l = dyn_array<freq_prio_eutra_s>;

// FreqPriorityListNR ::= SEQUENCE (SIZE (1..8)) OF FreqPriorityNR
using freq_prio_list_nr_l = dyn_array<freq_prio_nr_s>;

// LoggingInterval-r16 ::= ENUMERATED
struct logging_interv_r16_opts {
  enum options {
    ms320,
    ms640,
    ms1280,
    ms2560,
    ms5120,
    ms10240,
    ms20480,
    ms30720,
    ms40960,
    ms61440,
    infinity,
    nulltype
  } value;
  typedef int32_t number_type;

  const char* to_string() const;
  int32_t     to_number() const;
};
using logging_interv_r16_e = enumerated<logging_interv_r16_opts>;

// MasterKeyUpdate ::= SEQUENCE
struct master_key_upd_s {
  bool          ext                     = false;
  bool          key_set_change_ind      = false;
  uint8_t       next_hop_chaining_count = 0;
  dyn_octstring nas_container;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasGapConfig ::= SEQUENCE
struct meas_gap_cfg_s {
  using gap_to_add_mod_list_r17_l_ = dyn_array<gap_cfg_r17_s>;
  using gap_to_release_list_r17_l_ = bounded_array<uint8_t, 8>;

  // member variables
  bool                       ext             = false;
  bool                       gap_fr2_present = false;
  setup_release_c<gap_cfg_s> gap_fr2;
  // ...
  // group 0
  copy_ptr<setup_release_c<gap_cfg_s>> gap_fr1;
  copy_ptr<setup_release_c<gap_cfg_s>> gap_ue;
  // group 1
  copy_ptr<gap_to_add_mod_list_r17_l_>                 gap_to_add_mod_list_r17;
  copy_ptr<gap_to_release_list_r17_l_>                 gap_to_release_list_r17;
  copy_ptr<pos_meas_gap_pre_cfg_to_add_mod_list_r17_l> pos_meas_gap_pre_cfg_to_add_mod_list_r17;
  copy_ptr<pos_meas_gap_pre_cfg_to_release_list_r17_l> pos_meas_gap_pre_cfg_to_release_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasGapSharingConfig ::= SEQUENCE
struct meas_gap_sharing_cfg_s {
  bool                                       ext                     = false;
  bool                                       gap_sharing_fr2_present = false;
  setup_release_c<meas_gap_sharing_scheme_e> gap_sharing_fr2;
  // ...
  // group 0
  copy_ptr<setup_release_c<meas_gap_sharing_scheme_e>> gap_sharing_fr1;
  copy_ptr<setup_release_c<meas_gap_sharing_scheme_e>> gap_sharing_ue;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasIdToAddModList ::= SEQUENCE (SIZE (1..64)) OF MeasIdToAddMod
using meas_id_to_add_mod_list_l = dyn_array<meas_id_to_add_mod_s>;

// MeasIdToRemoveList ::= SEQUENCE (SIZE (1..64)) OF INTEGER (1..64)
using meas_id_to_rem_list_l = dyn_array<uint8_t>;

// MeasObjectToAddModList ::= SEQUENCE (SIZE (1..64)) OF MeasObjectToAddMod
using meas_obj_to_add_mod_list_l = dyn_array<meas_obj_to_add_mod_s>;

// MeasObjectToRemoveList ::= SEQUENCE (SIZE (1..64)) OF INTEGER (1..64)
using meas_obj_to_rem_list_l = dyn_array<uint8_t>;

// OtherConfig ::= SEQUENCE
struct other_cfg_s {
  struct delay_budget_report_cfg_c_ {
    struct setup_s_ {
      struct delay_budget_report_prohibit_timer_opts {
        enum options { s0, s0dot4, s0dot8, s1dot6, s3, s6, s12, s30, nulltype } value;
        typedef float number_type;

        const char* to_string() const;
        float       to_number() const;
        const char* to_number_string() const;
      };
      using delay_budget_report_prohibit_timer_e_ = enumerated<delay_budget_report_prohibit_timer_opts>;

      // member variables
      delay_budget_report_prohibit_timer_e_ delay_budget_report_prohibit_timer;
    };
    struct types_opts {
      enum options { release, setup, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    delay_budget_report_cfg_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "delayBudgetReportingConfig");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "delayBudgetReportingConfig");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                       delay_budget_report_cfg_present = false;
  delay_budget_report_cfg_c_ delay_budget_report_cfg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PeriodicRNAU-TimerValue ::= ENUMERATED
struct periodic_rnau_timer_value_opts {
  enum options { min5, min10, min20, min30, min60, min120, min360, min720, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using periodic_rnau_timer_value_e = enumerated<periodic_rnau_timer_value_opts>;

// QuantityConfig ::= SEQUENCE
struct quant_cfg_s {
  using quant_cfg_nr_list_l_ = dyn_array<quant_cfg_nr_s>;

  // member variables
  bool                 ext = false;
  quant_cfg_nr_list_l_ quant_cfg_nr_list;
  // ...
  // group 0
  copy_ptr<filt_cfg_s> quant_cfg_eutra;
  // group 1
  copy_ptr<quant_cfg_utra_fdd_r16_s> quant_cfg_utra_fdd_r16;
  copy_ptr<filt_cfg_cli_r16_s>       quant_cfg_cli_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RAN-NotificationAreaInfo ::= CHOICE
struct ran_notif_area_info_c {
  struct types_opts {
    enum options { cell_list, ran_area_cfg_list, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  ran_notif_area_info_c() = default;
  ran_notif_area_info_c(const ran_notif_area_info_c& other);
  ran_notif_area_info_c& operator=(const ran_notif_area_info_c& other);
  ~ran_notif_area_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  plmn_ran_area_cell_list_l& cell_list()
  {
    assert_choice_type(types::cell_list, type_, "RAN-NotificationAreaInfo");
    return c.get<plmn_ran_area_cell_list_l>();
  }
  plmn_ran_area_cfg_list_l& ran_area_cfg_list()
  {
    assert_choice_type(types::ran_area_cfg_list, type_, "RAN-NotificationAreaInfo");
    return c.get<plmn_ran_area_cfg_list_l>();
  }
  const plmn_ran_area_cell_list_l& cell_list() const
  {
    assert_choice_type(types::cell_list, type_, "RAN-NotificationAreaInfo");
    return c.get<plmn_ran_area_cell_list_l>();
  }
  const plmn_ran_area_cfg_list_l& ran_area_cfg_list() const
  {
    assert_choice_type(types::ran_area_cfg_list, type_, "RAN-NotificationAreaInfo");
    return c.get<plmn_ran_area_cfg_list_l>();
  }
  plmn_ran_area_cell_list_l& set_cell_list();
  plmn_ran_area_cfg_list_l&  set_ran_area_cfg_list();

private:
  types                                                                type_;
  choice_buffer_t<plmn_ran_area_cell_list_l, plmn_ran_area_cfg_list_l> c;

  void destroy_();
};

// RRCReconfiguration-v1540-IEs ::= SEQUENCE
struct rrc_recfg_v1540_ies_s {
  bool                  other_cfg_v1540_present = false;
  bool                  non_crit_ext_present    = false;
  other_cfg_v1540_s     other_cfg_v1540;
  rrc_recfg_v1560_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCRelease-v1610-IEs ::= SEQUENCE
struct rrc_release_v1610_ies_s {
  bool                                     voice_fallback_ind_r16_present = false;
  bool                                     meas_idle_cfg_r16_present      = false;
  bool                                     non_crit_ext_present           = false;
  setup_release_c<meas_idle_cfg_ded_r16_s> meas_idle_cfg_r16;
  rrc_release_v1650_ies_s                  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResume-v1610-IEs ::= SEQUENCE
struct rrc_resume_v1610_ies_s {
  struct mrdc_secondary_cell_group_r16_c_ {
    struct types_opts {
      enum options { nr_scg_r16, eutra_scg_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    mrdc_secondary_cell_group_r16_c_() = default;
    mrdc_secondary_cell_group_r16_c_(const mrdc_secondary_cell_group_r16_c_& other);
    mrdc_secondary_cell_group_r16_c_& operator=(const mrdc_secondary_cell_group_r16_c_& other);
    ~mrdc_secondary_cell_group_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dyn_octstring& nr_scg_r16()
    {
      assert_choice_type(types::nr_scg_r16, type_, "mrdc-SecondaryCellGroup-r16");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& eutra_scg_r16()
    {
      assert_choice_type(types::eutra_scg_r16, type_, "mrdc-SecondaryCellGroup-r16");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& nr_scg_r16() const
    {
      assert_choice_type(types::nr_scg_r16, type_, "mrdc-SecondaryCellGroup-r16");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& eutra_scg_r16() const
    {
      assert_choice_type(types::eutra_scg_r16, type_, "mrdc-SecondaryCellGroup-r16");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& set_nr_scg_r16();
    dyn_octstring& set_eutra_scg_r16();

  private:
    types                          type_;
    choice_buffer_t<dyn_octstring> c;

    void destroy_();
  };

  // member variables
  bool                                        idle_mode_meas_req_r16_present        = false;
  bool                                        restore_mcg_scells_r16_present        = false;
  bool                                        restore_scg_r16_present               = false;
  bool                                        mrdc_secondary_cell_group_r16_present = false;
  bool                                        need_for_gaps_cfg_nr_r16_present      = false;
  bool                                        non_crit_ext_present                  = false;
  mrdc_secondary_cell_group_r16_c_            mrdc_secondary_cell_group_r16;
  setup_release_c<need_for_gaps_cfg_nr_r16_s> need_for_gaps_cfg_nr_r16;
  rrc_resume_v1700_ies_s                      non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RedirectedCarrierInfo-EUTRA ::= SEQUENCE
struct redirected_carrier_info_eutra_s {
  struct cn_type_opts {
    enum options { epc, five_gc, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using cn_type_e_ = enumerated<cn_type_opts>;

  // member variables
  bool       cn_type_present = false;
  uint32_t   eutra_freq      = 0;
  cn_type_e_ cn_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportConfigToAddModList ::= SEQUENCE (SIZE (1..64)) OF ReportConfigToAddMod
using report_cfg_to_add_mod_list_l = dyn_array<report_cfg_to_add_mod_s>;

// ReportConfigToRemoveList ::= SEQUENCE (SIZE (1..64)) OF INTEGER (1..64)
using report_cfg_to_rem_list_l = dyn_array<uint8_t>;

// SDT-Config-r17 ::= SEQUENCE
struct sdt_cfg_r17_s {
  using sdt_drb_list_r17_l_ = bounded_array<uint8_t, 29>;
  struct sdt_drb_continue_rohc_r17_opts {
    enum options { cell, rna, nulltype } value;

    const char* to_string() const;
  };
  using sdt_drb_continue_rohc_r17_e_ = enumerated<sdt_drb_continue_rohc_r17_opts>;

  // member variables
  bool                           sdt_drb_list_r17_present          = false;
  bool                           sdt_srb2_ind_r17_present          = false;
  bool                           sdt_mac_phy_cg_cfg_r17_present    = false;
  bool                           sdt_drb_continue_rohc_r17_present = false;
  sdt_drb_list_r17_l_            sdt_drb_list_r17;
  setup_release_c<dyn_octstring> sdt_mac_phy_cg_cfg_r17;
  sdt_drb_continue_rohc_r17_e_   sdt_drb_continue_rohc_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRAT-Request ::= SEQUENCE
struct ue_cap_rat_request_s {
  bool          ext = false;
  rat_type_e    rat_type;
  dyn_octstring cap_request_filt;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaConfiguration-r16 ::= SEQUENCE
struct area_cfg_r16_s {
  using inter_freq_target_list_r16_l_ = dyn_array<inter_freq_target_info_r16_s>;

  // member variables
  area_cfg_r16_c                area_cfg_r16;
  inter_freq_target_list_r16_l_ inter_freq_target_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellReselectionPriorities ::= SEQUENCE
struct cell_resel_priorities_s {
  struct t320_opts {
    enum options { min5, min10, min20, min30, min60, min120, min180, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using t320_e_ = enumerated<t320_opts>;

  // member variables
  bool                   ext          = false;
  bool                   t320_present = false;
  freq_prio_list_eutra_l freq_prio_list_eutra;
  freq_prio_list_nr_l    freq_prio_list_nr;
  t320_e_                t320;
  // ...
  // group 0
  copy_ptr<freq_prio_list_ded_slicing_r17_l> freq_prio_list_ded_slicing_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransfer-v1610-IEs ::= SEQUENCE
struct dl_info_transfer_v1610_ies_s {
  bool                         ref_time_info_r16_present = false;
  bool                         non_crit_ext_present      = false;
  ref_time_info_r16_s          ref_time_info_r16;
  dl_info_transfer_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-CountMSB-InfoList ::= SEQUENCE (SIZE (1..29)) OF DRB-CountMSB-Info
using drb_count_msb_info_list_l = dyn_array<drb_count_msb_info_s>;

// LoggedEventTriggerConfig-r16 ::= SEQUENCE
struct logged_event_trigger_cfg_r16_s {
  bool                 ext = false;
  event_type_r16_c     event_type_r16;
  logging_interv_r16_e logging_interv_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggedMeasurementConfiguration-v1700-IEs ::= SEQUENCE
struct logged_meas_cfg_v1700_ies_s {
  bool             sig_logged_meas_type_r17_present = false;
  bool             early_meas_ind_r17_present       = false;
  bool             area_cfg_v1700_present           = false;
  bool             non_crit_ext_present             = false;
  area_cfg_v1700_s area_cfg_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggedPeriodicalReportConfig-r16 ::= SEQUENCE
struct logged_periodical_report_cfg_r16_s {
  bool                 ext = false;
  logging_interv_r16_e logging_interv_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggingDuration-r16 ::= ENUMERATED
struct logging_dur_r16_opts {
  enum options { min10, min20, min40, min60, min90, min120, spare2, spare1, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using logging_dur_r16_e = enumerated<logging_dur_r16_opts>;

// MeasConfig ::= SEQUENCE
struct meas_cfg_s {
  struct s_measure_cfg_c_ {
    struct types_opts {
      enum options { ssb_rsrp, csi_rsrp, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    s_measure_cfg_c_() = default;
    s_measure_cfg_c_(const s_measure_cfg_c_& other);
    s_measure_cfg_c_& operator=(const s_measure_cfg_c_& other);
    ~s_measure_cfg_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_rsrp()
    {
      assert_choice_type(types::ssb_rsrp, type_, "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rsrp()
    {
      assert_choice_type(types::csi_rsrp, type_, "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_rsrp() const
    {
      assert_choice_type(types::ssb_rsrp, type_, "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rsrp() const
    {
      assert_choice_type(types::csi_rsrp, type_, "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_rsrp();
    uint8_t& set_csi_rsrp();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                         ext                          = false;
  bool                         s_measure_cfg_present        = false;
  bool                         quant_cfg_present            = false;
  bool                         meas_gap_cfg_present         = false;
  bool                         meas_gap_sharing_cfg_present = false;
  meas_obj_to_rem_list_l       meas_obj_to_rem_list;
  meas_obj_to_add_mod_list_l   meas_obj_to_add_mod_list;
  report_cfg_to_rem_list_l     report_cfg_to_rem_list;
  report_cfg_to_add_mod_list_l report_cfg_to_add_mod_list;
  meas_id_to_rem_list_l        meas_id_to_rem_list;
  meas_id_to_add_mod_list_l    meas_id_to_add_mod_list;
  s_measure_cfg_c_             s_measure_cfg;
  quant_cfg_s                  quant_cfg;
  meas_gap_cfg_s               meas_gap_cfg;
  meas_gap_sharing_cfg_s       meas_gap_sharing_cfg;
  // ...
  // group 0
  bool interfreq_cfg_no_gap_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityFromNRCommand-v1610-IEs ::= SEQUENCE
struct mob_from_nr_cmd_v1610_ies_s {
  bool voice_fallback_ind_r16_present = false;
  bool non_crit_ext_present           = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityList2-r16 ::= SEQUENCE (SIZE (1..16)) OF PLMN-Identity
using plmn_id_list2_r16_l = dyn_array<plmn_id_s>;

// RRCReconfiguration-v1530-IEs ::= SEQUENCE
struct rrc_recfg_v1530_ies_s {
  using ded_nas_msg_list_l_ = bounded_array<dyn_octstring, 29>;

  // member variables
  bool                  full_cfg_present       = false;
  bool                  master_key_upd_present = false;
  bool                  other_cfg_present      = false;
  bool                  non_crit_ext_present   = false;
  dyn_octstring         master_cell_group;
  ded_nas_msg_list_l_   ded_nas_msg_list;
  master_key_upd_s      master_key_upd;
  dyn_octstring         ded_sib1_delivery;
  dyn_octstring         ded_sys_info_delivery;
  other_cfg_s           other_cfg;
  rrc_recfg_v1540_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReestablishment-v1700-IEs ::= SEQUENCE
struct rrc_reest_v1700_ies_s {
  bool                                       sl_l2_remote_ue_cfg_r17_present = false;
  bool                                       non_crit_ext_present            = false;
  setup_release_c<sl_l2_remote_ue_cfg_r17_s> sl_l2_remote_ue_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCRelease-v1540-IEs ::= SEQUENCE
struct rrc_release_v1540_ies_s {
  bool                    wait_time_present    = false;
  bool                    non_crit_ext_present = false;
  uint8_t                 wait_time            = 1;
  rrc_release_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResume-v1560-IEs ::= SEQUENCE
struct rrc_resume_v1560_ies_s {
  bool                   sk_counter_present   = false;
  bool                   non_crit_ext_present = false;
  dyn_octstring          radio_bearer_cfg2;
  uint32_t               sk_counter = 0;
  rrc_resume_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RedirectedCarrierInfo ::= CHOICE
struct redirected_carrier_info_c {
  struct types_opts {
    enum options { nr, eutra, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  redirected_carrier_info_c() = default;
  redirected_carrier_info_c(const redirected_carrier_info_c& other);
  redirected_carrier_info_c& operator=(const redirected_carrier_info_c& other);
  ~redirected_carrier_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  carrier_info_nr_s& nr()
  {
    assert_choice_type(types::nr, type_, "RedirectedCarrierInfo");
    return c.get<carrier_info_nr_s>();
  }
  redirected_carrier_info_eutra_s& eutra()
  {
    assert_choice_type(types::eutra, type_, "RedirectedCarrierInfo");
    return c.get<redirected_carrier_info_eutra_s>();
  }
  const carrier_info_nr_s& nr() const
  {
    assert_choice_type(types::nr, type_, "RedirectedCarrierInfo");
    return c.get<carrier_info_nr_s>();
  }
  const redirected_carrier_info_eutra_s& eutra() const
  {
    assert_choice_type(types::eutra, type_, "RedirectedCarrierInfo");
    return c.get<redirected_carrier_info_eutra_s>();
  }
  carrier_info_nr_s&               set_nr();
  redirected_carrier_info_eutra_s& set_eutra();

private:
  types                                                               type_;
  choice_buffer_t<carrier_info_nr_s, redirected_carrier_info_eutra_s> c;

  void destroy_();
};

// SecurityConfigSMC ::= SEQUENCE
struct security_cfg_smc_s {
  bool                     ext = false;
  security_algorithm_cfg_s security_algorithm_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SuspendConfig ::= SEQUENCE
struct suspend_cfg_s {
  bool                        ext                         = false;
  bool                        ran_notif_area_info_present = false;
  bool                        t380_present                = false;
  fixed_bitstring<40>         full_i_rnti;
  fixed_bitstring<24>         short_i_rnti;
  paging_cycle_e              ran_paging_cycle;
  ran_notif_area_info_c       ran_notif_area_info;
  periodic_rnau_timer_value_e t380;
  uint8_t                     next_hop_chaining_count = 0;
  // ...
  // group 0
  bool                                     sl_ue_id_remote_r17_present           = false;
  bool                                     ran_extended_paging_cycle_r17_present = false;
  uint32_t                                 sl_ue_id_remote_r17                   = 0;
  copy_ptr<setup_release_c<sdt_cfg_r17_s>> sdt_cfg_r17;
  copy_ptr<setup_release_c<dyn_octstring>> srs_pos_rrc_inactive_r17;
  extended_paging_cycle_r17_e              ran_extended_paging_cycle_r17;
  // group 1
  copy_ptr<setup_release_c<non_cell_defining_ssb_r17_s>> ncd_ssb_red_cap_init_bwp_sdt_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRAT-RequestList ::= SEQUENCE (SIZE (1..8)) OF UE-CapabilityRAT-Request
using ue_cap_rat_request_list_l = dyn_array<ue_cap_rat_request_s>;

// UEInformationRequest-v1700-IEs ::= SEQUENCE
struct ue_info_request_v1700_ies_s {
  bool success_ho_report_req_r17_present   = false;
  bool coarse_location_request_r17_present = false;
  bool non_crit_ext_present                = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CounterCheck-IEs ::= SEQUENCE
struct counter_check_ies_s {
  bool                      non_crit_ext_present = false;
  drb_count_msb_info_list_l drb_count_msb_info_list;
  dyn_octstring             late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLDedicatedMessageSegment-r16-IEs ::= SEQUENCE
struct dl_ded_msg_segment_r16_ies_s {
  struct rrc_msg_segment_type_r16_opts {
    enum options { not_last_segment, last_segment, nulltype } value;

    const char* to_string() const;
  };
  using rrc_msg_segment_type_r16_e_ = enumerated<rrc_msg_segment_type_r16_opts>;

  // member variables
  bool                        non_crit_ext_present = false;
  uint8_t                     segment_num_r16      = 0;
  dyn_octstring               rrc_msg_segment_container_r16;
  rrc_msg_segment_type_r16_e_ rrc_msg_segment_type_r16;
  dyn_octstring               late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransfer-IEs ::= SEQUENCE
struct dl_info_transfer_ies_s {
  bool                         non_crit_ext_present = false;
  dyn_octstring                ded_nas_msg;
  dyn_octstring                late_non_crit_ext;
  dl_info_transfer_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransferMRDC-r16-IEs ::= SEQUENCE
struct dl_info_transfer_mrdc_r16_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring dl_dcch_msg_nr_r16;
  dyn_octstring dl_dcch_msg_eutra_r16;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggedMeasurementConfiguration-r16-IEs ::= SEQUENCE
struct logged_meas_cfg_r16_ies_s {
  struct report_type_c_ {
    struct types_opts {
      enum options { periodical, event_triggered, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    report_type_c_() = default;
    report_type_c_(const report_type_c_& other);
    report_type_c_& operator=(const report_type_c_& other);
    ~report_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    logged_periodical_report_cfg_r16_s& periodical()
    {
      assert_choice_type(types::periodical, type_, "reportType");
      return c.get<logged_periodical_report_cfg_r16_s>();
    }
    logged_event_trigger_cfg_r16_s& event_triggered()
    {
      assert_choice_type(types::event_triggered, type_, "reportType");
      return c.get<logged_event_trigger_cfg_r16_s>();
    }
    const logged_periodical_report_cfg_r16_s& periodical() const
    {
      assert_choice_type(types::periodical, type_, "reportType");
      return c.get<logged_periodical_report_cfg_r16_s>();
    }
    const logged_event_trigger_cfg_r16_s& event_triggered() const
    {
      assert_choice_type(types::event_triggered, type_, "reportType");
      return c.get<logged_event_trigger_cfg_r16_s>();
    }
    logged_periodical_report_cfg_r16_s& set_periodical();
    logged_event_trigger_cfg_r16_s&     set_event_triggered();

  private:
    types                                                                               type_;
    choice_buffer_t<logged_event_trigger_cfg_r16_s, logged_periodical_report_cfg_r16_s> c;

    void destroy_();
  };

  // member variables
  bool                                                         area_cfg_r16_present         = false;
  bool                                                         bt_name_list_r16_present     = false;
  bool                                                         wlan_name_list_r16_present   = false;
  bool                                                         sensor_name_list_r16_present = false;
  bool                                                         non_crit_ext_present         = false;
  trace_ref_r16_s                                              trace_ref_r16;
  fixed_octstring<2>                                           trace_recording_session_ref_r16;
  fixed_octstring<1>                                           tce_id_r16;
  fixed_bitstring<48>                                          absolute_time_info_r16;
  area_cfg_r16_s                                               area_cfg_r16;
  plmn_id_list2_r16_l                                          plmn_id_list_r16;
  setup_release_c<dyn_seq_of<bounded_octstring<1, 248>, 1, 4>> bt_name_list_r16;
  setup_release_c<dyn_seq_of<bounded_octstring<1, 32>, 1, 4>>  wlan_name_list_r16;
  setup_release_c<sensor_name_list_r16_s>                      sensor_name_list_r16;
  logging_dur_r16_e                                            logging_dur_r16;
  report_type_c_                                               report_type;
  dyn_octstring                                                late_non_crit_ext;
  logged_meas_cfg_v1700_ies_s                                  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityFromNRCommand-IEs ::= SEQUENCE
struct mob_from_nr_cmd_ies_s {
  struct target_rat_type_opts {
    enum options { eutra, utra_fdd_v1610, spare2, spare1, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using target_rat_type_e_ = enumerated<target_rat_type_opts, true>;

  // member variables
  bool                        non_crit_ext_present = false;
  target_rat_type_e_          target_rat_type;
  dyn_octstring               target_rat_msg_container;
  dyn_octstring               nas_security_param_from_nr;
  dyn_octstring               late_non_crit_ext;
  mob_from_nr_cmd_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfiguration-IEs ::= SEQUENCE
struct rrc_recfg_ies_s {
  bool                  radio_bearer_cfg_present = false;
  bool                  meas_cfg_present         = false;
  bool                  non_crit_ext_present     = false;
  radio_bearer_cfg_s    radio_bearer_cfg;
  dyn_octstring         secondary_cell_group;
  meas_cfg_s            meas_cfg;
  dyn_octstring         late_non_crit_ext;
  rrc_recfg_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReestablishment-IEs ::= SEQUENCE
struct rrc_reest_ies_s {
  bool                  non_crit_ext_present    = false;
  uint8_t               next_hop_chaining_count = 0;
  dyn_octstring         late_non_crit_ext;
  rrc_reest_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCRelease-IEs ::= SEQUENCE
struct rrc_release_ies_s {
  struct depriorit_req_s_ {
    struct depriorit_type_opts {
      enum options { freq, nr, nulltype } value;

      const char* to_string() const;
    };
    using depriorit_type_e_ = enumerated<depriorit_type_opts>;
    struct depriorit_timer_opts {
      enum options { min5, min10, min15, min30, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using depriorit_timer_e_ = enumerated<depriorit_timer_opts>;

    // member variables
    depriorit_type_e_  depriorit_type;
    depriorit_timer_e_ depriorit_timer;
  };

  // member variables
  bool                      redirected_carrier_info_present = false;
  bool                      cell_resel_priorities_present   = false;
  bool                      suspend_cfg_present             = false;
  bool                      depriorit_req_present           = false;
  bool                      non_crit_ext_present            = false;
  redirected_carrier_info_c redirected_carrier_info;
  cell_resel_priorities_s   cell_resel_priorities;
  suspend_cfg_s             suspend_cfg;
  depriorit_req_s_          depriorit_req;
  dyn_octstring             late_non_crit_ext;
  rrc_release_v1540_ies_s   non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResume-IEs ::= SEQUENCE
struct rrc_resume_ies_s {
  bool                   radio_bearer_cfg_present = false;
  bool                   meas_cfg_present         = false;
  bool                   full_cfg_present         = false;
  bool                   non_crit_ext_present     = false;
  radio_bearer_cfg_s     radio_bearer_cfg;
  dyn_octstring          master_cell_group;
  meas_cfg_s             meas_cfg;
  dyn_octstring          late_non_crit_ext;
  rrc_resume_v1560_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeCommand-IEs ::= SEQUENCE
struct security_mode_cmd_ies_s {
  bool               non_crit_ext_present = false;
  security_cfg_smc_s security_cfg_smc;
  dyn_octstring      late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityEnquiry-IEs ::= SEQUENCE
struct ue_cap_enquiry_ies_s {
  ue_cap_rat_request_list_l ue_cap_rat_request_list;
  dyn_octstring             late_non_crit_ext;
  dyn_octstring             ue_cap_enquiry_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationRequest-r16-IEs ::= SEQUENCE
struct ue_info_request_r16_ies_s {
  bool                        idle_mode_meas_req_r16_present       = false;
  bool                        log_meas_report_req_r16_present      = false;
  bool                        conn_est_fail_report_req_r16_present = false;
  bool                        ra_report_req_r16_present            = false;
  bool                        rlf_report_req_r16_present           = false;
  bool                        mob_history_report_req_r16_present   = false;
  bool                        non_crit_ext_present                 = false;
  dyn_octstring               late_non_crit_ext;
  ue_info_request_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CounterCheck ::= SEQUENCE
struct counter_check_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { counter_check, crit_exts_future, nulltype } value;

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
    counter_check_ies_s& counter_check()
    {
      assert_choice_type(types::counter_check, type_, "criticalExtensions");
      return c;
    }
    const counter_check_ies_s& counter_check() const
    {
      assert_choice_type(types::counter_check, type_, "criticalExtensions");
      return c;
    }
    counter_check_ies_s& set_counter_check();
    void                 set_crit_exts_future();

  private:
    types               type_;
    counter_check_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLDedicatedMessageSegment-r16 ::= SEQUENCE
struct dl_ded_msg_segment_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { dl_ded_msg_segment_r16, crit_exts_future, nulltype } value;

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
    dl_ded_msg_segment_r16_ies_s& dl_ded_msg_segment_r16()
    {
      assert_choice_type(types::dl_ded_msg_segment_r16, type_, "criticalExtensions");
      return c;
    }
    const dl_ded_msg_segment_r16_ies_s& dl_ded_msg_segment_r16() const
    {
      assert_choice_type(types::dl_ded_msg_segment_r16, type_, "criticalExtensions");
      return c;
    }
    dl_ded_msg_segment_r16_ies_s& set_dl_ded_msg_segment_r16();
    void                          set_crit_exts_future();

  private:
    types                        type_;
    dl_ded_msg_segment_r16_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransfer ::= SEQUENCE
struct dl_info_transfer_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { dl_info_transfer, crit_exts_future, nulltype } value;

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
    dl_info_transfer_ies_s& dl_info_transfer()
    {
      assert_choice_type(types::dl_info_transfer, type_, "criticalExtensions");
      return c;
    }
    const dl_info_transfer_ies_s& dl_info_transfer() const
    {
      assert_choice_type(types::dl_info_transfer, type_, "criticalExtensions");
      return c;
    }
    dl_info_transfer_ies_s& set_dl_info_transfer();
    void                    set_crit_exts_future();

  private:
    types                  type_;
    dl_info_transfer_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransferMRDC-r16 ::= SEQUENCE
struct dl_info_transfer_mrdc_r16_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { dl_info_transfer_mrdc_r16, spare3, spare2, spare1, nulltype } value;

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
      dl_info_transfer_mrdc_r16_ies_s& dl_info_transfer_mrdc_r16()
      {
        assert_choice_type(types::dl_info_transfer_mrdc_r16, type_, "c1");
        return c;
      }
      const dl_info_transfer_mrdc_r16_ies_s& dl_info_transfer_mrdc_r16() const
      {
        assert_choice_type(types::dl_info_transfer_mrdc_r16, type_, "c1");
        return c;
      }
      dl_info_transfer_mrdc_r16_ies_s& set_dl_info_transfer_mrdc_r16();
      void                             set_spare3();
      void                             set_spare2();
      void                             set_spare1();

    private:
      types                           type_;
      dl_info_transfer_mrdc_r16_ies_s c;
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

// LoggedMeasurementConfiguration-r16 ::= SEQUENCE
struct logged_meas_cfg_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { logged_meas_cfg_r16, crit_exts_future, nulltype } value;

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
    logged_meas_cfg_r16_ies_s& logged_meas_cfg_r16()
    {
      assert_choice_type(types::logged_meas_cfg_r16, type_, "criticalExtensions");
      return c;
    }
    const logged_meas_cfg_r16_ies_s& logged_meas_cfg_r16() const
    {
      assert_choice_type(types::logged_meas_cfg_r16, type_, "criticalExtensions");
      return c;
    }
    logged_meas_cfg_r16_ies_s& set_logged_meas_cfg_r16();
    void                       set_crit_exts_future();

  private:
    types                     type_;
    logged_meas_cfg_r16_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityFromNRCommand ::= SEQUENCE
struct mob_from_nr_cmd_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { mob_from_nr_cmd, crit_exts_future, nulltype } value;

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
    mob_from_nr_cmd_ies_s& mob_from_nr_cmd()
    {
      assert_choice_type(types::mob_from_nr_cmd, type_, "criticalExtensions");
      return c;
    }
    const mob_from_nr_cmd_ies_s& mob_from_nr_cmd() const
    {
      assert_choice_type(types::mob_from_nr_cmd, type_, "criticalExtensions");
      return c;
    }
    mob_from_nr_cmd_ies_s& set_mob_from_nr_cmd();
    void                   set_crit_exts_future();

  private:
    types                 type_;
    mob_from_nr_cmd_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfiguration ::= SEQUENCE
struct rrc_recfg_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_recfg, crit_exts_future, nulltype } value;

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
    rrc_recfg_ies_s& rrc_recfg()
    {
      assert_choice_type(types::rrc_recfg, type_, "criticalExtensions");
      return c;
    }
    const rrc_recfg_ies_s& rrc_recfg() const
    {
      assert_choice_type(types::rrc_recfg, type_, "criticalExtensions");
      return c;
    }
    rrc_recfg_ies_s& set_rrc_recfg();
    void             set_crit_exts_future();

  private:
    types           type_;
    rrc_recfg_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReestablishment ::= SEQUENCE
struct rrc_reest_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_reest, crit_exts_future, nulltype } value;

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
    rrc_reest_ies_s& rrc_reest()
    {
      assert_choice_type(types::rrc_reest, type_, "criticalExtensions");
      return c;
    }
    const rrc_reest_ies_s& rrc_reest() const
    {
      assert_choice_type(types::rrc_reest, type_, "criticalExtensions");
      return c;
    }
    rrc_reest_ies_s& set_rrc_reest();
    void             set_crit_exts_future();

  private:
    types           type_;
    rrc_reest_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCRelease ::= SEQUENCE
struct rrc_release_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_release, crit_exts_future, nulltype } value;

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
    rrc_release_ies_s& rrc_release()
    {
      assert_choice_type(types::rrc_release, type_, "criticalExtensions");
      return c;
    }
    const rrc_release_ies_s& rrc_release() const
    {
      assert_choice_type(types::rrc_release, type_, "criticalExtensions");
      return c;
    }
    rrc_release_ies_s& set_rrc_release();
    void               set_crit_exts_future();

  private:
    types             type_;
    rrc_release_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResume ::= SEQUENCE
struct rrc_resume_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_resume, crit_exts_future, nulltype } value;

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
    rrc_resume_ies_s& rrc_resume()
    {
      assert_choice_type(types::rrc_resume, type_, "criticalExtensions");
      return c;
    }
    const rrc_resume_ies_s& rrc_resume() const
    {
      assert_choice_type(types::rrc_resume, type_, "criticalExtensions");
      return c;
    }
    rrc_resume_ies_s& set_rrc_resume();
    void              set_crit_exts_future();

  private:
    types            type_;
    rrc_resume_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeCommand ::= SEQUENCE
struct security_mode_cmd_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { security_mode_cmd, crit_exts_future, nulltype } value;

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
    security_mode_cmd_ies_s& security_mode_cmd()
    {
      assert_choice_type(types::security_mode_cmd, type_, "criticalExtensions");
      return c;
    }
    const security_mode_cmd_ies_s& security_mode_cmd() const
    {
      assert_choice_type(types::security_mode_cmd, type_, "criticalExtensions");
      return c;
    }
    security_mode_cmd_ies_s& set_security_mode_cmd();
    void                     set_crit_exts_future();

  private:
    types                   type_;
    security_mode_cmd_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityEnquiry ::= SEQUENCE
struct ue_cap_enquiry_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_cap_enquiry, crit_exts_future, nulltype } value;

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
    ue_cap_enquiry_ies_s& ue_cap_enquiry()
    {
      assert_choice_type(types::ue_cap_enquiry, type_, "criticalExtensions");
      return c;
    }
    const ue_cap_enquiry_ies_s& ue_cap_enquiry() const
    {
      assert_choice_type(types::ue_cap_enquiry, type_, "criticalExtensions");
      return c;
    }
    ue_cap_enquiry_ies_s& set_ue_cap_enquiry();
    void                  set_crit_exts_future();

  private:
    types                type_;
    ue_cap_enquiry_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationRequest-r16 ::= SEQUENCE
struct ue_info_request_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_info_request_r16, crit_exts_future, nulltype } value;

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
    ue_info_request_r16_ies_s& ue_info_request_r16()
    {
      assert_choice_type(types::ue_info_request_r16, type_, "criticalExtensions");
      return c;
    }
    const ue_info_request_r16_ies_s& ue_info_request_r16() const
    {
      assert_choice_type(types::ue_info_request_r16, type_, "criticalExtensions");
      return c;
    }
    ue_info_request_r16_ies_s& set_ue_info_request_r16();
    void                       set_crit_exts_future();

  private:
    types                     type_;
    ue_info_request_r16_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-DCCH-MessageType ::= CHOICE
struct dl_dcch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options {
        rrc_recfg,
        rrc_resume,
        rrc_release,
        rrc_reest,
        security_mode_cmd,
        dl_info_transfer,
        ue_cap_enquiry,
        counter_check,
        mob_from_nr_cmd,
        dl_ded_msg_segment_r16,
        ue_info_request_r16,
        dl_info_transfer_mrdc_r16,
        logged_meas_cfg_r16,
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
    c1_c_(const c1_c_& other);
    c1_c_& operator=(const c1_c_& other);
    ~c1_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_recfg_s& rrc_recfg()
    {
      assert_choice_type(types::rrc_recfg, type_, "c1");
      return c.get<rrc_recfg_s>();
    }
    rrc_resume_s& rrc_resume()
    {
      assert_choice_type(types::rrc_resume, type_, "c1");
      return c.get<rrc_resume_s>();
    }
    rrc_release_s& rrc_release()
    {
      assert_choice_type(types::rrc_release, type_, "c1");
      return c.get<rrc_release_s>();
    }
    rrc_reest_s& rrc_reest()
    {
      assert_choice_type(types::rrc_reest, type_, "c1");
      return c.get<rrc_reest_s>();
    }
    security_mode_cmd_s& security_mode_cmd()
    {
      assert_choice_type(types::security_mode_cmd, type_, "c1");
      return c.get<security_mode_cmd_s>();
    }
    dl_info_transfer_s& dl_info_transfer()
    {
      assert_choice_type(types::dl_info_transfer, type_, "c1");
      return c.get<dl_info_transfer_s>();
    }
    ue_cap_enquiry_s& ue_cap_enquiry()
    {
      assert_choice_type(types::ue_cap_enquiry, type_, "c1");
      return c.get<ue_cap_enquiry_s>();
    }
    counter_check_s& counter_check()
    {
      assert_choice_type(types::counter_check, type_, "c1");
      return c.get<counter_check_s>();
    }
    mob_from_nr_cmd_s& mob_from_nr_cmd()
    {
      assert_choice_type(types::mob_from_nr_cmd, type_, "c1");
      return c.get<mob_from_nr_cmd_s>();
    }
    dl_ded_msg_segment_r16_s& dl_ded_msg_segment_r16()
    {
      assert_choice_type(types::dl_ded_msg_segment_r16, type_, "c1");
      return c.get<dl_ded_msg_segment_r16_s>();
    }
    ue_info_request_r16_s& ue_info_request_r16()
    {
      assert_choice_type(types::ue_info_request_r16, type_, "c1");
      return c.get<ue_info_request_r16_s>();
    }
    dl_info_transfer_mrdc_r16_s& dl_info_transfer_mrdc_r16()
    {
      assert_choice_type(types::dl_info_transfer_mrdc_r16, type_, "c1");
      return c.get<dl_info_transfer_mrdc_r16_s>();
    }
    logged_meas_cfg_r16_s& logged_meas_cfg_r16()
    {
      assert_choice_type(types::logged_meas_cfg_r16, type_, "c1");
      return c.get<logged_meas_cfg_r16_s>();
    }
    const rrc_recfg_s& rrc_recfg() const
    {
      assert_choice_type(types::rrc_recfg, type_, "c1");
      return c.get<rrc_recfg_s>();
    }
    const rrc_resume_s& rrc_resume() const
    {
      assert_choice_type(types::rrc_resume, type_, "c1");
      return c.get<rrc_resume_s>();
    }
    const rrc_release_s& rrc_release() const
    {
      assert_choice_type(types::rrc_release, type_, "c1");
      return c.get<rrc_release_s>();
    }
    const rrc_reest_s& rrc_reest() const
    {
      assert_choice_type(types::rrc_reest, type_, "c1");
      return c.get<rrc_reest_s>();
    }
    const security_mode_cmd_s& security_mode_cmd() const
    {
      assert_choice_type(types::security_mode_cmd, type_, "c1");
      return c.get<security_mode_cmd_s>();
    }
    const dl_info_transfer_s& dl_info_transfer() const
    {
      assert_choice_type(types::dl_info_transfer, type_, "c1");
      return c.get<dl_info_transfer_s>();
    }
    const ue_cap_enquiry_s& ue_cap_enquiry() const
    {
      assert_choice_type(types::ue_cap_enquiry, type_, "c1");
      return c.get<ue_cap_enquiry_s>();
    }
    const counter_check_s& counter_check() const
    {
      assert_choice_type(types::counter_check, type_, "c1");
      return c.get<counter_check_s>();
    }
    const mob_from_nr_cmd_s& mob_from_nr_cmd() const
    {
      assert_choice_type(types::mob_from_nr_cmd, type_, "c1");
      return c.get<mob_from_nr_cmd_s>();
    }
    const dl_ded_msg_segment_r16_s& dl_ded_msg_segment_r16() const
    {
      assert_choice_type(types::dl_ded_msg_segment_r16, type_, "c1");
      return c.get<dl_ded_msg_segment_r16_s>();
    }
    const ue_info_request_r16_s& ue_info_request_r16() const
    {
      assert_choice_type(types::ue_info_request_r16, type_, "c1");
      return c.get<ue_info_request_r16_s>();
    }
    const dl_info_transfer_mrdc_r16_s& dl_info_transfer_mrdc_r16() const
    {
      assert_choice_type(types::dl_info_transfer_mrdc_r16, type_, "c1");
      return c.get<dl_info_transfer_mrdc_r16_s>();
    }
    const logged_meas_cfg_r16_s& logged_meas_cfg_r16() const
    {
      assert_choice_type(types::logged_meas_cfg_r16, type_, "c1");
      return c.get<logged_meas_cfg_r16_s>();
    }
    rrc_recfg_s&                 set_rrc_recfg();
    rrc_resume_s&                set_rrc_resume();
    rrc_release_s&               set_rrc_release();
    rrc_reest_s&                 set_rrc_reest();
    security_mode_cmd_s&         set_security_mode_cmd();
    dl_info_transfer_s&          set_dl_info_transfer();
    ue_cap_enquiry_s&            set_ue_cap_enquiry();
    counter_check_s&             set_counter_check();
    mob_from_nr_cmd_s&           set_mob_from_nr_cmd();
    dl_ded_msg_segment_r16_s&    set_dl_ded_msg_segment_r16();
    ue_info_request_r16_s&       set_ue_info_request_r16();
    dl_info_transfer_mrdc_r16_s& set_dl_info_transfer_mrdc_r16();
    logged_meas_cfg_r16_s&       set_logged_meas_cfg_r16();
    void                         set_spare3();
    void                         set_spare2();
    void                         set_spare1();

  private:
    types type_;
    choice_buffer_t<counter_check_s,
                    dl_ded_msg_segment_r16_s,
                    dl_info_transfer_mrdc_r16_s,
                    dl_info_transfer_s,
                    logged_meas_cfg_r16_s,
                    mob_from_nr_cmd_s,
                    rrc_recfg_s,
                    rrc_reest_s,
                    rrc_release_s,
                    rrc_resume_s,
                    security_mode_cmd_s,
                    ue_cap_enquiry_s,
                    ue_info_request_r16_s>
        c;

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
  dl_dcch_msg_type_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "DL-DCCH-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "DL-DCCH-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// DL-DCCH-Message ::= SEQUENCE
struct dl_dcch_msg_s {
  dl_dcch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1

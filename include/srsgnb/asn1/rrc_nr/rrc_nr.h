/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/*******************************************************************************
 *
 *                    3GPP TS ASN1 RRC NR v15.11.0 (2020-10)
 *
 ******************************************************************************/

#ifndef SRSGNB_ASN1_RRC_NR_RRC_NR_H
#define SRSGNB_ASN1_RRC_NR_RRC_NR_H

#include "serving_cell.h"
#include "ue_cap.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// PDCCH-ConfigSIB1 ::= SEQUENCE
struct pdcch_cfg_sib1_s {
  uint8_t coreset_zero      = 0;
  uint8_t search_space_zero = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// EUTRA-Q-OffsetRange ::= ENUMERATED
struct eutra_q_offset_range_opts {
  enum options {
    db_minus24,
    db_minus22,
    db_minus20,
    db_minus18,
    db_minus16,
    db_minus14,
    db_minus12,
    db_minus10,
    db_minus8,
    db_minus6,
    db_minus5,
    db_minus4,
    db_minus3,
    db_minus2,
    db_minus1,
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
typedef enumerated<eutra_q_offset_range_opts> eutra_q_offset_range_e;

// Q-OffsetRange ::= ENUMERATED
struct q_offset_range_opts {
  enum options {
    db_minus24,
    db_minus22,
    db_minus20,
    db_minus18,
    db_minus16,
    db_minus14,
    db_minus12,
    db_minus10,
    db_minus8,
    db_minus6,
    db_minus5,
    db_minus4,
    db_minus3,
    db_minus2,
    db_minus1,
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
typedef enumerated<q_offset_range_opts> q_offset_range_e;

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
  typedef enumerated<range_opts> range_e_;

  // member variables
  bool     range_present = false;
  uint16_t start         = 0;
  range_e_ range;

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
  typedef enumerated<range_opts> range_e_;

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
typedef enumerated<cell_resel_sub_prio_opts> cell_resel_sub_prio_e;

// EUTRA-AllowedMeasBandwidth ::= ENUMERATED
struct eutra_allowed_meas_bw_opts {
  enum options { mbw6, mbw15, mbw25, mbw50, mbw75, mbw100, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<eutra_allowed_meas_bw_opts> eutra_allowed_meas_bw_e;

// EUTRA-FreqBlackCellList ::= SEQUENCE (SIZE (1..16)) OF EUTRA-PhysCellIdRange
using eutra_freq_black_cell_list_l = dyn_array<eutra_pci_range_s>;

// EUTRA-FreqNeighCellList ::= SEQUENCE (SIZE (1..8)) OF EUTRA-FreqNeighCellInfo
using eutra_freq_neigh_cell_list_l = dyn_array<eutra_freq_neigh_cell_info_s>;

// EUTRA-MultiBandInfoList ::= SEQUENCE (SIZE (1..8)) OF EUTRA-MultiBandInfo
using eutra_multi_band_info_list_l = dyn_array<eutra_multi_band_info_s>;

// InterFreqBlackCellList ::= SEQUENCE (SIZE (1..16)) OF PCI-Range
using inter_freq_black_cell_list_l = dyn_array<pci_range_s>;

// InterFreqNeighCellList ::= SEQUENCE (SIZE (1..16)) OF InterFreqNeighCellInfo
using inter_freq_neigh_cell_list_l = dyn_array<inter_freq_neigh_cell_info_s>;

// MCC ::= SEQUENCE (SIZE (3)) OF INTEGER (0..9)
using mcc_l = std::array<uint8_t, 3>;

// MNC ::= SEQUENCE (SIZE (2..3)) OF INTEGER (0..9)
using mnc_l = bounded_array<uint8_t, 3>;

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
    typedef enumerated<types_opts> types;

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
  typedef enumerated<dur_opts> dur_e_;

  // member variables
  periodicity_and_offset_c_ periodicity_and_offset;
  dur_e_                    dur;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-ToMeasure ::= CHOICE
struct ssb_to_measure_c {
  struct types_opts {
    enum options { short_bitmap, medium_bitmap, long_bitmap, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

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
    return c.get<fixed_bitstring<4> >();
  }
  fixed_bitstring<8>& medium_bitmap()
  {
    assert_choice_type(types::medium_bitmap, type_, "SSB-ToMeasure");
    return c.get<fixed_bitstring<8> >();
  }
  fixed_bitstring<64>& long_bitmap()
  {
    assert_choice_type(types::long_bitmap, type_, "SSB-ToMeasure");
    return c.get<fixed_bitstring<64> >();
  }
  const fixed_bitstring<4>& short_bitmap() const
  {
    assert_choice_type(types::short_bitmap, type_, "SSB-ToMeasure");
    return c.get<fixed_bitstring<4> >();
  }
  const fixed_bitstring<8>& medium_bitmap() const
  {
    assert_choice_type(types::medium_bitmap, type_, "SSB-ToMeasure");
    return c.get<fixed_bitstring<8> >();
  }
  const fixed_bitstring<64>& long_bitmap() const
  {
    assert_choice_type(types::long_bitmap, type_, "SSB-ToMeasure");
    return c.get<fixed_bitstring<64> >();
  }
  fixed_bitstring<4>&  set_short_bitmap();
  fixed_bitstring<8>&  set_medium_bitmap();
  fixed_bitstring<64>& set_long_bitmap();

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<64> > c;

  void destroy_();
};

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

// CarrierFreqEUTRA ::= SEQUENCE
struct carrier_freq_eutra_s {
  struct thresh_x_q_s_ {
    uint8_t thresh_x_high_q = 0;
    uint8_t thresh_x_low_q  = 0;
  };

  // member variables
  bool                         cell_resel_prio_present     = false;
  bool                         cell_resel_sub_prio_present = false;
  bool                         thresh_x_q_present          = false;
  uint32_t                     carrier_freq                = 0;
  eutra_multi_band_info_list_l eutra_multi_band_info_list;
  eutra_freq_neigh_cell_list_l eutra_freq_neigh_cell_list;
  eutra_freq_black_cell_list_l eutra_black_cell_list;
  eutra_allowed_meas_bw_e      allowed_meas_bw;
  bool                         presence_ant_port1 = false;
  uint8_t                      cell_resel_prio    = 0;
  cell_resel_sub_prio_e        cell_resel_sub_prio;
  uint8_t                      thresh_x_high = 0;
  uint8_t                      thresh_x_low  = 0;
  int8_t                       q_rx_lev_min  = -70;
  int8_t                       q_qual_min    = -34;
  int8_t                       p_max_eutra   = -30;
  thresh_x_q_s_                thresh_x_q;

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
  bool                          ext                                        = false;
  bool                          nrof_ss_blocks_to_average_present          = false;
  bool                          abs_thresh_ss_blocks_consolidation_present = false;
  bool                          smtc_present                               = false;
  bool                          ssb_to_measure_present                     = false;
  bool                          ss_rssi_meas_present                       = false;
  bool                          q_rx_lev_min_sul_present                   = false;
  bool                          q_qual_min_present                         = false;
  bool                          p_max_present                              = false;
  bool                          t_resel_nr_sf_present                      = false;
  bool                          thresh_x_q_present                         = false;
  bool                          cell_resel_prio_present                    = false;
  bool                          cell_resel_sub_prio_present                = false;
  bool                          q_offset_freq_present                      = false;
  uint32_t                      dl_carrier_freq                            = 0;
  multi_freq_band_list_nr_sib_l freq_band_list;
  multi_freq_band_list_nr_sib_l freq_band_list_sul;
  uint8_t                       nrof_ss_blocks_to_average = 2;
  thres_nr_s                    abs_thresh_ss_blocks_consolidation;
  ssb_mtc_s                     smtc;
  subcarrier_spacing_e          ssb_subcarrier_spacing;
  ssb_to_measure_c              ssb_to_measure;
  bool                          derive_ssb_idx_from_cell = false;
  ss_rssi_meas_s                ss_rssi_meas;
  int8_t                        q_rx_lev_min     = -70;
  int8_t                        q_rx_lev_min_sul = -70;
  int8_t                        q_qual_min       = -43;
  int8_t                        p_max            = -30;
  uint8_t                       t_resel_nr       = 0;
  speed_state_scale_factors_s   t_resel_nr_sf;
  uint8_t                       thresh_x_high_p = 0;
  uint8_t                       thresh_x_low_p  = 0;
  thresh_x_q_s_                 thresh_x_q;
  uint8_t                       cell_resel_prio = 0;
  cell_resel_sub_prio_e         cell_resel_sub_prio;
  q_offset_range_e              q_offset_freq;
  inter_freq_neigh_cell_list_l  inter_freq_neigh_cell_list;
  inter_freq_black_cell_list_l  inter_freq_black_cell_list;
  // ...

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

// PLMN-Identity ::= SEQUENCE
struct plmn_id_s {
  bool  mcc_present = false;
  mcc_l mcc;
  mnc_l mnc;

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

// UAC-BarringPerCat ::= SEQUENCE
struct uac_barr_per_cat_s {
  uint8_t access_category       = 1;
  uint8_t uac_barr_info_set_idx = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqListEUTRA ::= SEQUENCE (SIZE (1..8)) OF CarrierFreqEUTRA
using carrier_freq_list_eutra_l = dyn_array<carrier_freq_eutra_s>;

// InterFreqCarrierFreqList ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo
using inter_freq_carrier_freq_list_l = dyn_array<inter_freq_carrier_freq_info_s>;

// IntraFreqBlackCellList ::= SEQUENCE (SIZE (1..16)) OF PCI-Range
using intra_freq_black_cell_list_l = dyn_array<pci_range_s>;

// IntraFreqNeighCellList ::= SEQUENCE (SIZE (1..16)) OF IntraFreqNeighCellInfo
using intra_freq_neigh_cell_list_l = dyn_array<intra_freq_neigh_cell_info_s>;

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

// PLMN-IdentityInfo ::= SEQUENCE
struct plmn_id_info_s {
  using plmn_id_list_l_ = dyn_array<plmn_id_s>;
  struct cell_reserved_for_oper_opts {
    enum options { reserved, not_reserved, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<cell_reserved_for_oper_opts> cell_reserved_for_oper_e_;

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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RangeToBestCell ::= Q-OffsetRange
using range_to_best_cell_e = q_offset_range_e;

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

// SIB-Mapping ::= SEQUENCE (SIZE (1..32)) OF SIB-TypeInfo
using sib_map_l = dyn_array<sib_type_info_s>;

// UAC-BarringPerCatList ::= SEQUENCE (SIZE (1..63)) OF UAC-BarringPerCat
using uac_barr_per_cat_list_l = dyn_array<uac_barr_per_cat_s>;

// PLMN-IdentityInfoList ::= SEQUENCE (SIZE (1..12)) OF PLMN-IdentityInfo
using plmn_id_info_list_l = dyn_array<plmn_id_info_s>;

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
          enum options { db_minus6, db_minus4, db_minus2, db0, nulltype } value;
          typedef int8_t number_type;

          const char* to_string() const;
          int8_t      to_number() const;
        };
        typedef enumerated<sf_medium_opts> sf_medium_e_;
        struct sf_high_opts {
          enum options { db_minus6, db_minus4, db_minus2, db0, nulltype } value;
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

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool                            ext = false;
  cell_resel_info_common_s_       cell_resel_info_common;
  cell_resel_serving_freq_info_s_ cell_resel_serving_freq_info;
  intra_freq_cell_resel_info_s_   intra_freq_cell_resel_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB3 ::= SEQUENCE
struct sib3_s {
  bool                         ext = false;
  intra_freq_neigh_cell_list_l intra_freq_neigh_cell_list;
  intra_freq_black_cell_list_l intra_freq_black_cell_list;
  dyn_octstring                late_non_crit_ext;
  // ...

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

// CellAccessRelatedInfo ::= SEQUENCE
struct cell_access_related_info_s {
  bool                ext                                 = false;
  bool                cell_reserved_for_other_use_present = false;
  plmn_id_info_list_l plmn_id_list;
  // ...

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

// SI-SchedulingInfo ::= SEQUENCE
struct si_sched_info_s {
  using sched_info_list_l_ = dyn_array<sched_info_s>;
  struct si_win_len_opts {
    enum options { s5, s10, s20, s40, s80, s160, s320, s640, s1280, nulltype } value;
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

// SystemInformation-IEs ::= SEQUENCE
struct sys_info_ies_s {
  struct sib_type_and_info_item_c_ {
    struct types_opts {
      enum options { sib2, sib3, sib4, sib5, sib6, sib7, sib8, sib9, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts, true> types;

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
    sib2_s& set_sib2();
    sib3_s& set_sib3();
    sib4_s& set_sib4();
    sib5_s& set_sib5();
    sib6_s& set_sib6();
    sib7_s& set_sib7();
    sib8_s& set_sib8();
    sib9_s& set_sib9();

  private:
    types                                                                           type_;
    choice_buffer_t<sib2_s, sib3_s, sib4_s, sib5_s, sib6_s, sib7_s, sib8_s, sib9_s> c;

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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformation ::= SEQUENCE
struct sys_info_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { sys_info, crit_exts_future, nulltype } value;

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
    sys_info_ies_s& sys_info()
    {
      assert_choice_type(types::sys_info, type_, "criticalExtensions");
      return c;
    }
    const sys_info_ies_s& sys_info() const
    {
      assert_choice_type(types::sys_info, type_, "criticalExtensions");
      return c;
    }
    sys_info_ies_s& set_sys_info();
    void            set_crit_exts_future();

  private:
    types          type_;
    sys_info_ies_s c;
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

// UL-DataSplitThreshold ::= ENUMERATED
struct ul_data_split_thres_opts {
  enum options {
    b0,
    b100,
    b200,
    b400,
    b800,
    b1600,
    b3200,
    b6400,
    b12800,
    b25600,
    b51200,
    b102400,
    b204800,
    b409600,
    b819200,
    b1228800,
    b1638400,
    b2457600,
    b3276800,
    b4096000,
    b4915200,
    b5734400,
    b6553600,
    infinity,
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
  typedef int32_t number_type;

  const char* to_string() const;
  int32_t     to_number() const;
};
typedef enumerated<ul_data_split_thres_opts> ul_data_split_thres_e;

// CipheringAlgorithm ::= ENUMERATED
struct ciphering_algorithm_opts {
  enum options { nea0, nea1, nea2, nea3, spare4, spare3, spare2, spare1, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<ciphering_algorithm_opts, true> ciphering_algorithm_e;

// IntegrityProtAlgorithm ::= ENUMERATED
struct integrity_prot_algorithm_opts {
  enum options { nia0, nia1, nia2, nia3, spare4, spare3, spare2, spare1, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<integrity_prot_algorithm_opts, true> integrity_prot_algorithm_e;

// PDCP-Config ::= SEQUENCE
struct pdcp_cfg_s {
  struct drb_s_ {
    struct discard_timer_opts {
      enum options {
        ms10,
        ms20,
        ms30,
        ms40,
        ms50,
        ms60,
        ms75,
        ms100,
        ms150,
        ms200,
        ms250,
        ms300,
        ms500,
        ms750,
        ms1500,
        infinity,
        nulltype
      } value;
      typedef int16_t number_type;

      const char* to_string() const;
      int16_t     to_number() const;
    };
    typedef enumerated<discard_timer_opts> discard_timer_e_;
    struct pdcp_sn_size_ul_opts {
      enum options { len12bits, len18bits, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<pdcp_sn_size_ul_opts> pdcp_sn_size_ul_e_;
    struct pdcp_sn_size_dl_opts {
      enum options { len12bits, len18bits, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<pdcp_sn_size_dl_opts> pdcp_sn_size_dl_e_;
    struct hdr_compress_c_ {
      struct rohc_s_ {
        struct profiles_s_ {
          bool profile0x0001 = false;
          bool profile0x0002 = false;
          bool profile0x0003 = false;
          bool profile0x0004 = false;
          bool profile0x0006 = false;
          bool profile0x0101 = false;
          bool profile0x0102 = false;
          bool profile0x0103 = false;
          bool profile0x0104 = false;
        };

        // member variables
        bool        max_cid_present           = false;
        bool        drb_continue_rohc_present = false;
        uint16_t    max_cid                   = 1;
        profiles_s_ profiles;
      };
      struct ul_only_rohc_s_ {
        struct profiles_s_ {
          bool profile0x0006 = false;
        };

        // member variables
        bool        max_cid_present           = false;
        bool        drb_continue_rohc_present = false;
        uint16_t    max_cid                   = 1;
        profiles_s_ profiles;
      };
      struct types_opts {
        enum options { not_used, rohc, ul_only_rohc, /*...*/ nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts, true> types;

      // choice methods
      hdr_compress_c_() = default;
      hdr_compress_c_(const hdr_compress_c_& other);
      hdr_compress_c_& operator=(const hdr_compress_c_& other);
      ~hdr_compress_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      rohc_s_& rohc()
      {
        assert_choice_type(types::rohc, type_, "headerCompression");
        return c.get<rohc_s_>();
      }
      ul_only_rohc_s_& ul_only_rohc()
      {
        assert_choice_type(types::ul_only_rohc, type_, "headerCompression");
        return c.get<ul_only_rohc_s_>();
      }
      const rohc_s_& rohc() const
      {
        assert_choice_type(types::rohc, type_, "headerCompression");
        return c.get<rohc_s_>();
      }
      const ul_only_rohc_s_& ul_only_rohc() const
      {
        assert_choice_type(types::ul_only_rohc, type_, "headerCompression");
        return c.get<ul_only_rohc_s_>();
      }
      void             set_not_used();
      rohc_s_&         set_rohc();
      ul_only_rohc_s_& set_ul_only_rohc();

    private:
      types                                     type_;
      choice_buffer_t<rohc_s_, ul_only_rohc_s_> c;

      void destroy_();
    };

    // member variables
    bool               discard_timer_present          = false;
    bool               pdcp_sn_size_ul_present        = false;
    bool               pdcp_sn_size_dl_present        = false;
    bool               integrity_protection_present   = false;
    bool               status_report_required_present = false;
    bool               out_of_order_delivery_present  = false;
    discard_timer_e_   discard_timer;
    pdcp_sn_size_ul_e_ pdcp_sn_size_ul;
    pdcp_sn_size_dl_e_ pdcp_sn_size_dl;
    hdr_compress_c_    hdr_compress;
  };
  struct more_than_one_rlc_s_ {
    struct primary_path_s_ {
      bool    cell_group_present = false;
      bool    lc_ch_present      = false;
      uint8_t cell_group         = 0;
      uint8_t lc_ch              = 1;
    };

    // member variables
    bool                  ul_data_split_thres_present = false;
    bool                  pdcp_dupl_present           = false;
    primary_path_s_       primary_path;
    ul_data_split_thres_e ul_data_split_thres;
    bool                  pdcp_dupl = false;
  };
  struct t_reordering_opts {
    enum options {
      ms0,
      ms1,
      ms2,
      ms4,
      ms5,
      ms8,
      ms10,
      ms15,
      ms20,
      ms30,
      ms40,
      ms50,
      ms60,
      ms80,
      ms100,
      ms120,
      ms140,
      ms160,
      ms180,
      ms200,
      ms220,
      ms240,
      ms260,
      ms280,
      ms300,
      ms500,
      ms750,
      ms1000,
      ms1250,
      ms1500,
      ms1750,
      ms2000,
      ms2250,
      ms2500,
      ms2750,
      ms3000,
      spare28,
      spare27,
      spare26,
      spare25,
      spare24,
      spare23,
      spare22,
      spare21,
      spare20,
      spare19,
      spare18,
      spare17,
      spare16,
      spare15,
      spare14,
      spare13,
      spare12,
      spare11,
      spare10,
      spare09,
      spare08,
      spare07,
      spare06,
      spare05,
      spare04,
      spare03,
      spare02,
      spare01,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t_reordering_opts> t_reordering_e_;

  // member variables
  bool                 ext                       = false;
  bool                 drb_present               = false;
  bool                 more_than_one_rlc_present = false;
  bool                 t_reordering_present      = false;
  drb_s_               drb;
  more_than_one_rlc_s_ more_than_one_rlc;
  t_reordering_e_      t_reordering;
  // ...
  // group 0
  bool ciphering_disabled_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SDAP-Config ::= SEQUENCE
struct sdap_cfg_s {
  struct sdap_hdr_dl_opts {
    enum options { present, absent, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<sdap_hdr_dl_opts> sdap_hdr_dl_e_;
  struct sdap_hdr_ul_opts {
    enum options { present, absent, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<sdap_hdr_ul_opts> sdap_hdr_ul_e_;
  using mapped_qos_flows_to_add_l_     = dyn_array<uint8_t>;
  using mapped_qos_flows_to_release_l_ = dyn_array<uint8_t>;

  // member variables
  bool                           ext         = false;
  uint16_t                       pdu_session = 0;
  sdap_hdr_dl_e_                 sdap_hdr_dl;
  sdap_hdr_ul_e_                 sdap_hdr_ul;
  bool                           default_drb = false;
  mapped_qos_flows_to_add_l_     mapped_qos_flows_to_add;
  mapped_qos_flows_to_release_l_ mapped_qos_flows_to_release;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-ToAddMod ::= SEQUENCE
struct drb_to_add_mod_s {
  struct cn_assoc_c_ {
    struct types_opts {
      enum options { eps_bearer_id, sdap_cfg, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    cn_assoc_c_() = default;
    cn_assoc_c_(const cn_assoc_c_& other);
    cn_assoc_c_& operator=(const cn_assoc_c_& other);
    ~cn_assoc_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& eps_bearer_id()
    {
      assert_choice_type(types::eps_bearer_id, type_, "cnAssociation");
      return c.get<uint8_t>();
    }
    sdap_cfg_s& sdap_cfg()
    {
      assert_choice_type(types::sdap_cfg, type_, "cnAssociation");
      return c.get<sdap_cfg_s>();
    }
    const uint8_t& eps_bearer_id() const
    {
      assert_choice_type(types::eps_bearer_id, type_, "cnAssociation");
      return c.get<uint8_t>();
    }
    const sdap_cfg_s& sdap_cfg() const
    {
      assert_choice_type(types::sdap_cfg, type_, "cnAssociation");
      return c.get<sdap_cfg_s>();
    }
    uint8_t&    set_eps_bearer_id();
    sdap_cfg_s& set_sdap_cfg();

  private:
    types                       type_;
    choice_buffer_t<sdap_cfg_s> c;

    void destroy_();
  };

  // member variables
  bool        ext                      = false;
  bool        cn_assoc_present         = false;
  bool        reestablish_pdcp_present = false;
  bool        recover_pdcp_present     = false;
  bool        pdcp_cfg_present         = false;
  cn_assoc_c_ cn_assoc;
  uint8_t     drb_id = 1;
  pdcp_cfg_s  pdcp_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRB-ToAddMod ::= SEQUENCE
struct srb_to_add_mod_s {
  bool       ext                      = false;
  bool       reestablish_pdcp_present = false;
  bool       discard_on_pdcp_present  = false;
  bool       pdcp_cfg_present         = false;
  uint8_t    srb_id                   = 1;
  pdcp_cfg_s pdcp_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityAlgorithmConfig ::= SEQUENCE
struct security_algorithm_cfg_s {
  bool                       ext                              = false;
  bool                       integrity_prot_algorithm_present = false;
  ciphering_algorithm_e      ciphering_algorithm;
  integrity_prot_algorithm_e integrity_prot_algorithm;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-ToAddModList ::= SEQUENCE (SIZE (1..29)) OF DRB-ToAddMod
using drb_to_add_mod_list_l = dyn_array<drb_to_add_mod_s>;

// DRB-ToReleaseList ::= SEQUENCE (SIZE (1..29)) OF INTEGER (1..32)
using drb_to_release_list_l = bounded_array<uint8_t, 29>;

// SRB-ToAddModList ::= SEQUENCE (SIZE (1..2)) OF SRB-ToAddMod
using srb_to_add_mod_list_l = dyn_array<srb_to_add_mod_s>;

// SecurityConfig ::= SEQUENCE
struct security_cfg_s {
  struct key_to_use_opts {
    enum options { master, secondary, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<key_to_use_opts> key_to_use_e_;

  // member variables
  bool                     ext                            = false;
  bool                     security_algorithm_cfg_present = false;
  bool                     key_to_use_present             = false;
  security_algorithm_cfg_s security_algorithm_cfg;
  key_to_use_e_            key_to_use;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioBearerConfig ::= SEQUENCE
struct radio_bearer_cfg_s {
  bool                  ext                     = false;
  bool                  srb3_to_release_present = false;
  bool                  security_cfg_present    = false;
  srb_to_add_mod_list_l srb_to_add_mod_list;
  drb_to_add_mod_list_l drb_to_add_mod_list;
  drb_to_release_list_l drb_to_release_list;
  security_cfg_s        security_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReject-IEs ::= SEQUENCE
struct rrc_reject_ies_s {
  bool          wait_time_present    = false;
  bool          non_crit_ext_present = false;
  uint8_t       wait_time            = 1;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetup-IEs ::= SEQUENCE
struct rrc_setup_ies_s {
  bool               non_crit_ext_present = false;
  radio_bearer_cfg_s radio_bearer_cfg;
  dyn_octstring      master_cell_group;
  dyn_octstring      late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReject ::= SEQUENCE
struct rrc_reject_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_reject, crit_exts_future, nulltype } value;

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
    rrc_reject_ies_s& rrc_reject()
    {
      assert_choice_type(types::rrc_reject, type_, "criticalExtensions");
      return c;
    }
    const rrc_reject_ies_s& rrc_reject() const
    {
      assert_choice_type(types::rrc_reject, type_, "criticalExtensions");
      return c;
    }
    rrc_reject_ies_s& set_rrc_reject();
    void              set_crit_exts_future();

  private:
    types            type_;
    rrc_reject_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetup ::= SEQUENCE
struct rrc_setup_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_setup, crit_exts_future, nulltype } value;

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
    rrc_setup_ies_s& rrc_setup()
    {
      assert_choice_type(types::rrc_setup, type_, "criticalExtensions");
      return c;
    }
    const rrc_setup_ies_s& rrc_setup() const
    {
      assert_choice_type(types::rrc_setup, type_, "criticalExtensions");
      return c;
    }
    rrc_setup_ies_s& set_rrc_setup();
    void             set_crit_exts_future();

  private:
    types           type_;
    rrc_setup_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-CCCH-MessageType ::= CHOICE
struct dl_ccch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { rrc_reject, rrc_setup, spare2, spare1, nulltype } value;

      const char* to_string() const;
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
    rrc_reject_s& rrc_reject()
    {
      assert_choice_type(types::rrc_reject, type_, "c1");
      return c.get<rrc_reject_s>();
    }
    rrc_setup_s& rrc_setup()
    {
      assert_choice_type(types::rrc_setup, type_, "c1");
      return c.get<rrc_setup_s>();
    }
    const rrc_reject_s& rrc_reject() const
    {
      assert_choice_type(types::rrc_reject, type_, "c1");
      return c.get<rrc_reject_s>();
    }
    const rrc_setup_s& rrc_setup() const
    {
      assert_choice_type(types::rrc_setup, type_, "c1");
      return c.get<rrc_setup_s>();
    }
    rrc_reject_s& set_rrc_reject();
    rrc_setup_s&  set_rrc_setup();
    void          set_spare2();
    void          set_spare1();

  private:
    types                                      type_;
    choice_buffer_t<rrc_reject_s, rrc_setup_s> c;

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
  dl_ccch_msg_type_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "DL-CCCH-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "DL-CCCH-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// DL-CCCH-Message ::= SEQUENCE
struct dl_ccch_msg_s {
  dl_ccch_msg_type_c msg;

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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
      return c.get<fixed_bitstring<4> >();
    }
    fixed_bitstring<12>& row2()
    {
      assert_choice_type(types::row2, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<12> >();
    }
    const fixed_bitstring<4>& row1() const
    {
      assert_choice_type(types::row1, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<4> >();
    }
    const fixed_bitstring<12>& row2() const
    {
      assert_choice_type(types::row2, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<12> >();
    }
    fixed_bitstring<4>&  set_row1();
    fixed_bitstring<12>& set_row2();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<12> > c;

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
    typedef enumerated<nrof_prbs_opts> nrof_prbs_e_;

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
  typedef enumerated<density_opts> density_e_;
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

// FilterCoefficient ::= ENUMERATED
struct filt_coef_opts {
  enum options {
    fc0,
    fc1,
    fc2,
    fc3,
    fc4,
    fc5,
    fc6,
    fc7,
    fc8,
    fc9,
    fc11,
    fc13,
    fc15,
    fc17,
    fc19,
    spare1,
    /*...*/ nulltype
  } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<filt_coef_opts, true> filt_coef_e;

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

// MeasTriggerQuantity ::= CHOICE
struct meas_trigger_quant_c {
  struct types_opts {
    enum options { rsrp, rsrq, sinr, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  meas_trigger_quant_c() = default;
  meas_trigger_quant_c(const meas_trigger_quant_c& other);
  meas_trigger_quant_c& operator=(const meas_trigger_quant_c& other);
  ~meas_trigger_quant_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& rsrp()
  {
    assert_choice_type(types::rsrp, type_, "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  uint8_t& rsrq()
  {
    assert_choice_type(types::rsrq, type_, "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  uint8_t& sinr()
  {
    assert_choice_type(types::sinr, type_, "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  const uint8_t& rsrp() const
  {
    assert_choice_type(types::rsrp, type_, "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  const uint8_t& rsrq() const
  {
    assert_choice_type(types::rsrq, type_, "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  const uint8_t& sinr() const
  {
    assert_choice_type(types::sinr, type_, "MeasTriggerQuantity");
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

// MeasTriggerQuantityEUTRA ::= CHOICE
struct meas_trigger_quant_eutra_c {
  struct types_opts {
    enum options { rsrp, rsrq, sinr, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

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

// MeasTriggerQuantityOffset ::= CHOICE
struct meas_trigger_quant_offset_c {
  struct types_opts {
    enum options { rsrp, rsrq, sinr, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  meas_trigger_quant_offset_c() = default;
  meas_trigger_quant_offset_c(const meas_trigger_quant_offset_c& other);
  meas_trigger_quant_offset_c& operator=(const meas_trigger_quant_offset_c& other);
  ~meas_trigger_quant_offset_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  int8_t& rsrp()
  {
    assert_choice_type(types::rsrp, type_, "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  int8_t& rsrq()
  {
    assert_choice_type(types::rsrq, type_, "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  int8_t& sinr()
  {
    assert_choice_type(types::sinr, type_, "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  const int8_t& rsrp() const
  {
    assert_choice_type(types::rsrp, type_, "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  const int8_t& rsrq() const
  {
    assert_choice_type(types::rsrq, type_, "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  const int8_t& sinr() const
  {
    assert_choice_type(types::sinr, type_, "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  int8_t& set_rsrp();
  int8_t& set_rsrq();
  int8_t& set_sinr();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// NR-RS-Type ::= ENUMERATED
struct nr_rs_type_opts {
  enum options { ssb, csi_rs, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<nr_rs_type_opts> nr_rs_type_e;

// ReportInterval ::= ENUMERATED
struct report_interv_opts {
  enum options {
    ms120,
    ms240,
    ms480,
    ms640,
    ms1024,
    ms2048,
    ms5120,
    ms10240,
    ms20480,
    ms40960,
    min1,
    min6,
    min12,
    min30,
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<report_interv_opts> report_interv_e;

// SSB-ConfigMobility ::= SEQUENCE
struct ssb_cfg_mob_s {
  bool                              ext                    = false;
  bool                              ssb_to_measure_present = false;
  bool                              ss_rssi_meas_present   = false;
  setup_release_c<ssb_to_measure_c> ssb_to_measure;
  bool                              derive_ssb_idx_from_cell = false;
  ss_rssi_meas_s                    ss_rssi_meas;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TimeToTrigger ::= ENUMERATED
struct time_to_trigger_opts {
  enum options {
    ms0,
    ms40,
    ms64,
    ms80,
    ms100,
    ms128,
    ms160,
    ms256,
    ms320,
    ms480,
    ms512,
    ms640,
    ms1024,
    ms1280,
    ms2560,
    ms5120,
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<time_to_trigger_opts> time_to_trigger_e;

// CellsToAddModList ::= SEQUENCE (SIZE (1..32)) OF CellsToAddMod
using cells_to_add_mod_list_l = dyn_array<cells_to_add_mod_s>;

// EUTRA-BlackCell ::= SEQUENCE
struct eutra_black_cell_s {
  uint8_t           cell_idx_eutra = 1;
  eutra_pci_range_s pci_range;

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
      bool                        use_white_cell_list = false;
    };
    struct event_a4_s_ {
      meas_trigger_quant_c a4_thres;
      bool                 report_on_leave = false;
      uint8_t              hysteresis      = 0;
      time_to_trigger_e    time_to_trigger;
      bool                 use_white_cell_list = false;
    };
    struct event_a5_s_ {
      meas_trigger_quant_c a5_thres1;
      meas_trigger_quant_c a5_thres2;
      bool                 report_on_leave = false;
      uint8_t              hysteresis      = 0;
      time_to_trigger_e    time_to_trigger;
      bool                 use_white_cell_list = false;
    };
    struct event_a6_s_ {
      meas_trigger_quant_offset_c a6_offset;
      bool                        report_on_leave = false;
      uint8_t                     hysteresis      = 0;
      time_to_trigger_e           time_to_trigger;
      bool                        use_white_cell_list = false;
    };
    struct types_opts {
      enum options { event_a1, event_a2, event_a3, event_a4, event_a5, event_a6, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts, true> types;

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
    event_a1_s_& set_event_a1();
    event_a2_s_& set_event_a2();
    event_a3_s_& set_event_a3();
    event_a4_s_& set_event_a4();
    event_a5_s_& set_event_a5();
    event_a6_s_& set_event_a6();

  private:
    types                                                                                         type_;
    choice_buffer_t<event_a1_s_, event_a2_s_, event_a3_s_, event_a4_s_, event_a5_s_, event_a6_s_> c;

    void destroy_();
  };
  struct report_amount_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<report_amount_opts> report_amount_e_;

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
    struct types_opts {
      enum options { event_b1, event_b2, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts, true> types;

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
    event_b1_s_& set_event_b1();
    event_b2_s_& set_event_b2();

  private:
    types                                     type_;
    choice_buffer_t<event_b1_s_, event_b2_s_> c;

    void destroy_();
  };
  struct report_amount_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<report_amount_opts> report_amount_e_;

  // member variables
  bool                ext = false;
  event_id_c_         event_id;
  nr_rs_type_e        rs_type;
  report_interv_e     report_interv;
  report_amount_e_    report_amount;
  meas_report_quant_s report_quant;
  uint8_t             max_report_cells = 1;
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

// PCI-List ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..1007)
using pci_list_l = bounded_array<uint16_t, 32>;

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
  typedef enumerated<report_amount_opts> report_amount_e_;

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
  bool                use_white_cell_list         = false;
  // ...

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
  typedef enumerated<report_amount_opts> report_amount_e_;

  // member variables
  bool                ext = false;
  report_interv_e     report_interv;
  report_amount_e_    report_amount;
  meas_report_quant_s report_quant;
  uint8_t             max_report_cells = 1;
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

// SSB-MTC2 ::= SEQUENCE
struct ssb_mtc2_s {
  using pci_list_l_ = dyn_array<uint16_t>;
  struct periodicity_opts {
    enum options { sf5, sf10, sf20, sf40, sf80, spare3, spare2, spare1, nulltype } value;
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

// MRDC-SecondaryCellGroupConfig ::= SEQUENCE
struct mrdc_secondary_cell_group_cfg_s {
  struct mrdc_secondary_cell_group_c_ {
    struct types_opts {
      enum options { nr_scg, eutra_scg, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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

// MeasObjectEUTRA ::= SEQUENCE
struct meas_obj_eutra_s {
  using cells_to_add_mod_list_eutran_l_       = dyn_array<eutra_cell_s>;
  using black_cells_to_add_mod_list_eutran_l_ = dyn_array<eutra_black_cell_s>;

  // member variables
  bool                                  ext                          = false;
  bool                                  eutra_q_offset_range_present = false;
  uint32_t                              carrier_freq                 = 0;
  eutra_allowed_meas_bw_e               allowed_meas_bw;
  eutra_cell_idx_list_l                 cells_to_rem_list_eutran;
  cells_to_add_mod_list_eutran_l_       cells_to_add_mod_list_eutran;
  eutra_cell_idx_list_l                 black_cells_to_rem_list_eutran;
  black_cells_to_add_mod_list_eutran_l_ black_cells_to_add_mod_list_eutran;
  bool                                  eutra_presence_ant_port1 = false;
  eutra_q_offset_range_e                eutra_q_offset_range;
  bool                                  wideband_rsrq_meas = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectNR ::= SEQUENCE
struct meas_obj_nr_s {
  using black_cells_to_add_mod_list_l_ = dyn_array<pci_range_elem_s>;
  using white_cells_to_add_mod_list_l_ = dyn_array<pci_range_elem_s>;
  struct meas_cycle_scell_opts {
    enum options { sf160, sf256, sf320, sf512, sf640, sf1024, sf1280, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<meas_cycle_scell_opts> meas_cycle_scell_e_;

  // member variables
  bool                           ext                                        = false;
  bool                           ssb_freq_present                           = false;
  bool                           ssb_subcarrier_spacing_present             = false;
  bool                           smtc1_present                              = false;
  bool                           smtc2_present                              = false;
  bool                           ref_freq_csi_rs_present                    = false;
  bool                           abs_thresh_ss_blocks_consolidation_present = false;
  bool                           abs_thresh_csi_rs_consolidation_present    = false;
  bool                           nrof_ss_blocks_to_average_present          = false;
  bool                           nrof_csi_rs_res_to_average_present         = false;
  uint32_t                       ssb_freq                                   = 0;
  subcarrier_spacing_e           ssb_subcarrier_spacing;
  ssb_mtc_s                      smtc1;
  ssb_mtc2_s                     smtc2;
  uint32_t                       ref_freq_csi_rs = 0;
  ref_sig_cfg_s                  ref_sig_cfg;
  thres_nr_s                     abs_thresh_ss_blocks_consolidation;
  thres_nr_s                     abs_thresh_csi_rs_consolidation;
  uint8_t                        nrof_ss_blocks_to_average  = 2;
  uint8_t                        nrof_csi_rs_res_to_average = 2;
  uint8_t                        quant_cfg_idx              = 1;
  q_offset_range_list_s          offset_mo;
  pci_list_l                     cells_to_rem_list;
  cells_to_add_mod_list_l        cells_to_add_mod_list;
  pci_range_idx_list_l           black_cells_to_rem_list;
  black_cells_to_add_mod_list_l_ black_cells_to_add_mod_list;
  pci_range_idx_list_l           white_cells_to_rem_list;
  white_cells_to_add_mod_list_l_ white_cells_to_add_mod_list;
  // ...
  // group 0
  bool                freq_band_ind_nr_present = false;
  bool                meas_cycle_scell_present = false;
  uint16_t            freq_band_ind_nr         = 1;
  meas_cycle_scell_e_ meas_cycle_scell;

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
  typedef enumerated<overheat_ind_prohibit_timer_opts> overheat_ind_prohibit_timer_e_;

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

// QuantityConfigRS ::= SEQUENCE
struct quant_cfg_rs_s {
  filt_cfg_s ssb_filt_cfg;
  filt_cfg_s csi_rs_filt_cfg;

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
    typedef enumerated<types_opts, true, 1> types;

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
      enum options { periodical, event_triggered, /*...*/ report_cgi, report_sftd, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts, true, 2> types;

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
    periodical_report_cfg_s& set_periodical();
    event_trigger_cfg_s&     set_event_triggered();
    report_cgi_s&            set_report_cgi();
    report_sftd_nr_s&        set_report_sftd();

  private:
    types                                                                                         type_;
    choice_buffer_t<event_trigger_cfg_s, periodical_report_cfg_s, report_cgi_s, report_sftd_nr_s> c;

    void destroy_();
  };

  // member variables
  report_type_c_ report_type;

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
  typedef enumerated<mgl_opts> mgl_e_;
  struct mgrp_opts {
    enum options { ms20, ms40, ms80, ms160, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<mgrp_opts> mgrp_e_;
  struct mgta_opts {
    enum options { ms0, ms0dot25, ms0dot5, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<mgta_opts> mgta_e_;
  struct ref_serv_cell_ind_opts {
    enum options { pcell, pscell, mcg_fr2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ref_serv_cell_ind_opts> ref_serv_cell_ind_e_;

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
typedef enumerated<meas_gap_sharing_scheme_opts> meas_gap_sharing_scheme_e;

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

// MeasObjectToAddMod ::= SEQUENCE
struct meas_obj_to_add_mod_s {
  struct meas_obj_c_ {
    struct types_opts {
      enum options { meas_obj_nr, /*...*/ meas_obj_eutra, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts, true, 1> types;

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
    meas_obj_nr_s&    set_meas_obj_nr();
    meas_obj_eutra_s& set_meas_obj_eutra();

  private:
    types                                            type_;
    choice_buffer_t<meas_obj_eutra_s, meas_obj_nr_s> c;

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

// RAT-Type ::= ENUMERATED
struct rat_type_opts {
  enum options { nr, eutra_nr, eutra, spare1, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<rat_type_opts, true> rat_type_e;

// RRCReconfiguration-v1560-IEs ::= SEQUENCE
struct rrc_recfg_v1560_ies_s {
  bool                                             mrdc_secondary_cell_group_cfg_present = false;
  bool                                             sk_counter_present                    = false;
  bool                                             non_crit_ext_present                  = false;
  setup_release_c<mrdc_secondary_cell_group_cfg_s> mrdc_secondary_cell_group_cfg;
  dyn_octstring                                    radio_bearer_cfg2;
  uint32_t                                         sk_counter = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportConfigToAddMod ::= SEQUENCE
struct report_cfg_to_add_mod_s {
  struct report_cfg_c_ {
    struct types_opts {
      enum options { report_cfg_nr, /*...*/ report_cfg_inter_rat, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts, true, 1> types;

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
    report_cfg_nr_s&        set_report_cfg_nr();
    report_cfg_inter_rat_s& set_report_cfg_inter_rat();

  private:
    types                                                    type_;
    choice_buffer_t<report_cfg_inter_rat_s, report_cfg_nr_s> c;

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

// FreqPriorityListEUTRA ::= SEQUENCE (SIZE (1..8)) OF FreqPriorityEUTRA
using freq_prio_list_eutra_l = dyn_array<freq_prio_eutra_s>;

// FreqPriorityListNR ::= SEQUENCE (SIZE (1..8)) OF FreqPriorityNR
using freq_prio_list_nr_l = dyn_array<freq_prio_nr_s>;

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
  bool                       ext             = false;
  bool                       gap_fr2_present = false;
  setup_release_c<gap_cfg_s> gap_fr2;
  // ...
  // group 0
  copy_ptr<setup_release_c<gap_cfg_s> > gap_fr1;
  copy_ptr<setup_release_c<gap_cfg_s> > gap_ue;

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
  copy_ptr<setup_release_c<meas_gap_sharing_scheme_e> > gap_sharing_fr1;
  copy_ptr<setup_release_c<meas_gap_sharing_scheme_e> > gap_sharing_ue;

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
      typedef enumerated<delay_budget_report_prohibit_timer_opts> delay_budget_report_prohibit_timer_e_;

      // member variables
      delay_budget_report_prohibit_timer_e_ delay_budget_report_prohibit_timer;
    };
    struct types_opts {
      enum options { release, setup, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
typedef enumerated<periodic_rnau_timer_value_opts> periodic_rnau_timer_value_e;

// QuantityConfig ::= SEQUENCE
struct quant_cfg_s {
  using quant_cfg_nr_list_l_ = dyn_array<quant_cfg_nr_s>;

  // member variables
  bool                 ext = false;
  quant_cfg_nr_list_l_ quant_cfg_nr_list;
  // ...
  // group 0
  copy_ptr<filt_cfg_s> quant_cfg_eutra;

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
  typedef enumerated<types_opts, true> types;

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

// RedirectedCarrierInfo-EUTRA ::= SEQUENCE
struct redirected_carrier_info_eutra_s {
  struct cn_type_opts {
    enum options { epc, five_gc, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<cn_type_opts> cn_type_e_;

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

// CellReselectionPriorities ::= SEQUENCE
struct cell_resel_priorities_s {
  struct t320_opts {
    enum options { min5, min10, min20, min30, min60, min120, min180, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<t320_opts> t320_e_;

  // member variables
  bool                   ext          = false;
  bool                   t320_present = false;
  freq_prio_list_eutra_l freq_prio_list_eutra;
  freq_prio_list_nr_l    freq_prio_list_nr;
  t320_e_                t320;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-CountMSB-InfoList ::= SEQUENCE (SIZE (1..29)) OF DRB-CountMSB-Info
using drb_count_msb_info_list_l = dyn_array<drb_count_msb_info_s>;

// MeasConfig ::= SEQUENCE
struct meas_cfg_s {
  struct s_measure_cfg_c_ {
    struct types_opts {
      enum options { ssb_rsrp, csi_rsrp, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// RRCRelease-v1540-IEs ::= SEQUENCE
struct rrc_release_v1540_ies_s {
  bool    wait_time_present    = false;
  bool    non_crit_ext_present = false;
  uint8_t wait_time            = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResume-v1560-IEs ::= SEQUENCE
struct rrc_resume_v1560_ies_s {
  bool          sk_counter_present   = false;
  bool          non_crit_ext_present = false;
  dyn_octstring radio_bearer_cfg2;
  uint32_t      sk_counter = 0;

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
  typedef enumerated<types_opts, true> types;

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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRAT-RequestList ::= SEQUENCE (SIZE (1..8)) OF UE-CapabilityRAT-Request
using ue_cap_rat_request_list_l = dyn_array<ue_cap_rat_request_s>;

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

// DLInformationTransfer-IEs ::= SEQUENCE
struct dl_info_transfer_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring ded_nas_msg;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityFromNRCommand-IEs ::= SEQUENCE
struct mob_from_nr_cmd_ies_s {
  struct target_rat_type_opts {
    enum options { eutra, spare3, spare2, spare1, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<target_rat_type_opts, true> target_rat_type_e_;

  // member variables
  bool               non_crit_ext_present = false;
  target_rat_type_e_ target_rat_type;
  dyn_octstring      target_rat_msg_container;
  dyn_octstring      nas_security_param_from_nr;
  dyn_octstring      late_non_crit_ext;

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
  bool          non_crit_ext_present    = false;
  uint8_t       next_hop_chaining_count = 0;
  dyn_octstring late_non_crit_ext;

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
    typedef enumerated<depriorit_type_opts> depriorit_type_e_;
    struct depriorit_timer_opts {
      enum options { min5, min10, min15, min30, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<depriorit_timer_opts> depriorit_timer_e_;

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

// CounterCheck ::= SEQUENCE
struct counter_check_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { counter_check, crit_exts_future, nulltype } value;

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

// DLInformationTransfer ::= SEQUENCE
struct dl_info_transfer_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { dl_info_transfer, crit_exts_future, nulltype } value;

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

// MobilityFromNRCommand ::= SEQUENCE
struct mob_from_nr_cmd_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { mob_from_nr_cmd, crit_exts_future, nulltype } value;

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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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
    rrc_recfg_s&         set_rrc_recfg();
    rrc_resume_s&        set_rrc_resume();
    rrc_release_s&       set_rrc_release();
    rrc_reest_s&         set_rrc_reest();
    security_mode_cmd_s& set_security_mode_cmd();
    dl_info_transfer_s&  set_dl_info_transfer();
    ue_cap_enquiry_s&    set_ue_cap_enquiry();
    counter_check_s&     set_counter_check();
    mob_from_nr_cmd_s&   set_mob_from_nr_cmd();
    void                 set_spare7();
    void                 set_spare6();
    void                 set_spare5();
    void                 set_spare4();
    void                 set_spare3();
    void                 set_spare2();
    void                 set_spare1();

  private:
    types type_;
    choice_buffer_t<counter_check_s,
                    dl_info_transfer_s,
                    mob_from_nr_cmd_s,
                    rrc_recfg_s,
                    rrc_reest_s,
                    rrc_release_s,
                    rrc_resume_s,
                    security_mode_cmd_s,
                    ue_cap_enquiry_s>
        c;

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

// PagingUE-Identity ::= CHOICE
struct paging_ue_id_c {
  struct types_opts {
    enum options { ng_minus5_g_s_tmsi, full_i_rnti, /*...*/ nulltype } value;
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
  fixed_bitstring<48>& ng_minus5_g_s_tmsi()
  {
    assert_choice_type(types::ng_minus5_g_s_tmsi, type_, "PagingUE-Identity");
    return c.get<fixed_bitstring<48> >();
  }
  fixed_bitstring<40>& full_i_rnti()
  {
    assert_choice_type(types::full_i_rnti, type_, "PagingUE-Identity");
    return c.get<fixed_bitstring<40> >();
  }
  const fixed_bitstring<48>& ng_minus5_g_s_tmsi() const
  {
    assert_choice_type(types::ng_minus5_g_s_tmsi, type_, "PagingUE-Identity");
    return c.get<fixed_bitstring<48> >();
  }
  const fixed_bitstring<40>& full_i_rnti() const
  {
    assert_choice_type(types::full_i_rnti, type_, "PagingUE-Identity");
    return c.get<fixed_bitstring<40> >();
  }
  fixed_bitstring<48>& set_ng_minus5_g_s_tmsi();
  fixed_bitstring<40>& set_full_i_rnti();

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<48> > c;

  void destroy_();
};

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

// PagingRecordList ::= SEQUENCE (SIZE (1..32)) OF PagingRecord
using paging_record_list_l = dyn_array<paging_record_s>;

// Paging ::= SEQUENCE
struct paging_s {
  bool                 non_crit_ext_present = false;
  paging_record_list_l paging_record_list;
  dyn_octstring        late_non_crit_ext;

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

// EstablishmentCause ::= ENUMERATED
struct establishment_cause_opts {
  enum options {
    emergency,
    high_prio_access,
    mt_access,
    mo_sig,
    mo_data,
    mo_voice_call,
    mo_video_call,
    mo_sms,
    mps_prio_access,
    mcs_prio_access,
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
typedef enumerated<establishment_cause_opts> establishment_cause_e;

// InitialUE-Identity ::= CHOICE
struct init_ue_id_c {
  struct types_opts {
    enum options { ng_minus5_g_s_tmsi_part1, random_value, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  init_ue_id_c() = default;
  init_ue_id_c(const init_ue_id_c& other);
  init_ue_id_c& operator=(const init_ue_id_c& other);
  ~init_ue_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<39>& ng_minus5_g_s_tmsi_part1()
  {
    assert_choice_type(types::ng_minus5_g_s_tmsi_part1, type_, "InitialUE-Identity");
    return c.get<fixed_bitstring<39> >();
  }
  fixed_bitstring<39>& random_value()
  {
    assert_choice_type(types::random_value, type_, "InitialUE-Identity");
    return c.get<fixed_bitstring<39> >();
  }
  const fixed_bitstring<39>& ng_minus5_g_s_tmsi_part1() const
  {
    assert_choice_type(types::ng_minus5_g_s_tmsi_part1, type_, "InitialUE-Identity");
    return c.get<fixed_bitstring<39> >();
  }
  const fixed_bitstring<39>& random_value() const
  {
    assert_choice_type(types::random_value, type_, "InitialUE-Identity");
    return c.get<fixed_bitstring<39> >();
  }
  fixed_bitstring<39>& set_ng_minus5_g_s_tmsi_part1();
  fixed_bitstring<39>& set_random_value();

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<39> > c;

  void destroy_();
};

// ReestabUE-Identity ::= SEQUENCE
struct reestab_ue_id_s {
  uint32_t            c_rnti = 0;
  uint16_t            pci    = 0;
  fixed_bitstring<16> short_mac_i;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReestablishmentCause ::= ENUMERATED
struct reest_cause_opts {
  enum options { recfg_fail, ho_fail, other_fail, spare1, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<reest_cause_opts> reest_cause_e;

// ResumeCause ::= ENUMERATED
struct resume_cause_opts {
  enum options {
    emergency,
    high_prio_access,
    mt_access,
    mo_sig,
    mo_data,
    mo_voice_call,
    mo_video_call,
    mo_sms,
    rna_upd,
    mps_prio_access,
    mcs_prio_access,
    spare1,
    spare2,
    spare3,
    spare4,
    spare5,
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<resume_cause_opts> resume_cause_e;

// RRCReestablishmentRequest-IEs ::= SEQUENCE
struct rrc_reest_request_ies_s {
  reestab_ue_id_s    ue_id;
  reest_cause_e      reest_cause;
  fixed_bitstring<1> spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeRequest-IEs ::= SEQUENCE
struct rrc_resume_request_ies_s {
  fixed_bitstring<24> resume_id;
  fixed_bitstring<16> resume_mac_i;
  resume_cause_e      resume_cause;
  fixed_bitstring<1>  spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetupRequest-IEs ::= SEQUENCE
struct rrc_setup_request_ies_s {
  init_ue_id_c          ue_id;
  establishment_cause_e establishment_cause;
  fixed_bitstring<1>    spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSystemInfoRequest-IEs ::= SEQUENCE
struct rrc_sys_info_request_ies_s {
  fixed_bitstring<32> requested_si_list;
  fixed_bitstring<12> spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReestablishmentRequest ::= SEQUENCE
struct rrc_reest_request_s {
  rrc_reest_request_ies_s rrc_reest_request;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeRequest ::= SEQUENCE
struct rrc_resume_request_s {
  rrc_resume_request_ies_s rrc_resume_request;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetupRequest ::= SEQUENCE
struct rrc_setup_request_s {
  rrc_setup_request_ies_s rrc_setup_request;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSystemInfoRequest ::= SEQUENCE
struct rrc_sys_info_request_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_sys_info_request, crit_exts_future, nulltype } value;

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
    rrc_sys_info_request_ies_s& rrc_sys_info_request()
    {
      assert_choice_type(types::rrc_sys_info_request, type_, "criticalExtensions");
      return c;
    }
    const rrc_sys_info_request_ies_s& rrc_sys_info_request() const
    {
      assert_choice_type(types::rrc_sys_info_request, type_, "criticalExtensions");
      return c;
    }
    rrc_sys_info_request_ies_s& set_rrc_sys_info_request();
    void                        set_crit_exts_future();

  private:
    types                      type_;
    rrc_sys_info_request_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-CCCH-MessageType ::= CHOICE
struct ul_ccch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { rrc_setup_request, rrc_resume_request, rrc_reest_request, rrc_sys_info_request, nulltype } value;

      const char* to_string() const;
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
    rrc_setup_request_s& rrc_setup_request()
    {
      assert_choice_type(types::rrc_setup_request, type_, "c1");
      return c.get<rrc_setup_request_s>();
    }
    rrc_resume_request_s& rrc_resume_request()
    {
      assert_choice_type(types::rrc_resume_request, type_, "c1");
      return c.get<rrc_resume_request_s>();
    }
    rrc_reest_request_s& rrc_reest_request()
    {
      assert_choice_type(types::rrc_reest_request, type_, "c1");
      return c.get<rrc_reest_request_s>();
    }
    rrc_sys_info_request_s& rrc_sys_info_request()
    {
      assert_choice_type(types::rrc_sys_info_request, type_, "c1");
      return c.get<rrc_sys_info_request_s>();
    }
    const rrc_setup_request_s& rrc_setup_request() const
    {
      assert_choice_type(types::rrc_setup_request, type_, "c1");
      return c.get<rrc_setup_request_s>();
    }
    const rrc_resume_request_s& rrc_resume_request() const
    {
      assert_choice_type(types::rrc_resume_request, type_, "c1");
      return c.get<rrc_resume_request_s>();
    }
    const rrc_reest_request_s& rrc_reest_request() const
    {
      assert_choice_type(types::rrc_reest_request, type_, "c1");
      return c.get<rrc_reest_request_s>();
    }
    const rrc_sys_info_request_s& rrc_sys_info_request() const
    {
      assert_choice_type(types::rrc_sys_info_request, type_, "c1");
      return c.get<rrc_sys_info_request_s>();
    }
    rrc_setup_request_s&    set_rrc_setup_request();
    rrc_resume_request_s&   set_rrc_resume_request();
    rrc_reest_request_s&    set_rrc_reest_request();
    rrc_sys_info_request_s& set_rrc_sys_info_request();

  private:
    types                                                                                                   type_;
    choice_buffer_t<rrc_reest_request_s, rrc_resume_request_s, rrc_setup_request_s, rrc_sys_info_request_s> c;

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
  ul_ccch_msg_type_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "UL-CCCH-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "UL-CCCH-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// UL-CCCH-Message ::= SEQUENCE
struct ul_ccch_msg_s {
  ul_ccch_msg_type_c msg;

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

// MeasQuantityResults ::= SEQUENCE
struct meas_quant_results_s {
  bool    rsrp_present = false;
  bool    rsrq_present = false;
  bool    sinr_present = false;
  uint8_t rsrp         = 0;
  uint8_t rsrq         = 0;
  uint8_t sinr         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-Identity-EUTRA-5GC ::= CHOICE
struct plmn_id_eutra_minus5_gc_c {
  struct types_opts {
    enum options { plmn_id_eutra_minus5_gc, plmn_idx, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  plmn_id_eutra_minus5_gc_c() = default;
  plmn_id_eutra_minus5_gc_c(const plmn_id_eutra_minus5_gc_c& other);
  plmn_id_eutra_minus5_gc_c& operator=(const plmn_id_eutra_minus5_gc_c& other);
  ~plmn_id_eutra_minus5_gc_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  plmn_id_s& plmn_id_eutra_minus5_gc()
  {
    assert_choice_type(types::plmn_id_eutra_minus5_gc, type_, "PLMN-Identity-EUTRA-5GC");
    return c.get<plmn_id_s>();
  }
  uint8_t& plmn_idx()
  {
    assert_choice_type(types::plmn_idx, type_, "PLMN-Identity-EUTRA-5GC");
    return c.get<uint8_t>();
  }
  const plmn_id_s& plmn_id_eutra_minus5_gc() const
  {
    assert_choice_type(types::plmn_id_eutra_minus5_gc, type_, "PLMN-Identity-EUTRA-5GC");
    return c.get<plmn_id_s>();
  }
  const uint8_t& plmn_idx() const
  {
    assert_choice_type(types::plmn_idx, type_, "PLMN-Identity-EUTRA-5GC");
    return c.get<uint8_t>();
  }
  plmn_id_s& set_plmn_id_eutra_minus5_gc();
  uint8_t&   set_plmn_idx();

private:
  types                      type_;
  choice_buffer_t<plmn_id_s> c;

  void destroy_();
};

// CellIdentity-EUTRA-5GC ::= CHOICE
struct cell_id_eutra_minus5_gc_c {
  struct types_opts {
    enum options { cell_id_eutra, cell_id_idx, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  cell_id_eutra_minus5_gc_c() = default;
  cell_id_eutra_minus5_gc_c(const cell_id_eutra_minus5_gc_c& other);
  cell_id_eutra_minus5_gc_c& operator=(const cell_id_eutra_minus5_gc_c& other);
  ~cell_id_eutra_minus5_gc_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<28>& cell_id_eutra()
  {
    assert_choice_type(types::cell_id_eutra, type_, "CellIdentity-EUTRA-5GC");
    return c.get<fixed_bitstring<28> >();
  }
  uint8_t& cell_id_idx()
  {
    assert_choice_type(types::cell_id_idx, type_, "CellIdentity-EUTRA-5GC");
    return c.get<uint8_t>();
  }
  const fixed_bitstring<28>& cell_id_eutra() const
  {
    assert_choice_type(types::cell_id_eutra, type_, "CellIdentity-EUTRA-5GC");
    return c.get<fixed_bitstring<28> >();
  }
  const uint8_t& cell_id_idx() const
  {
    assert_choice_type(types::cell_id_idx, type_, "CellIdentity-EUTRA-5GC");
    return c.get<uint8_t>();
  }
  fixed_bitstring<28>& set_cell_id_eutra();
  uint8_t&             set_cell_id_idx();

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<28> > c;

  void destroy_();
};

// MultiFrequencyBandListNR ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..1024)
using multi_freq_band_list_nr_l = bounded_array<uint16_t, 8>;

// PLMN-IdentityList-EUTRA-5GC ::= SEQUENCE (SIZE (1..12)) OF PLMN-Identity-EUTRA-5GC
using plmn_id_list_eutra_minus5_gc_l = dyn_array<plmn_id_eutra_minus5_gc_c>;

// PLMN-IdentityList-EUTRA-EPC ::= SEQUENCE (SIZE (1..12)) OF PLMN-Identity
using plmn_id_list_eutra_epc_l = dyn_array<plmn_id_s>;

// ResultsPerCSI-RS-Index ::= SEQUENCE
struct results_per_csi_rs_idx_s {
  bool                 csi_rs_results_present = false;
  uint8_t              csi_rs_idx             = 0;
  meas_quant_results_s csi_rs_results;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultsPerSSB-Index ::= SEQUENCE
struct results_per_ssb_idx_s {
  bool                 ssb_results_present = false;
  uint8_t              ssb_idx             = 0;
  meas_quant_results_s ssb_results;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CGI-InfoNR ::= SEQUENCE
struct cgi_info_nr_s {
  struct no_sib1_s_ {
    uint8_t          ssb_subcarrier_offset = 0;
    pdcch_cfg_sib1_s pdcch_cfg_sib1;
  };

  // member variables
  bool                      ext             = false;
  bool                      no_sib1_present = false;
  plmn_id_info_list_l       plmn_id_info_list;
  multi_freq_band_list_nr_l freq_band_list;
  no_sib1_s_                no_sib1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellAccessRelatedInfo-EUTRA-5GC ::= SEQUENCE
struct cell_access_related_info_eutra_minus5_gc_s {
  bool                           ranac_minus5gc_present = false;
  plmn_id_list_eutra_minus5_gc_l plmn_id_list_eutra_minus5gc;
  fixed_bitstring<24>            tac_eutra_minus5gc;
  uint16_t                       ranac_minus5gc = 0;
  cell_id_eutra_minus5_gc_c      cell_id_eutra_minus5gc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellAccessRelatedInfo-EUTRA-EPC ::= SEQUENCE
struct cell_access_related_info_eutra_epc_s {
  plmn_id_list_eutra_epc_l plmn_id_list_eutra_epc;
  fixed_bitstring<16>      tac_eutra_epc;
  fixed_bitstring<28>      cell_id_eutra_epc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MultiBandInfoListEUTRA ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..256)
using multi_band_info_list_eutra_l = bounded_array<uint16_t, 8>;

// ResultsPerCSI-RS-IndexList ::= SEQUENCE (SIZE (1..64)) OF ResultsPerCSI-RS-Index
using results_per_csi_rs_idx_list_l = dyn_array<results_per_csi_rs_idx_s>;

// ResultsPerSSB-IndexList ::= SEQUENCE (SIZE (1..64)) OF ResultsPerSSB-Index
using results_per_ssb_idx_list_l = dyn_array<results_per_ssb_idx_s>;

// CGI-InfoEUTRA ::= SEQUENCE
struct cgi_info_eutra_s {
  struct cgi_info_epc_s_ {
    using cgi_info_epc_list_l_ = dyn_array<cell_access_related_info_eutra_epc_s>;

    // member variables
    cell_access_related_info_eutra_epc_s cgi_info_epc_legacy;
    cgi_info_epc_list_l_                 cgi_info_epc_list;
  };
  using cgi_info_minus5_gc_l_ = dyn_array<cell_access_related_info_eutra_minus5_gc_s>;

  // member variables
  bool                         cgi_info_epc_present       = false;
  bool                         freq_band_ind_prio_present = false;
  cgi_info_epc_s_              cgi_info_epc;
  cgi_info_minus5_gc_l_        cgi_info_minus5_gc;
  uint16_t                     freq_band_ind = 1;
  multi_band_info_list_eutra_l multi_band_info_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasQuantityResultsEUTRA ::= SEQUENCE
struct meas_quant_results_eutra_s {
  bool    rsrp_present = false;
  bool    rsrq_present = false;
  bool    sinr_present = false;
  uint8_t rsrp         = 0;
  uint8_t rsrq         = 0;
  uint8_t sinr         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultNR ::= SEQUENCE
struct meas_result_nr_s {
  struct meas_result_s_ {
    struct cell_results_s_ {
      bool                 results_ssb_cell_present    = false;
      bool                 results_csi_rs_cell_present = false;
      meas_quant_results_s results_ssb_cell;
      meas_quant_results_s results_csi_rs_cell;
    };
    struct rs_idx_results_s_ {
      results_per_ssb_idx_list_l    results_ssb_idxes;
      results_per_csi_rs_idx_list_l results_csi_rs_idxes;
    };

    // member variables
    bool              rs_idx_results_present = false;
    cell_results_s_   cell_results;
    rs_idx_results_s_ rs_idx_results;
  };

  // member variables
  bool           ext         = false;
  bool           pci_present = false;
  uint16_t       pci         = 0;
  meas_result_s_ meas_result;
  // ...
  // group 0
  copy_ptr<cgi_info_nr_s> cgi_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultEUTRA ::= SEQUENCE
struct meas_result_eutra_s {
  bool                       ext              = false;
  bool                       cgi_info_present = false;
  uint16_t                   eutra_pci        = 0;
  meas_quant_results_eutra_s meas_result;
  cgi_info_eutra_s           cgi_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultListNR ::= SEQUENCE (SIZE (1..8)) OF MeasResultNR
using meas_result_list_nr_l = dyn_array<meas_result_nr_s>;

// UplinkTxDirectCurrentBWP ::= SEQUENCE
struct ul_tx_direct_current_bwp_s {
  uint8_t  bwp_id                     = 0;
  bool     shift7dot5k_hz             = false;
  uint16_t tx_direct_current_location = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-RSTD-Info ::= SEQUENCE
struct eutra_rstd_info_s {
  bool     ext             = false;
  uint32_t carrier_freq    = 0;
  uint8_t  meas_prs_offset = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResult2EUTRA ::= SEQUENCE
struct meas_result2_eutra_s {
  bool                ext                                 = false;
  bool                meas_result_serving_cell_present    = false;
  bool                meas_result_best_neigh_cell_present = false;
  uint32_t            carrier_freq                        = 0;
  meas_result_eutra_s meas_result_serving_cell;
  meas_result_eutra_s meas_result_best_neigh_cell;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResult2NR ::= SEQUENCE
struct meas_result2_nr_s {
  bool                  ext                              = false;
  bool                  ssb_freq_present                 = false;
  bool                  ref_freq_csi_rs_present          = false;
  bool                  meas_result_serving_cell_present = false;
  uint32_t              ssb_freq                         = 0;
  uint32_t              ref_freq_csi_rs                  = 0;
  meas_result_nr_s      meas_result_serving_cell;
  meas_result_list_nr_l meas_result_neigh_cell_list_nr;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultCellSFTD-NR ::= SEQUENCE
struct meas_result_cell_sftd_nr_s {
  bool     rsrp_result_present          = false;
  uint16_t pci                          = 0;
  uint16_t sfn_offset_result            = 0;
  int16_t  frame_boundary_offset_result = -30720;
  uint8_t  rsrp_result                  = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultServMO ::= SEQUENCE
struct meas_result_serv_mo_s {
  bool             ext                                 = false;
  bool             meas_result_best_neigh_cell_present = false;
  uint8_t          serv_cell_id                        = 0;
  meas_result_nr_s meas_result_serving_cell;
  meas_result_nr_s meas_result_best_neigh_cell;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReducedAggregatedBandwidth ::= ENUMERATED
struct reduced_aggregated_bw_opts {
  enum options {
    mhz0,
    mhz10,
    mhz20,
    mhz30,
    mhz40,
    mhz50,
    mhz60,
    mhz80,
    mhz100,
    mhz200,
    mhz300,
    mhz400,
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<reduced_aggregated_bw_opts> reduced_aggregated_bw_e;

// UplinkTxDirectCurrentCell ::= SEQUENCE
struct ul_tx_direct_current_cell_s {
  using ul_direct_current_bwp_l_     = dyn_array<ul_tx_direct_current_bwp_s>;
  using ul_direct_current_bwp_sul_l_ = dyn_array<ul_tx_direct_current_bwp_s>;

  // member variables
  bool                     ext           = false;
  uint8_t                  serv_cell_idx = 0;
  ul_direct_current_bwp_l_ ul_direct_current_bwp;
  // ...
  // group 0
  copy_ptr<ul_direct_current_bwp_sul_l_> ul_direct_current_bwp_sul;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-CountInfo ::= SEQUENCE
struct drb_count_info_s {
  uint8_t  drb_id   = 1;
  uint64_t count_ul = 0;
  uint64_t count_dl = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-RSTD-InfoList ::= SEQUENCE (SIZE (1..3)) OF EUTRA-RSTD-Info
using eutra_rstd_info_list_l = dyn_array<eutra_rstd_info_s>;

// MeasResultCellListSFTD-NR ::= SEQUENCE (SIZE (1..3)) OF MeasResultCellSFTD-NR
using meas_result_cell_list_sftd_nr_l = dyn_array<meas_result_cell_sftd_nr_s>;

// MeasResultFreqList ::= SEQUENCE (SIZE (1..8)) OF MeasResult2NR
using meas_result_freq_list_l = dyn_array<meas_result2_nr_s>;

// MeasResultFreqListFailMRDC ::= SEQUENCE (SIZE (1..8)) OF MeasResult2EUTRA
using meas_result_freq_list_fail_mrdc_l = dyn_array<meas_result2_eutra_s>;

// MeasResultListEUTRA ::= SEQUENCE (SIZE (1..8)) OF MeasResultEUTRA
using meas_result_list_eutra_l = dyn_array<meas_result_eutra_s>;

// MeasResultSFTD-EUTRA ::= SEQUENCE
struct meas_result_sftd_eutra_s {
  bool     rsrp_result_present          = false;
  uint16_t eutra_pci                    = 0;
  uint16_t sfn_offset_result            = 0;
  int16_t  frame_boundary_offset_result = -30720;
  uint8_t  rsrp_result                  = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultServFreqListEUTRA-SCG ::= SEQUENCE (SIZE (1..32)) OF MeasResult2EUTRA
using meas_result_serv_freq_list_eutra_scg_l = dyn_array<meas_result2_eutra_s>;

// MeasResultServFreqListNR-SCG ::= SEQUENCE (SIZE (1..32)) OF MeasResult2NR
using meas_result_serv_freq_list_nr_scg_l = dyn_array<meas_result2_nr_s>;

// MeasResultServMOList ::= SEQUENCE (SIZE (1..32)) OF MeasResultServMO
using meas_result_serv_mo_list_l = dyn_array<meas_result_serv_mo_s>;

// OverheatingAssistance ::= SEQUENCE
struct overheat_assist_s {
  struct reduced_max_ccs_s_ {
    uint8_t reduced_ccs_dl = 0;
    uint8_t reduced_ccs_ul = 0;
  };
  struct reduced_max_bw_fr1_s_ {
    reduced_aggregated_bw_e reduced_bw_fr1_dl;
    reduced_aggregated_bw_e reduced_bw_fr1_ul;
  };
  struct reduced_max_bw_fr2_s_ {
    reduced_aggregated_bw_e reduced_bw_fr2_dl;
    reduced_aggregated_bw_e reduced_bw_fr2_ul;
  };
  struct reduced_max_mimo_layers_fr1_s_ {
    mimo_layers_dl_e reduced_mimo_layers_fr1_dl;
    mimo_layers_ul_e reduced_mimo_layers_fr1_ul;
  };
  struct reduced_max_mimo_layers_fr2_s_ {
    mimo_layers_dl_e reduced_mimo_layers_fr2_dl;
    mimo_layers_ul_e reduced_mimo_layers_fr2_ul;
  };

  // member variables
  bool                           reduced_max_ccs_present             = false;
  bool                           reduced_max_bw_fr1_present          = false;
  bool                           reduced_max_bw_fr2_present          = false;
  bool                           reduced_max_mimo_layers_fr1_present = false;
  bool                           reduced_max_mimo_layers_fr2_present = false;
  reduced_max_ccs_s_             reduced_max_ccs;
  reduced_max_bw_fr1_s_          reduced_max_bw_fr1;
  reduced_max_bw_fr2_s_          reduced_max_bw_fr2;
  reduced_max_mimo_layers_fr1_s_ reduced_max_mimo_layers_fr1;
  reduced_max_mimo_layers_fr2_s_ reduced_max_mimo_layers_fr2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationComplete-v1560-IEs ::= SEQUENCE
struct rrc_recfg_complete_v1560_ies_s {
  struct scg_resp_c_ {
    struct types_opts {
      enum options { nr_scg_resp, eutra_scg_resp, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    scg_resp_c_() = default;
    scg_resp_c_(const scg_resp_c_& other);
    scg_resp_c_& operator=(const scg_resp_c_& other);
    ~scg_resp_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dyn_octstring& nr_scg_resp()
    {
      assert_choice_type(types::nr_scg_resp, type_, "scg-Response");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& eutra_scg_resp()
    {
      assert_choice_type(types::eutra_scg_resp, type_, "scg-Response");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& nr_scg_resp() const
    {
      assert_choice_type(types::nr_scg_resp, type_, "scg-Response");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& eutra_scg_resp() const
    {
      assert_choice_type(types::eutra_scg_resp, type_, "scg-Response");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& set_nr_scg_resp();
    dyn_octstring& set_eutra_scg_resp();

  private:
    types                          type_;
    choice_buffer_t<dyn_octstring> c;

    void destroy_();
  };

  // member variables
  bool        scg_resp_present     = false;
  bool        non_crit_ext_present = false;
  scg_resp_c_ scg_resp;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRAT-Container ::= SEQUENCE
struct ue_cap_rat_container_s {
  rat_type_e    rat_type;
  dyn_octstring ue_cap_rat_container;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkTxDirectCurrentList ::= SEQUENCE (SIZE (1..32)) OF UplinkTxDirectCurrentCell
using ul_tx_direct_current_list_l = dyn_array<ul_tx_direct_current_cell_s>;

// DRB-CountInfoList ::= SEQUENCE (SIZE (0..29)) OF DRB-CountInfo
using drb_count_info_list_l = dyn_array<drb_count_info_s>;

// DelayBudgetReport ::= CHOICE
struct delay_budget_report_c {
  struct type1_opts {
    enum options {
      ms_minus1280,
      ms_minus640,
      ms_minus320,
      ms_minus160,
      ms_minus80,
      ms_minus60,
      ms_minus40,
      ms_minus20,
      ms0,
      ms20,
      ms40,
      ms60,
      ms80,
      ms160,
      ms320,
      ms640,
      ms1280,
      nulltype
    } value;
    typedef int16_t number_type;

    const char* to_string() const;
    int16_t     to_number() const;
  };
  typedef enumerated<type1_opts> type1_e_;
  struct types_opts {
    enum options { type1, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  types       type() const { return types::type1; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  type1_e_&       type1() { return c; }
  const type1_e_& type1() const { return c; }

private:
  type1_e_ c;
};

// FailureInfoRLC-Bearer ::= SEQUENCE
struct fail_info_rlc_bearer_s {
  struct fail_type_opts {
    enum options { rlc_fail, spare3, spare2, spare1, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<fail_type_opts> fail_type_e_;

  // member variables
  uint8_t      cell_group_id = 0;
  uint8_t      lc_ch_id      = 1;
  fail_type_e_ fail_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailureReportSCG ::= SEQUENCE
struct fail_report_scg_s {
  struct fail_type_opts {
    enum options {
      t310_expiry,
      random_access_problem,
      rlc_max_num_retx,
      synch_recfg_fail_scg,
      scg_recfg_fail,
      srb3_integrity_fail,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<fail_type_opts> fail_type_e_;

  // member variables
  bool                    ext = false;
  fail_type_e_            fail_type;
  meas_result_freq_list_l meas_result_freq_list;
  dyn_octstring           meas_result_scg_fail;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailureReportSCG-EUTRA ::= SEQUENCE
struct fail_report_scg_eutra_s {
  struct fail_type_opts {
    enum options {
      t313_expiry,
      random_access_problem,
      rlc_max_num_retx,
      scg_change_fail,
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
  typedef enumerated<fail_type_opts> fail_type_e_;

  // member variables
  bool                              ext = false;
  fail_type_e_                      fail_type;
  meas_result_freq_list_fail_mrdc_l meas_result_freq_list_mrdc;
  dyn_octstring                     meas_result_scg_fail_mrdc;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationMeasurementInfo ::= CHOICE
struct location_meas_info_c {
  struct types_opts {
    enum options { eutra_rstd, /*...*/ eutra_fine_timing_detection, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true, 1> types;

  // choice methods
  location_meas_info_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_rstd_info_list_l& eutra_rstd()
  {
    assert_choice_type(types::eutra_rstd, type_, "LocationMeasurementInfo");
    return c;
  }
  const eutra_rstd_info_list_l& eutra_rstd() const
  {
    assert_choice_type(types::eutra_rstd, type_, "LocationMeasurementInfo");
    return c;
  }
  eutra_rstd_info_list_l& set_eutra_rstd();
  void                    set_eutra_fine_timing_detection();

private:
  types                  type_;
  eutra_rstd_info_list_l c;
};

// MeasResults ::= SEQUENCE
struct meas_results_s {
  struct meas_result_neigh_cells_c_ {
    struct types_opts {
      enum options { meas_result_list_nr, /*...*/ meas_result_list_eutra, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts, true, 1> types;

    // choice methods
    meas_result_neigh_cells_c_() = default;
    meas_result_neigh_cells_c_(const meas_result_neigh_cells_c_& other);
    meas_result_neigh_cells_c_& operator=(const meas_result_neigh_cells_c_& other);
    ~meas_result_neigh_cells_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    meas_result_list_nr_l& meas_result_list_nr()
    {
      assert_choice_type(types::meas_result_list_nr, type_, "measResultNeighCells");
      return c.get<meas_result_list_nr_l>();
    }
    meas_result_list_eutra_l& meas_result_list_eutra()
    {
      assert_choice_type(types::meas_result_list_eutra, type_, "measResultNeighCells");
      return c.get<meas_result_list_eutra_l>();
    }
    const meas_result_list_nr_l& meas_result_list_nr() const
    {
      assert_choice_type(types::meas_result_list_nr, type_, "measResultNeighCells");
      return c.get<meas_result_list_nr_l>();
    }
    const meas_result_list_eutra_l& meas_result_list_eutra() const
    {
      assert_choice_type(types::meas_result_list_eutra, type_, "measResultNeighCells");
      return c.get<meas_result_list_eutra_l>();
    }
    meas_result_list_nr_l&    set_meas_result_list_nr();
    meas_result_list_eutra_l& set_meas_result_list_eutra();

  private:
    types                                                            type_;
    choice_buffer_t<meas_result_list_eutra_l, meas_result_list_nr_l> c;

    void destroy_();
  };

  // member variables
  bool                       ext                             = false;
  bool                       meas_result_neigh_cells_present = false;
  uint8_t                    meas_id                         = 1;
  meas_result_serv_mo_list_l meas_result_serving_mo_list;
  meas_result_neigh_cells_c_ meas_result_neigh_cells;
  // ...
  // group 0
  copy_ptr<meas_result_serv_freq_list_eutra_scg_l> meas_result_serv_freq_list_eutra_scg;
  copy_ptr<meas_result_serv_freq_list_nr_scg_l>    meas_result_serv_freq_list_nr_scg;
  copy_ptr<meas_result_sftd_eutra_s>               meas_result_sftd_eutra;
  copy_ptr<meas_result_cell_sftd_nr_s>             meas_result_sftd_nr;
  // group 1
  copy_ptr<meas_result_cell_list_sftd_nr_l> meas_result_cell_list_sftd_nr;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationComplete-v1530-IEs ::= SEQUENCE
struct rrc_recfg_complete_v1530_ies_s {
  bool                           non_crit_ext_present = false;
  ul_tx_direct_current_list_l    ul_tx_direct_current_list;
  rrc_recfg_complete_v1560_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RegisteredAMF ::= SEQUENCE
struct registered_amf_s {
  bool                plmn_id_present = false;
  plmn_id_s           plmn_id;
  fixed_bitstring<24> amf_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// S-NSSAI ::= CHOICE
struct s_nssai_c {
  struct types_opts {
    enum options { sst, sst_sd, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  s_nssai_c() = default;
  s_nssai_c(const s_nssai_c& other);
  s_nssai_c& operator=(const s_nssai_c& other);
  ~s_nssai_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<8>& sst()
  {
    assert_choice_type(types::sst, type_, "S-NSSAI");
    return c.get<fixed_bitstring<8> >();
  }
  fixed_bitstring<32>& sst_sd()
  {
    assert_choice_type(types::sst_sd, type_, "S-NSSAI");
    return c.get<fixed_bitstring<32> >();
  }
  const fixed_bitstring<8>& sst() const
  {
    assert_choice_type(types::sst, type_, "S-NSSAI");
    return c.get<fixed_bitstring<8> >();
  }
  const fixed_bitstring<32>& sst_sd() const
  {
    assert_choice_type(types::sst_sd, type_, "S-NSSAI");
    return c.get<fixed_bitstring<32> >();
  }
  fixed_bitstring<8>&  set_sst();
  fixed_bitstring<32>& set_sst_sd();

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<32> > c;

  void destroy_();
};

// SCGFailureInformation-v1590-IEs ::= SEQUENCE
struct scg_fail_info_v1590_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCGFailureInformationEUTRA-v1590-IEs ::= SEQUENCE
struct scg_fail_info_eutra_v1590_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRAT-ContainerList ::= SEQUENCE (SIZE (0..8)) OF UE-CapabilityRAT-Container
using ue_cap_rat_container_list_l = dyn_array<ue_cap_rat_container_s>;

// UEAssistanceInformation-v1540-IEs ::= SEQUENCE
struct ueassist_info_v1540_ies_s {
  bool              overheat_assist_present = false;
  bool              non_crit_ext_present    = false;
  overheat_assist_s overheat_assist;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CounterCheckResponse-IEs ::= SEQUENCE
struct counter_check_resp_ies_s {
  bool                  non_crit_ext_present = false;
  drb_count_info_list_l drb_count_info_list;
  dyn_octstring         late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailureInformation-IEs ::= SEQUENCE
struct fail_info_ies_s {
  bool                   fail_info_rlc_bearer_present = false;
  bool                   non_crit_ext_present         = false;
  fail_info_rlc_bearer_s fail_info_rlc_bearer;
  dyn_octstring          late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationMeasurementIndication-IEs ::= SEQUENCE
struct location_meas_ind_ies_s {
  bool                                  non_crit_ext_present = false;
  setup_release_c<location_meas_info_c> meas_ind;
  dyn_octstring                         late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementReport-IEs ::= SEQUENCE
struct meas_report_ies_s {
  bool           non_crit_ext_present = false;
  meas_results_s meas_results;
  dyn_octstring  late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationComplete-IEs ::= SEQUENCE
struct rrc_recfg_complete_ies_s {
  bool                           non_crit_ext_present = false;
  dyn_octstring                  late_non_crit_ext;
  rrc_recfg_complete_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReestablishmentComplete-IEs ::= SEQUENCE
struct rrc_reest_complete_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeComplete-IEs ::= SEQUENCE
struct rrc_resume_complete_ies_s {
  bool                        sel_plmn_id_present  = false;
  bool                        non_crit_ext_present = false;
  dyn_octstring               ded_nas_msg;
  uint8_t                     sel_plmn_id = 1;
  ul_tx_direct_current_list_l ul_tx_direct_current_list;
  dyn_octstring               late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetupComplete-IEs ::= SEQUENCE
struct rrc_setup_complete_ies_s {
  struct guami_type_opts {
    enum options { native, mapped, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<guami_type_opts> guami_type_e_;
  using s_nssai_list_l_ = dyn_array<s_nssai_c>;
  struct ng_minus5_g_s_tmsi_value_c_ {
    struct types_opts {
      enum options { ng_minus5_g_s_tmsi, ng_minus5_g_s_tmsi_part2, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ng_minus5_g_s_tmsi_value_c_() = default;
    ng_minus5_g_s_tmsi_value_c_(const ng_minus5_g_s_tmsi_value_c_& other);
    ng_minus5_g_s_tmsi_value_c_& operator=(const ng_minus5_g_s_tmsi_value_c_& other);
    ~ng_minus5_g_s_tmsi_value_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<48>& ng_minus5_g_s_tmsi()
    {
      assert_choice_type(types::ng_minus5_g_s_tmsi, type_, "ng-5G-S-TMSI-Value");
      return c.get<fixed_bitstring<48> >();
    }
    fixed_bitstring<9>& ng_minus5_g_s_tmsi_part2()
    {
      assert_choice_type(types::ng_minus5_g_s_tmsi_part2, type_, "ng-5G-S-TMSI-Value");
      return c.get<fixed_bitstring<9> >();
    }
    const fixed_bitstring<48>& ng_minus5_g_s_tmsi() const
    {
      assert_choice_type(types::ng_minus5_g_s_tmsi, type_, "ng-5G-S-TMSI-Value");
      return c.get<fixed_bitstring<48> >();
    }
    const fixed_bitstring<9>& ng_minus5_g_s_tmsi_part2() const
    {
      assert_choice_type(types::ng_minus5_g_s_tmsi_part2, type_, "ng-5G-S-TMSI-Value");
      return c.get<fixed_bitstring<9> >();
    }
    fixed_bitstring<48>& set_ng_minus5_g_s_tmsi();
    fixed_bitstring<9>&  set_ng_minus5_g_s_tmsi_part2();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<48> > c;

    void destroy_();
  };

  // member variables
  bool                        registered_amf_present           = false;
  bool                        guami_type_present               = false;
  bool                        ng_minus5_g_s_tmsi_value_present = false;
  bool                        non_crit_ext_present             = false;
  uint8_t                     sel_plmn_id                      = 1;
  registered_amf_s            registered_amf;
  guami_type_e_               guami_type;
  s_nssai_list_l_             s_nssai_list;
  dyn_octstring               ded_nas_msg;
  ng_minus5_g_s_tmsi_value_c_ ng_minus5_g_s_tmsi_value;
  dyn_octstring               late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCGFailureInformation-IEs ::= SEQUENCE
struct scg_fail_info_ies_s {
  bool                      fail_report_scg_present = false;
  bool                      non_crit_ext_present    = false;
  fail_report_scg_s         fail_report_scg;
  scg_fail_info_v1590_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCGFailureInformationEUTRA-IEs ::= SEQUENCE
struct scg_fail_info_eutra_ies_s {
  bool                            fail_report_scg_eutra_present = false;
  bool                            non_crit_ext_present          = false;
  fail_report_scg_eutra_s         fail_report_scg_eutra;
  scg_fail_info_eutra_v1590_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeComplete-IEs ::= SEQUENCE
struct security_mode_complete_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeFailure-IEs ::= SEQUENCE
struct security_mode_fail_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEAssistanceInformation-IEs ::= SEQUENCE
struct ueassist_info_ies_s {
  bool                      delay_budget_report_present = false;
  bool                      non_crit_ext_present        = false;
  delay_budget_report_c     delay_budget_report;
  dyn_octstring             late_non_crit_ext;
  ueassist_info_v1540_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityInformation-IEs ::= SEQUENCE
struct ue_cap_info_ies_s {
  bool                        ue_cap_rat_container_list_present = false;
  bool                        non_crit_ext_present              = false;
  ue_cap_rat_container_list_l ue_cap_rat_container_list;
  dyn_octstring               late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULInformationTransfer-IEs ::= SEQUENCE
struct ul_info_transfer_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring ded_nas_msg;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULInformationTransferMRDC-IEs ::= SEQUENCE
struct ul_info_transfer_mrdc_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring ul_dcch_msg_nr;
  dyn_octstring ul_dcch_msg_eutra;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CounterCheckResponse ::= SEQUENCE
struct counter_check_resp_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { counter_check_resp, crit_exts_future, nulltype } value;

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
    counter_check_resp_ies_s& counter_check_resp()
    {
      assert_choice_type(types::counter_check_resp, type_, "criticalExtensions");
      return c;
    }
    const counter_check_resp_ies_s& counter_check_resp() const
    {
      assert_choice_type(types::counter_check_resp, type_, "criticalExtensions");
      return c;
    }
    counter_check_resp_ies_s& set_counter_check_resp();
    void                      set_crit_exts_future();

  private:
    types                    type_;
    counter_check_resp_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailureInformation ::= SEQUENCE
struct fail_info_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { fail_info, crit_exts_future, nulltype } value;

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
    fail_info_ies_s& fail_info()
    {
      assert_choice_type(types::fail_info, type_, "criticalExtensions");
      return c;
    }
    const fail_info_ies_s& fail_info() const
    {
      assert_choice_type(types::fail_info, type_, "criticalExtensions");
      return c;
    }
    fail_info_ies_s& set_fail_info();
    void             set_crit_exts_future();

  private:
    types           type_;
    fail_info_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationMeasurementIndication ::= SEQUENCE
struct location_meas_ind_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { location_meas_ind, crit_exts_future, nulltype } value;

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
    location_meas_ind_ies_s& location_meas_ind()
    {
      assert_choice_type(types::location_meas_ind, type_, "criticalExtensions");
      return c;
    }
    const location_meas_ind_ies_s& location_meas_ind() const
    {
      assert_choice_type(types::location_meas_ind, type_, "criticalExtensions");
      return c;
    }
    location_meas_ind_ies_s& set_location_meas_ind();
    void                     set_crit_exts_future();

  private:
    types                   type_;
    location_meas_ind_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementReport ::= SEQUENCE
struct meas_report_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { meas_report, crit_exts_future, nulltype } value;

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
    meas_report_ies_s& meas_report()
    {
      assert_choice_type(types::meas_report, type_, "criticalExtensions");
      return c;
    }
    const meas_report_ies_s& meas_report() const
    {
      assert_choice_type(types::meas_report, type_, "criticalExtensions");
      return c;
    }
    meas_report_ies_s& set_meas_report();
    void               set_crit_exts_future();

  private:
    types             type_;
    meas_report_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationComplete ::= SEQUENCE
struct rrc_recfg_complete_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_recfg_complete, crit_exts_future, nulltype } value;

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
    rrc_recfg_complete_ies_s& rrc_recfg_complete()
    {
      assert_choice_type(types::rrc_recfg_complete, type_, "criticalExtensions");
      return c;
    }
    const rrc_recfg_complete_ies_s& rrc_recfg_complete() const
    {
      assert_choice_type(types::rrc_recfg_complete, type_, "criticalExtensions");
      return c;
    }
    rrc_recfg_complete_ies_s& set_rrc_recfg_complete();
    void                      set_crit_exts_future();

  private:
    types                    type_;
    rrc_recfg_complete_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReestablishmentComplete ::= SEQUENCE
struct rrc_reest_complete_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_reest_complete, crit_exts_future, nulltype } value;

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
    rrc_reest_complete_ies_s& rrc_reest_complete()
    {
      assert_choice_type(types::rrc_reest_complete, type_, "criticalExtensions");
      return c;
    }
    const rrc_reest_complete_ies_s& rrc_reest_complete() const
    {
      assert_choice_type(types::rrc_reest_complete, type_, "criticalExtensions");
      return c;
    }
    rrc_reest_complete_ies_s& set_rrc_reest_complete();
    void                      set_crit_exts_future();

  private:
    types                    type_;
    rrc_reest_complete_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeComplete ::= SEQUENCE
struct rrc_resume_complete_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_resume_complete, crit_exts_future, nulltype } value;

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
    rrc_resume_complete_ies_s& rrc_resume_complete()
    {
      assert_choice_type(types::rrc_resume_complete, type_, "criticalExtensions");
      return c;
    }
    const rrc_resume_complete_ies_s& rrc_resume_complete() const
    {
      assert_choice_type(types::rrc_resume_complete, type_, "criticalExtensions");
      return c;
    }
    rrc_resume_complete_ies_s& set_rrc_resume_complete();
    void                       set_crit_exts_future();

  private:
    types                     type_;
    rrc_resume_complete_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetupComplete ::= SEQUENCE
struct rrc_setup_complete_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_setup_complete, crit_exts_future, nulltype } value;

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
    rrc_setup_complete_ies_s& rrc_setup_complete()
    {
      assert_choice_type(types::rrc_setup_complete, type_, "criticalExtensions");
      return c;
    }
    const rrc_setup_complete_ies_s& rrc_setup_complete() const
    {
      assert_choice_type(types::rrc_setup_complete, type_, "criticalExtensions");
      return c;
    }
    rrc_setup_complete_ies_s& set_rrc_setup_complete();
    void                      set_crit_exts_future();

  private:
    types                    type_;
    rrc_setup_complete_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCGFailureInformation ::= SEQUENCE
struct scg_fail_info_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { scg_fail_info, crit_exts_future, nulltype } value;

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
    scg_fail_info_ies_s& scg_fail_info()
    {
      assert_choice_type(types::scg_fail_info, type_, "criticalExtensions");
      return c;
    }
    const scg_fail_info_ies_s& scg_fail_info() const
    {
      assert_choice_type(types::scg_fail_info, type_, "criticalExtensions");
      return c;
    }
    scg_fail_info_ies_s& set_scg_fail_info();
    void                 set_crit_exts_future();

  private:
    types               type_;
    scg_fail_info_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCGFailureInformationEUTRA ::= SEQUENCE
struct scg_fail_info_eutra_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { scg_fail_info_eutra, crit_exts_future, nulltype } value;

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
    scg_fail_info_eutra_ies_s& scg_fail_info_eutra()
    {
      assert_choice_type(types::scg_fail_info_eutra, type_, "criticalExtensions");
      return c;
    }
    const scg_fail_info_eutra_ies_s& scg_fail_info_eutra() const
    {
      assert_choice_type(types::scg_fail_info_eutra, type_, "criticalExtensions");
      return c;
    }
    scg_fail_info_eutra_ies_s& set_scg_fail_info_eutra();
    void                       set_crit_exts_future();

  private:
    types                     type_;
    scg_fail_info_eutra_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeComplete ::= SEQUENCE
struct security_mode_complete_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { security_mode_complete, crit_exts_future, nulltype } value;

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
    security_mode_complete_ies_s& security_mode_complete()
    {
      assert_choice_type(types::security_mode_complete, type_, "criticalExtensions");
      return c;
    }
    const security_mode_complete_ies_s& security_mode_complete() const
    {
      assert_choice_type(types::security_mode_complete, type_, "criticalExtensions");
      return c;
    }
    security_mode_complete_ies_s& set_security_mode_complete();
    void                          set_crit_exts_future();

  private:
    types                        type_;
    security_mode_complete_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityModeFailure ::= SEQUENCE
struct security_mode_fail_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { security_mode_fail, crit_exts_future, nulltype } value;

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
    security_mode_fail_ies_s& security_mode_fail()
    {
      assert_choice_type(types::security_mode_fail, type_, "criticalExtensions");
      return c;
    }
    const security_mode_fail_ies_s& security_mode_fail() const
    {
      assert_choice_type(types::security_mode_fail, type_, "criticalExtensions");
      return c;
    }
    security_mode_fail_ies_s& set_security_mode_fail();
    void                      set_crit_exts_future();

  private:
    types                    type_;
    security_mode_fail_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEAssistanceInformation ::= SEQUENCE
struct ueassist_info_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_assist_info, crit_exts_future, nulltype } value;

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
    ueassist_info_ies_s& ue_assist_info()
    {
      assert_choice_type(types::ue_assist_info, type_, "criticalExtensions");
      return c;
    }
    const ueassist_info_ies_s& ue_assist_info() const
    {
      assert_choice_type(types::ue_assist_info, type_, "criticalExtensions");
      return c;
    }
    ueassist_info_ies_s& set_ue_assist_info();
    void                 set_crit_exts_future();

  private:
    types               type_;
    ueassist_info_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityInformation ::= SEQUENCE
struct ue_cap_info_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_cap_info, crit_exts_future, nulltype } value;

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
    ue_cap_info_ies_s& ue_cap_info()
    {
      assert_choice_type(types::ue_cap_info, type_, "criticalExtensions");
      return c;
    }
    const ue_cap_info_ies_s& ue_cap_info() const
    {
      assert_choice_type(types::ue_cap_info, type_, "criticalExtensions");
      return c;
    }
    ue_cap_info_ies_s& set_ue_cap_info();
    void               set_crit_exts_future();

  private:
    types             type_;
    ue_cap_info_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULInformationTransfer ::= SEQUENCE
struct ul_info_transfer_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ul_info_transfer, crit_exts_future, nulltype } value;

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
    ul_info_transfer_ies_s& ul_info_transfer()
    {
      assert_choice_type(types::ul_info_transfer, type_, "criticalExtensions");
      return c;
    }
    const ul_info_transfer_ies_s& ul_info_transfer() const
    {
      assert_choice_type(types::ul_info_transfer, type_, "criticalExtensions");
      return c;
    }
    ul_info_transfer_ies_s& set_ul_info_transfer();
    void                    set_crit_exts_future();

  private:
    types                  type_;
    ul_info_transfer_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULInformationTransferMRDC ::= SEQUENCE
struct ul_info_transfer_mrdc_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ul_info_transfer_mrdc, spare3, spare2, spare1, nulltype } value;

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
      ul_info_transfer_mrdc_ies_s& ul_info_transfer_mrdc()
      {
        assert_choice_type(types::ul_info_transfer_mrdc, type_, "c1");
        return c;
      }
      const ul_info_transfer_mrdc_ies_s& ul_info_transfer_mrdc() const
      {
        assert_choice_type(types::ul_info_transfer_mrdc, type_, "c1");
        return c;
      }
      ul_info_transfer_mrdc_ies_s& set_ul_info_transfer_mrdc();
      void                         set_spare3();
      void                         set_spare2();
      void                         set_spare1();

    private:
      types                       type_;
      ul_info_transfer_mrdc_ies_s c;
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

// UL-DCCH-MessageType ::= CHOICE
struct ul_dcch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options {
        meas_report,
        rrc_recfg_complete,
        rrc_setup_complete,
        rrc_reest_complete,
        rrc_resume_complete,
        security_mode_complete,
        security_mode_fail,
        ul_info_transfer,
        location_meas_ind,
        ue_cap_info,
        counter_check_resp,
        ue_assist_info,
        fail_info,
        ul_info_transfer_mrdc,
        scg_fail_info,
        scg_fail_info_eutra,
        nulltype
      } value;

      const char* to_string() const;
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
    meas_report_s& meas_report()
    {
      assert_choice_type(types::meas_report, type_, "c1");
      return c.get<meas_report_s>();
    }
    rrc_recfg_complete_s& rrc_recfg_complete()
    {
      assert_choice_type(types::rrc_recfg_complete, type_, "c1");
      return c.get<rrc_recfg_complete_s>();
    }
    rrc_setup_complete_s& rrc_setup_complete()
    {
      assert_choice_type(types::rrc_setup_complete, type_, "c1");
      return c.get<rrc_setup_complete_s>();
    }
    rrc_reest_complete_s& rrc_reest_complete()
    {
      assert_choice_type(types::rrc_reest_complete, type_, "c1");
      return c.get<rrc_reest_complete_s>();
    }
    rrc_resume_complete_s& rrc_resume_complete()
    {
      assert_choice_type(types::rrc_resume_complete, type_, "c1");
      return c.get<rrc_resume_complete_s>();
    }
    security_mode_complete_s& security_mode_complete()
    {
      assert_choice_type(types::security_mode_complete, type_, "c1");
      return c.get<security_mode_complete_s>();
    }
    security_mode_fail_s& security_mode_fail()
    {
      assert_choice_type(types::security_mode_fail, type_, "c1");
      return c.get<security_mode_fail_s>();
    }
    ul_info_transfer_s& ul_info_transfer()
    {
      assert_choice_type(types::ul_info_transfer, type_, "c1");
      return c.get<ul_info_transfer_s>();
    }
    location_meas_ind_s& location_meas_ind()
    {
      assert_choice_type(types::location_meas_ind, type_, "c1");
      return c.get<location_meas_ind_s>();
    }
    ue_cap_info_s& ue_cap_info()
    {
      assert_choice_type(types::ue_cap_info, type_, "c1");
      return c.get<ue_cap_info_s>();
    }
    counter_check_resp_s& counter_check_resp()
    {
      assert_choice_type(types::counter_check_resp, type_, "c1");
      return c.get<counter_check_resp_s>();
    }
    ueassist_info_s& ue_assist_info()
    {
      assert_choice_type(types::ue_assist_info, type_, "c1");
      return c.get<ueassist_info_s>();
    }
    fail_info_s& fail_info()
    {
      assert_choice_type(types::fail_info, type_, "c1");
      return c.get<fail_info_s>();
    }
    ul_info_transfer_mrdc_s& ul_info_transfer_mrdc()
    {
      assert_choice_type(types::ul_info_transfer_mrdc, type_, "c1");
      return c.get<ul_info_transfer_mrdc_s>();
    }
    scg_fail_info_s& scg_fail_info()
    {
      assert_choice_type(types::scg_fail_info, type_, "c1");
      return c.get<scg_fail_info_s>();
    }
    scg_fail_info_eutra_s& scg_fail_info_eutra()
    {
      assert_choice_type(types::scg_fail_info_eutra, type_, "c1");
      return c.get<scg_fail_info_eutra_s>();
    }
    const meas_report_s& meas_report() const
    {
      assert_choice_type(types::meas_report, type_, "c1");
      return c.get<meas_report_s>();
    }
    const rrc_recfg_complete_s& rrc_recfg_complete() const
    {
      assert_choice_type(types::rrc_recfg_complete, type_, "c1");
      return c.get<rrc_recfg_complete_s>();
    }
    const rrc_setup_complete_s& rrc_setup_complete() const
    {
      assert_choice_type(types::rrc_setup_complete, type_, "c1");
      return c.get<rrc_setup_complete_s>();
    }
    const rrc_reest_complete_s& rrc_reest_complete() const
    {
      assert_choice_type(types::rrc_reest_complete, type_, "c1");
      return c.get<rrc_reest_complete_s>();
    }
    const rrc_resume_complete_s& rrc_resume_complete() const
    {
      assert_choice_type(types::rrc_resume_complete, type_, "c1");
      return c.get<rrc_resume_complete_s>();
    }
    const security_mode_complete_s& security_mode_complete() const
    {
      assert_choice_type(types::security_mode_complete, type_, "c1");
      return c.get<security_mode_complete_s>();
    }
    const security_mode_fail_s& security_mode_fail() const
    {
      assert_choice_type(types::security_mode_fail, type_, "c1");
      return c.get<security_mode_fail_s>();
    }
    const ul_info_transfer_s& ul_info_transfer() const
    {
      assert_choice_type(types::ul_info_transfer, type_, "c1");
      return c.get<ul_info_transfer_s>();
    }
    const location_meas_ind_s& location_meas_ind() const
    {
      assert_choice_type(types::location_meas_ind, type_, "c1");
      return c.get<location_meas_ind_s>();
    }
    const ue_cap_info_s& ue_cap_info() const
    {
      assert_choice_type(types::ue_cap_info, type_, "c1");
      return c.get<ue_cap_info_s>();
    }
    const counter_check_resp_s& counter_check_resp() const
    {
      assert_choice_type(types::counter_check_resp, type_, "c1");
      return c.get<counter_check_resp_s>();
    }
    const ueassist_info_s& ue_assist_info() const
    {
      assert_choice_type(types::ue_assist_info, type_, "c1");
      return c.get<ueassist_info_s>();
    }
    const fail_info_s& fail_info() const
    {
      assert_choice_type(types::fail_info, type_, "c1");
      return c.get<fail_info_s>();
    }
    const ul_info_transfer_mrdc_s& ul_info_transfer_mrdc() const
    {
      assert_choice_type(types::ul_info_transfer_mrdc, type_, "c1");
      return c.get<ul_info_transfer_mrdc_s>();
    }
    const scg_fail_info_s& scg_fail_info() const
    {
      assert_choice_type(types::scg_fail_info, type_, "c1");
      return c.get<scg_fail_info_s>();
    }
    const scg_fail_info_eutra_s& scg_fail_info_eutra() const
    {
      assert_choice_type(types::scg_fail_info_eutra, type_, "c1");
      return c.get<scg_fail_info_eutra_s>();
    }
    meas_report_s&            set_meas_report();
    rrc_recfg_complete_s&     set_rrc_recfg_complete();
    rrc_setup_complete_s&     set_rrc_setup_complete();
    rrc_reest_complete_s&     set_rrc_reest_complete();
    rrc_resume_complete_s&    set_rrc_resume_complete();
    security_mode_complete_s& set_security_mode_complete();
    security_mode_fail_s&     set_security_mode_fail();
    ul_info_transfer_s&       set_ul_info_transfer();
    location_meas_ind_s&      set_location_meas_ind();
    ue_cap_info_s&            set_ue_cap_info();
    counter_check_resp_s&     set_counter_check_resp();
    ueassist_info_s&          set_ue_assist_info();
    fail_info_s&              set_fail_info();
    ul_info_transfer_mrdc_s&  set_ul_info_transfer_mrdc();
    scg_fail_info_s&          set_scg_fail_info();
    scg_fail_info_eutra_s&    set_scg_fail_info_eutra();

  private:
    types type_;
    choice_buffer_t<counter_check_resp_s,
                    fail_info_s,
                    location_meas_ind_s,
                    meas_report_s,
                    rrc_recfg_complete_s,
                    rrc_reest_complete_s,
                    rrc_resume_complete_s,
                    rrc_setup_complete_s,
                    scg_fail_info_eutra_s,
                    scg_fail_info_s,
                    security_mode_complete_s,
                    security_mode_fail_s,
                    ue_cap_info_s,
                    ueassist_info_s,
                    ul_info_transfer_mrdc_s,
                    ul_info_transfer_s>
        c;

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
  ul_dcch_msg_type_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "UL-DCCH-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "UL-DCCH-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// UL-DCCH-Message ::= SEQUENCE
struct ul_dcch_msg_s {
  ul_dcch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-ParametersEUTRA-v1570 ::= SEQUENCE
struct ca_params_eutra_v1570_s {
  bool    dl_minus1024_qam_total_weighted_layers_present = false;
  uint8_t dl_minus1024_qam_total_weighted_layers         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1570 ::= SEQUENCE
struct band_combination_v1570_s {
  ca_params_eutra_v1570_s ca_params_eutra_v1570;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRDC-Parameters-v1580 ::= SEQUENCE
struct mrdc_params_v1580_s {
  bool dynamic_pwr_sharing_nedc_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1580 ::= SEQUENCE
struct band_combination_v1580_s {
  mrdc_params_v1580_s mrdc_params_v1580;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRDC-Parameters-v1590 ::= SEQUENCE
struct mrdc_params_v1590_s {
  bool inter_band_contiguous_mrdc_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-v1590 ::= SEQUENCE
struct band_combination_v1590_s {
  bool                     supported_bw_combination_set_intra_endc_present = false;
  bounded_bitstring<1, 32> supported_bw_combination_set_intra_endc;
  mrdc_params_v1590_s      mrdc_params_v1590;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationList-v1570 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1570
using band_combination_list_v1570_l = dyn_array<band_combination_v1570_s>;

// BandCombinationList-v1580 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1580
using band_combination_list_v1580_l = dyn_array<band_combination_v1580_s>;

// BandCombinationList-v1590 ::= SEQUENCE (SIZE (1..65536)) OF BandCombination-v1590
using band_combination_list_v1590_l = dyn_array<band_combination_v1590_s>;

// CFRA-CSIRS-Resource ::= SEQUENCE
struct cfra_csirs_res_s {
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
      using csirs_res_list_l_ = dyn_array<cfra_csirs_res_s>;

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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PollByte ::= ENUMERATED
struct poll_byte_opts {
  enum options {
    kb1,
    kb2,
    kb5,
    kb8,
    kb10,
    kb15,
    kb25,
    kb50,
    kb75,
    kb100,
    kb125,
    kb250,
    kb375,
    kb500,
    kb750,
    kb1000,
    kb1250,
    kb1500,
    kb2000,
    kb3000,
    kb4000,
    kb4500,
    kb5000,
    kb5500,
    kb6000,
    kb6500,
    kb7000,
    kb7500,
    mb8,
    mb9,
    mb10,
    mb11,
    mb12,
    mb13,
    mb14,
    mb15,
    mb16,
    mb17,
    mb18,
    mb20,
    mb25,
    mb30,
    mb40,
    infinity,
    spare20,
    spare19,
    spare18,
    spare17,
    spare16,
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
  typedef int32_t number_type;

  const char* to_string() const;
  int32_t     to_number() const;
};
typedef enumerated<poll_byte_opts> poll_byte_e;

// PollPDU ::= ENUMERATED
struct poll_pdu_opts {
  enum options {
    p4,
    p8,
    p16,
    p32,
    p64,
    p128,
    p256,
    p512,
    p1024,
    p2048,
    p4096,
    p6144,
    p8192,
    p12288,
    p16384,
    p20480,
    p24576,
    p28672,
    p32768,
    p40960,
    p49152,
    p57344,
    p65536,
    infinity,
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
  typedef int32_t number_type;

  const char* to_string() const;
  int32_t     to_number() const;
};
typedef enumerated<poll_pdu_opts> poll_pdu_e;

// SN-FieldLengthAM ::= ENUMERATED
struct sn_field_len_am_opts {
  enum options { size12, size18, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<sn_field_len_am_opts> sn_field_len_am_e;

// SN-FieldLengthUM ::= ENUMERATED
struct sn_field_len_um_opts {
  enum options { size6, size12, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<sn_field_len_um_opts> sn_field_len_um_e;

// T-PollRetransmit ::= ENUMERATED
struct t_poll_retx_opts {
  enum options {
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
    ms2000,
    ms4000,
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
typedef enumerated<t_poll_retx_opts> t_poll_retx_e;

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

// RACH-ConfigDedicated ::= SEQUENCE
struct rach_cfg_ded_s {
  bool                ext                       = false;
  bool                cfra_present              = false;
  bool                ra_prioritization_present = false;
  cfra_s              cfra;
  ra_prioritization_s ra_prioritization;
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
      return c.get<fixed_bitstring<4> >();
    }
    fixed_bitstring<8>& medium_bitmap()
    {
      assert_choice_type(types::medium_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<8> >();
    }
    fixed_bitstring<64>& long_bitmap()
    {
      assert_choice_type(types::long_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<64> >();
    }
    const fixed_bitstring<4>& short_bitmap() const
    {
      assert_choice_type(types::short_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<4> >();
    }
    const fixed_bitstring<8>& medium_bitmap() const
    {
      assert_choice_type(types::medium_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<8> >();
    }
    const fixed_bitstring<64>& long_bitmap() const
    {
      assert_choice_type(types::long_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<64> >();
    }
    fixed_bitstring<4>&  set_short_bitmap();
    fixed_bitstring<8>&  set_medium_bitmap();
    fixed_bitstring<64>& set_long_bitmap();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<64> > c;

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

// BSR-Config ::= SEQUENCE
struct bsr_cfg_s {
  struct periodic_bsr_timer_opts {
    enum options {
      sf1,
      sf5,
      sf10,
      sf16,
      sf20,
      sf32,
      sf40,
      sf64,
      sf80,
      sf128,
      sf160,
      sf320,
      sf640,
      sf1280,
      sf2560,
      infinity,
      nulltype
    } value;
    typedef int16_t number_type;

    const char* to_string() const;
    int16_t     to_number() const;
  };
  typedef enumerated<periodic_bsr_timer_opts> periodic_bsr_timer_e_;
  struct retx_bsr_timer_opts {
    enum options {
      sf10,
      sf20,
      sf40,
      sf80,
      sf160,
      sf320,
      sf640,
      sf1280,
      sf2560,
      sf5120,
      sf10240,
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
  typedef enumerated<retx_bsr_timer_opts> retx_bsr_timer_e_;
  struct lc_ch_sr_delay_timer_opts {
    enum options { sf20, sf40, sf64, sf128, sf512, sf1024, sf2560, spare1, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<lc_ch_sr_delay_timer_opts> lc_ch_sr_delay_timer_e_;

  // member variables
  bool                    ext                          = false;
  bool                    lc_ch_sr_delay_timer_present = false;
  periodic_bsr_timer_e_   periodic_bsr_timer;
  retx_bsr_timer_e_       retx_bsr_timer;
  lc_ch_sr_delay_timer_e_ lc_ch_sr_delay_timer;
  // ...

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

// DataInactivityTimer ::= ENUMERATED
struct data_inactivity_timer_opts {
  enum options { s1, s2, s3, s5, s7, s10, s15, s20, s40, s50, s60, s80, s100, s120, s150, s180, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<data_inactivity_timer_opts> data_inactivity_timer_e;

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
      enum options { ms0p02, ms0p04, ms0p0625, ms0p125, ms0p25, ms0p5, spare2, spare1, nulltype } value;

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

    // member variables
    bool                     ext                                 = false;
    bool                     max_pusch_dur_present               = false;
    bool                     cfgured_grant_type1_allowed_present = false;
    bool                     lc_ch_group_present                 = false;
    bool                     sched_request_id_present            = false;
    uint8_t                  prio                                = 1;
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

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool                  ext                        = false;
  bool                  ul_specific_params_present = false;
  ul_specific_params_s_ ul_specific_params;
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

// MAC-CellGroupConfig ::= SEQUENCE
struct mac_cell_group_cfg_s {
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
  bool                       skip_ul_tx_dynamic = false;
  // ...
  // group 0
  bool                                                csi_mask_present = false;
  bool                                                csi_mask         = false;
  copy_ptr<setup_release_c<data_inactivity_timer_e> > data_inactivity_timer;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhysicalCellGroupConfig ::= SEQUENCE
struct phys_cell_group_cfg_s {
  struct pdsch_harq_ack_codebook_opts {
    enum options { semi_static, dynamic_value, nulltype } value;

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

  // member variables
  bool                                          ext                                     = false;
  bool                                          harq_ack_spatial_bundling_pucch_present = false;
  bool                                          harq_ack_spatial_bundling_pusch_present = false;
  bool                                          p_nr_fr1_present                        = false;
  bool                                          tpc_srs_rnti_present                    = false;
  bool                                          tpc_pucch_rnti_present                  = false;
  bool                                          tpc_pusch_rnti_present                  = false;
  bool                                          sp_csi_rnti_present                     = false;
  bool                                          cs_rnti_present                         = false;
  int8_t                                        p_nr_fr1                                = -30;
  pdsch_harq_ack_codebook_e_                    pdsch_harq_ack_codebook;
  uint32_t                                      tpc_srs_rnti   = 0;
  uint32_t                                      tpc_pucch_rnti = 0;
  uint32_t                                      tpc_pusch_rnti = 0;
  uint32_t                                      sp_csi_rnti    = 0;
  setup_release_c<integer<uint32_t, 0, 65535> > cs_rnti;
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
  copy_ptr<setup_release_c<integer<uint8_t, 1, 15> > > pdcch_blind_detection;

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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SpCellConfig ::= SEQUENCE
struct sp_cell_cfg_s {
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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellGroupConfig ::= SEQUENCE
struct cell_group_cfg_s {
  using rlc_bearer_to_add_mod_list_l_ = dyn_array<rlc_bearer_cfg_s>;
  using rlc_bearer_to_release_list_l_ = bounded_array<uint8_t, 32>;
  using scell_to_add_mod_list_l_      = dyn_array<scell_cfg_s>;
  using scell_to_release_list_l_      = bounded_array<uint8_t, 31>;

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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasAndMobParametersMRDC-XDD-Diff-v1560 ::= SEQUENCE
struct meas_and_mob_params_mrdc_xdd_diff_v1560_s {
  bool sftd_meas_ps_cell_nedc_present = false;

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

// MeasResultCellListSFTD-EUTRA ::= SEQUENCE (SIZE (1..3)) OF MeasResultSFTD-EUTRA
using meas_result_cell_list_sftd_eutra_l = dyn_array<meas_result_sftd_eutra_s>;

// MeasResultList2NR ::= SEQUENCE (SIZE (1..8)) OF MeasResult2NR
using meas_result_list2_nr_l = dyn_array<meas_result2_nr_s>;

// MeasResultSCG-Failure ::= SEQUENCE
struct meas_result_scg_fail_s {
  bool                   ext = false;
  meas_result_list2_nr_l meas_result_per_mo_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NAICS-Capability-Entry ::= SEQUENCE
struct naics_cap_entry_s {
  struct nof_aggregated_prb_opts {
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
  typedef enumerated<nof_aggregated_prb_opts> nof_aggregated_prb_e_;

  // member variables
  bool                  ext                  = false;
  uint8_t               nof_naics_capable_cc = 1;
  nof_aggregated_prb_e_ nof_aggregated_prb;
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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRequestFilterNR-v1540 ::= SEQUENCE
struct ue_cap_request_filt_nr_v1540_s {
  bool srs_switching_time_request_present = false;
  bool non_crit_ext_present               = false;

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

// UE-CapabilityRequestFilterCommon ::= SEQUENCE
struct ue_cap_request_filt_common_s {
  struct mrdc_request_s_ {
    bool omit_en_dc_present    = false;
    bool include_nr_dc_present = false;
    bool include_ne_dc_present = false;
  };

  // member variables
  bool            ext                  = false;
  bool            mrdc_request_present = false;
  mrdc_request_s_ mrdc_request;
  // ...

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
  bool source_scg_cfgured_present = false;

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

// BandCombinationInfoSN ::= SEQUENCE
struct band_combination_info_sn_s {
  uint32_t band_combination_idx   = 1;
  uint8_t  requested_feature_sets = 1;

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

// CG-ConfigInfo-v1590-IEs ::= SEQUENCE
struct cg_cfg_info_v1590_ies_s {
  using serv_frequencies_mn_nr_l_ = bounded_array<uint32_t, 31>;

  // member variables
  bool                      non_crit_ext_present = false;
  serv_frequencies_mn_nr_l_ serv_frequencies_mn_nr;

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
  copy_ptr<setup_release_c<gap_cfg_s> > meas_gap_cfg_fr2;

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

struct cells_triggered_list_item_c_ {
  struct types_opts {
    enum options { pci, pci_eutra, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  cells_triggered_list_item_c_() = default;
  cells_triggered_list_item_c_(const cells_triggered_list_item_c_& other);
  cells_triggered_list_item_c_& operator=(const cells_triggered_list_item_c_& other);
  ~cells_triggered_list_item_c_() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& pci()
  {
    assert_choice_type(types::pci, type_, "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  uint16_t& pci_eutra()
  {
    assert_choice_type(types::pci_eutra, type_, "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  const uint16_t& pci() const
  {
    assert_choice_type(types::pci, type_, "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  const uint16_t& pci_eutra() const
  {
    assert_choice_type(types::pci_eutra, type_, "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  uint16_t& set_pci();
  uint16_t& set_pci_eutra();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// CellsTriggeredList ::= SEQUENCE (SIZE (1..32)) OF CellsTriggeredList-item
using cells_triggered_list_l = dyn_array<cells_triggered_list_item_c_>;

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

// MeasurementTimingConfiguration-v1550-IEs ::= SEQUENCE
struct meas_timing_cfg_v1550_ies_s {
  bool non_crit_ext_present      = false;
  bool camp_on_first_ssb         = false;
  bool ps_cell_only_on_first_ssb = false;

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

// UERadioPagingInformation-IEs ::= SEQUENCE
struct ue_radio_paging_info_ies_s {
  using supported_band_list_nr_for_paging_l_ = dyn_array<uint16_t>;

  // member variables
  bool                                 non_crit_ext_present = false;
  supported_band_list_nr_for_paging_l_ supported_band_list_nr_for_paging;

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

// VarMeasConfig ::= SEQUENCE
struct var_meas_cfg_s {
  struct s_measure_cfg_c_ {
    struct types_opts {
      enum options { ssb_rsrp, csi_rsrp, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
  bool                         quant_cfg_present     = false;
  bool                         s_measure_cfg_present = false;
  meas_id_to_add_mod_list_l    meas_id_list;
  meas_obj_to_add_mod_list_l   meas_obj_list;
  report_cfg_to_add_mod_list_l report_cfg_list;
  quant_cfg_s                  quant_cfg;
  s_measure_cfg_c_             s_measure_cfg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasReport ::= SEQUENCE
struct var_meas_report_s {
  uint8_t                meas_id = 1;
  cells_triggered_list_l cells_triggered_list;
  int64_t                nof_reports_sent = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasReportList ::= SEQUENCE (SIZE (1..64)) OF VarMeasReport
using var_meas_report_list_l = dyn_array<var_meas_report_s>;

// VarResumeMAC-Input ::= SEQUENCE
struct var_resume_mac_input_s {
  uint16_t            source_pci = 0;
  fixed_bitstring<36> target_cell_id;
  uint32_t            source_c_rnti = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarShortMAC-Input ::= SEQUENCE
struct var_short_mac_input_s {
  uint16_t            source_pci = 0;
  fixed_bitstring<36> target_cell_id;
  uint32_t            source_c_rnti = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1

#endif // SRSGNB_ASN1_RRC_NR_RRC_NR_H

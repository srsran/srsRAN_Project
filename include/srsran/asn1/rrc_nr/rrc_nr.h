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

#include "cell_group_config.h"
#include "nr_sidelink_preconf.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

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

// AI-RNTI-r16 ::= INTEGER (0..65535)
using ai_rnti_r16 = integer<uint32_t, 0, 65535>;

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

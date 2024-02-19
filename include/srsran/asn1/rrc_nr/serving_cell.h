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

#include "bwp_cfg.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// NR-NS-PmaxValue ::= SEQUENCE
struct nr_ns_pmax_value_s {
  bool    add_pmax_present  = false;
  int8_t  add_pmax          = -30;
  uint8_t add_spec_emission = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-MBSFN-SubframeConfig ::= SEQUENCE
struct eutra_mbsfn_sf_cfg_s {
  struct radioframe_alloc_period_opts {
    enum options { n1, n2, n4, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using radioframe_alloc_period_e_ = enumerated<radioframe_alloc_period_opts>;
  struct sf_alloc1_c_ {
    struct types_opts {
      enum options { one_frame, four_frames, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    sf_alloc1_c_() = default;
    sf_alloc1_c_(const sf_alloc1_c_& other);
    sf_alloc1_c_& operator=(const sf_alloc1_c_& other);
    ~sf_alloc1_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<6>& one_frame()
    {
      assert_choice_type(types::one_frame, type_, "subframeAllocation1");
      return c.get<fixed_bitstring<6>>();
    }
    fixed_bitstring<24>& four_frames()
    {
      assert_choice_type(types::four_frames, type_, "subframeAllocation1");
      return c.get<fixed_bitstring<24>>();
    }
    const fixed_bitstring<6>& one_frame() const
    {
      assert_choice_type(types::one_frame, type_, "subframeAllocation1");
      return c.get<fixed_bitstring<6>>();
    }
    const fixed_bitstring<24>& four_frames() const
    {
      assert_choice_type(types::four_frames, type_, "subframeAllocation1");
      return c.get<fixed_bitstring<24>>();
    }
    fixed_bitstring<6>&  set_one_frame();
    fixed_bitstring<24>& set_four_frames();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<24>> c;

    void destroy_();
  };
  struct sf_alloc2_c_ {
    struct types_opts {
      enum options { one_frame, four_frames, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    sf_alloc2_c_() = default;
    sf_alloc2_c_(const sf_alloc2_c_& other);
    sf_alloc2_c_& operator=(const sf_alloc2_c_& other);
    ~sf_alloc2_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<2>& one_frame()
    {
      assert_choice_type(types::one_frame, type_, "subframeAllocation2");
      return c.get<fixed_bitstring<2>>();
    }
    fixed_bitstring<8>& four_frames()
    {
      assert_choice_type(types::four_frames, type_, "subframeAllocation2");
      return c.get<fixed_bitstring<8>>();
    }
    const fixed_bitstring<2>& one_frame() const
    {
      assert_choice_type(types::one_frame, type_, "subframeAllocation2");
      return c.get<fixed_bitstring<2>>();
    }
    const fixed_bitstring<8>& four_frames() const
    {
      assert_choice_type(types::four_frames, type_, "subframeAllocation2");
      return c.get<fixed_bitstring<8>>();
    }
    fixed_bitstring<2>& set_one_frame();
    fixed_bitstring<8>& set_four_frames();

  private:
    types                               type_;
    choice_buffer_t<fixed_bitstring<8>> c;

    void destroy_();
  };

  // member variables
  bool                       ext               = false;
  bool                       sf_alloc2_present = false;
  radioframe_alloc_period_e_ radioframe_alloc_period;
  uint8_t                    radioframe_alloc_offset = 0;
  sf_alloc1_c_               sf_alloc1;
  sf_alloc2_c_               sf_alloc2;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-NS-PmaxList ::= SEQUENCE (SIZE (1..8)) OF NR-NS-PmaxValue
using nr_ns_pmax_list_l = dyn_array<nr_ns_pmax_value_s>;

// EUTRA-MBSFN-SubframeConfigList ::= SEQUENCE (SIZE (1..8)) OF EUTRA-MBSFN-SubframeConfig
using eutra_mbsfn_sf_cfg_list_l = dyn_array<eutra_mbsfn_sf_cfg_s>;

// NR-MultiBandInfo ::= SEQUENCE
struct nr_multi_band_info_s {
  bool              freq_band_ind_nr_present = false;
  uint16_t          freq_band_ind_nr         = 1;
  nr_ns_pmax_list_l nr_ns_pmax_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MultiFrequencyBandListNR-SIB ::= SEQUENCE (SIZE (1..8)) OF NR-MultiBandInfo
using multi_freq_band_list_nr_sib_l = dyn_array<nr_multi_band_info_s>;

// RateMatchPatternLTE-CRS ::= SEQUENCE
struct rate_match_pattern_lte_crs_s {
  struct carrier_bw_dl_opts {
    enum options { n6, n15, n25, n50, n75, n100, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using carrier_bw_dl_e_ = enumerated<carrier_bw_dl_opts>;
  struct nrof_crs_ports_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrof_crs_ports_e_ = enumerated<nrof_crs_ports_opts>;
  struct v_shift_opts {
    enum options { n0, n1, n2, n3, n4, n5, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using v_shift_e_ = enumerated<v_shift_opts>;

  // member variables
  uint16_t                  carrier_freq_dl = 0;
  carrier_bw_dl_e_          carrier_bw_dl;
  eutra_mbsfn_sf_cfg_list_l mbsfn_sf_cfg_list;
  nrof_crs_ports_e_         nrof_crs_ports;
  v_shift_e_                v_shift;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingCycle ::= ENUMERATED
struct paging_cycle_opts {
  enum options { rf32, rf64, rf128, rf256, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using paging_cycle_e = enumerated<paging_cycle_opts>;

// SCS-SpecificCarrier ::= SEQUENCE
struct scs_specific_carrier_s {
  bool                 ext               = false;
  uint16_t             offset_to_carrier = 0;
  subcarrier_spacing_e subcarrier_spacing;
  uint16_t             carrier_bw = 1;
  // ...
  // group 0
  bool     tx_direct_current_location_present = false;
  uint16_t tx_direct_current_location         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SubgroupConfig-r17 ::= SEQUENCE
struct subgroup_cfg_r17_s {
  bool    ext                                 = false;
  bool    subgroups_num_for_ue_id_r17_present = false;
  uint8_t subgroups_num_per_po_r17            = 1;
  uint8_t subgroups_num_for_ue_id_r17         = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCCH-Config ::= SEQUENCE
struct bcch_cfg_s {
  struct mod_period_coeff_opts {
    enum options { n2, n4, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mod_period_coeff_e_ = enumerated<mod_period_coeff_opts>;

  // member variables
  bool                ext = false;
  mod_period_coeff_e_ mod_period_coeff;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FrequencyInfoDL-SIB ::= SEQUENCE
struct freq_info_dl_sib_s {
  using scs_specific_carrier_list_l_ = dyn_array<scs_specific_carrier_s>;

  // member variables
  multi_freq_band_list_nr_sib_l freq_band_list;
  uint16_t                      offset_to_point_a = 0;
  scs_specific_carrier_list_l_  scs_specific_carrier_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FrequencyInfoUL-SIB ::= SEQUENCE
struct freq_info_ul_sib_s {
  using scs_specific_carrier_list_l_ = dyn_array<scs_specific_carrier_s>;

  // member variables
  bool                          ext                           = false;
  bool                          absolute_freq_point_a_present = false;
  bool                          p_max_present                 = false;
  bool                          freq_shift7p5khz_present      = false;
  multi_freq_band_list_nr_sib_l freq_band_list;
  uint32_t                      absolute_freq_point_a = 0;
  scs_specific_carrier_list_l_  scs_specific_carrier_list;
  int8_t                        p_max = -30;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCCH-Config ::= SEQUENCE
struct pcch_cfg_s {
  struct nand_paging_frame_offset_c_ {
    struct types_opts {
      enum options { one_t, half_t, quarter_t, one_eighth_t, one_sixteenth_t, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    nand_paging_frame_offset_c_() = default;
    nand_paging_frame_offset_c_(const nand_paging_frame_offset_c_& other);
    nand_paging_frame_offset_c_& operator=(const nand_paging_frame_offset_c_& other);
    ~nand_paging_frame_offset_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& half_t()
    {
      assert_choice_type(types::half_t, type_, "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    uint8_t& quarter_t()
    {
      assert_choice_type(types::quarter_t, type_, "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    uint8_t& one_eighth_t()
    {
      assert_choice_type(types::one_eighth_t, type_, "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    uint8_t& one_sixteenth_t()
    {
      assert_choice_type(types::one_sixteenth_t, type_, "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& half_t() const
    {
      assert_choice_type(types::half_t, type_, "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& quarter_t() const
    {
      assert_choice_type(types::quarter_t, type_, "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& one_eighth_t() const
    {
      assert_choice_type(types::one_eighth_t, type_, "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& one_sixteenth_t() const
    {
      assert_choice_type(types::one_sixteenth_t, type_, "nAndPagingFrameOffset");
      return c.get<uint8_t>();
    }
    void     set_one_t();
    uint8_t& set_half_t();
    uint8_t& set_quarter_t();
    uint8_t& set_one_eighth_t();
    uint8_t& set_one_sixteenth_t();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct ns_opts {
    enum options { four, two, one, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ns_e_ = enumerated<ns_opts>;
  struct first_pdcch_monitoring_occasion_of_po_c_ {
    using scs15_kh_zone_t_l_                                                              = bounded_array<uint8_t, 4>;
    using scs30_kh_zone_t_scs15_kh_zhalf_t_l_                                             = bounded_array<uint16_t, 4>;
    using scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_                         = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_ = bounded_array<uint16_t, 4>;
    using scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_ =
        bounded_array<uint16_t, 4>;
    using scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_ =
        bounded_array<uint16_t, 4>;
    using scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_ = bounded_array<uint16_t, 4>;
    using scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_                     = bounded_array<uint16_t, 4>;
    struct types_opts {
      enum options {
        scs15_kh_zone_t,
        scs30_kh_zone_t_scs15_kh_zhalf_t,
        scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t,
        scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t,
        scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t,
        scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
        scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t,
        scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    first_pdcch_monitoring_occasion_of_po_c_() = default;
    first_pdcch_monitoring_occasion_of_po_c_(const first_pdcch_monitoring_occasion_of_po_c_& other);
    first_pdcch_monitoring_occasion_of_po_c_& operator=(const first_pdcch_monitoring_occasion_of_po_c_& other);
    ~first_pdcch_monitoring_occasion_of_po_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scs15_kh_zone_t_l_& scs15_kh_zone_t()
    {
      assert_choice_type(types::scs15_kh_zone_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs15_kh_zone_t_l_>();
    }
    scs30_kh_zone_t_scs15_kh_zhalf_t_l_& scs30_kh_zone_t_scs15_kh_zhalf_t()
    {
      assert_choice_type(types::scs30_kh_zone_t_scs15_kh_zhalf_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
    }
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_& scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t()
    {
      assert_choice_type(
          types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
    }
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t()
    {
      assert_choice_type(types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
    }
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t()
    {
      assert_choice_type(types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
    }
    scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t()
    {
      assert_choice_type(types::scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
    }
    scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
    scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t()
    {
      assert_choice_type(types::scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
    }
    scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_& scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t()
    {
      assert_choice_type(
          types::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_>();
    }
    const scs15_kh_zone_t_l_& scs15_kh_zone_t() const
    {
      assert_choice_type(types::scs15_kh_zone_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs15_kh_zone_t_l_>();
    }
    const scs30_kh_zone_t_scs15_kh_zhalf_t_l_& scs30_kh_zone_t_scs15_kh_zhalf_t() const
    {
      assert_choice_type(types::scs30_kh_zone_t_scs15_kh_zhalf_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
    }
    const scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_&
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t() const
    {
      assert_choice_type(
          types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
    }
    const scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t() const
    {
      assert_choice_type(types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
    }
    const scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t() const
    {
      assert_choice_type(types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
    }
    const scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t() const
    {
      assert_choice_type(types::scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
    }
    const scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
    scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t() const
    {
      assert_choice_type(types::scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
    }
    const scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_& scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t() const
    {
      assert_choice_type(
          types::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_>();
    }
    scs15_kh_zone_t_l_&                                      set_scs15_kh_zone_t();
    scs30_kh_zone_t_scs15_kh_zhalf_t_l_&                     set_scs30_kh_zone_t_scs15_kh_zhalf_t();
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_& set_scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t();
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    set_scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t();
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    set_scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t();
    scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    set_scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t();
    scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
    set_scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t();
    scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_& set_scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t();

  private:
    types type_;
    choice_buffer_t<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_,
                    scs15_kh_zone_t_l_,
                    scs30_kh_zone_t_scs15_kh_zhalf_t_l_,
                    scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_,
                    scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_,
                    scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_,
                    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>
        c;

    void destroy_();
  };
  struct first_pdcch_monitoring_occasion_of_po_v1710_c_ {
    using scs480_kh_zone_eighth_t_l_    = bounded_array<uint16_t, 4>;
    using scs480_kh_zone_sixteenth_t_l_ = bounded_array<uint32_t, 4>;
    struct types_opts {
      enum options { scs480_kh_zone_eighth_t, scs480_kh_zone_sixteenth_t, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    first_pdcch_monitoring_occasion_of_po_v1710_c_() = default;
    first_pdcch_monitoring_occasion_of_po_v1710_c_(const first_pdcch_monitoring_occasion_of_po_v1710_c_& other);
    first_pdcch_monitoring_occasion_of_po_v1710_c_&
    operator=(const first_pdcch_monitoring_occasion_of_po_v1710_c_& other);
    ~first_pdcch_monitoring_occasion_of_po_v1710_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scs480_kh_zone_eighth_t_l_& scs480_kh_zone_eighth_t()
    {
      assert_choice_type(types::scs480_kh_zone_eighth_t, type_, "firstPDCCH-MonitoringOccasionOfPO-v1710");
      return c.get<scs480_kh_zone_eighth_t_l_>();
    }
    scs480_kh_zone_sixteenth_t_l_& scs480_kh_zone_sixteenth_t()
    {
      assert_choice_type(types::scs480_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO-v1710");
      return c.get<scs480_kh_zone_sixteenth_t_l_>();
    }
    const scs480_kh_zone_eighth_t_l_& scs480_kh_zone_eighth_t() const
    {
      assert_choice_type(types::scs480_kh_zone_eighth_t, type_, "firstPDCCH-MonitoringOccasionOfPO-v1710");
      return c.get<scs480_kh_zone_eighth_t_l_>();
    }
    const scs480_kh_zone_sixteenth_t_l_& scs480_kh_zone_sixteenth_t() const
    {
      assert_choice_type(types::scs480_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO-v1710");
      return c.get<scs480_kh_zone_sixteenth_t_l_>();
    }
    scs480_kh_zone_eighth_t_l_&    set_scs480_kh_zone_eighth_t();
    scs480_kh_zone_sixteenth_t_l_& set_scs480_kh_zone_sixteenth_t();

  private:
    types                                                                      type_;
    choice_buffer_t<scs480_kh_zone_eighth_t_l_, scs480_kh_zone_sixteenth_t_l_> c;

    void destroy_();
  };

  // member variables
  bool                                     ext                                           = false;
  bool                                     first_pdcch_monitoring_occasion_of_po_present = false;
  paging_cycle_e                           default_paging_cycle;
  nand_paging_frame_offset_c_              nand_paging_frame_offset;
  ns_e_                                    ns;
  first_pdcch_monitoring_occasion_of_po_c_ first_pdcch_monitoring_occasion_of_po;
  // ...
  // group 0
  bool    nrof_pdcch_monitoring_occasion_per_ssb_in_po_r16_present = false;
  uint8_t nrof_pdcch_monitoring_occasion_per_ssb_in_po_r16         = 2;
  // group 1
  bool                                                     ran_paging_in_idle_po_r17_present = false;
  copy_ptr<first_pdcch_monitoring_occasion_of_po_v1710_c_> first_pdcch_monitoring_occasion_of_po_v1710;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PEI-Config-r17 ::= SEQUENCE
struct pei_cfg_r17_s {
  struct po_num_per_pei_r17_opts {
    enum options { po1, po2, po4, po8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using po_num_per_pei_r17_e_ = enumerated<po_num_per_pei_r17_opts>;

  // member variables
  bool                  ext                             = false;
  bool                  last_used_cell_only_r17_present = false;
  po_num_per_pei_r17_e_ po_num_per_pei_r17;
  uint8_t               payload_size_dci_2_7_r17 = 1;
  uint8_t               pei_frame_offset_r17     = 0;
  subgroup_cfg_r17_s    subgroup_cfg_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UL-DL-Pattern ::= SEQUENCE
struct tdd_ul_dl_pattern_s {
  struct dl_ul_tx_periodicity_opts {
    enum options { ms0p5, ms0p625, ms1, ms1p25, ms2, ms2p5, ms5, ms10, nulltype } value;

    const char* to_string() const;
  };
  using dl_ul_tx_periodicity_e_ = enumerated<dl_ul_tx_periodicity_opts>;
  struct dl_ul_tx_periodicity_v1530_opts {
    enum options { ms3, ms4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using dl_ul_tx_periodicity_v1530_e_ = enumerated<dl_ul_tx_periodicity_v1530_opts>;

  // member variables
  bool                    ext = false;
  dl_ul_tx_periodicity_e_ dl_ul_tx_periodicity;
  uint16_t                nrof_dl_slots   = 0;
  uint8_t                 nrof_dl_symbols = 0;
  uint16_t                nrof_ul_slots   = 0;
  uint8_t                 nrof_ul_symbols = 0;
  // ...
  // group 0
  bool                          dl_ul_tx_periodicity_v1530_present = false;
  dl_ul_tx_periodicity_v1530_e_ dl_ul_tx_periodicity_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TimeAlignmentTimer ::= ENUMERATED
struct time_align_timer_opts {
  enum options { ms500, ms750, ms1280, ms1920, ms2560, ms5120, ms10240, infinity, nulltype } value;
  typedef int16_t number_type;

  const char* to_string() const;
  int16_t     to_number() const;
};
using time_align_timer_e = enumerated<time_align_timer_opts>;

// DownlinkConfigCommonSIB ::= SEQUENCE
struct dl_cfg_common_sib_s {
  bool               ext = false;
  freq_info_dl_sib_s freq_info_dl;
  bwp_dl_common_s    init_dl_bwp;
  bcch_cfg_s         bcch_cfg;
  pcch_cfg_s         pcch_cfg;
  // ...
  // group 0
  copy_ptr<pei_cfg_r17_s>   pei_cfg_r17;
  copy_ptr<bwp_dl_common_s> init_dl_bwp_red_cap_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HighSpeedConfig-r16 ::= SEQUENCE
struct high_speed_cfg_r16_s {
  bool ext                               = false;
  bool high_speed_meas_flag_r16_present  = false;
  bool high_speed_demod_flag_r16_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HighSpeedConfigFR2-r17 ::= SEQUENCE
struct high_speed_cfg_fr2_r17_s {
  struct high_speed_meas_flag_fr2_r17_opts {
    enum options { set1, set2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using high_speed_meas_flag_fr2_r17_e_ = enumerated<high_speed_meas_flag_fr2_r17_opts>;
  struct high_speed_deployment_type_fr2_r17_opts {
    enum options { unidirectional, bidirectional, nulltype } value;

    const char* to_string() const;
  };
  using high_speed_deployment_type_fr2_r17_e_ = enumerated<high_speed_deployment_type_fr2_r17_opts>;

  // member variables
  bool                                  ext                                                 = false;
  bool                                  high_speed_meas_flag_fr2_r17_present                = false;
  bool                                  high_speed_deployment_type_fr2_r17_present          = false;
  bool                                  high_speed_large_one_step_ul_timing_fr2_r17_present = false;
  high_speed_meas_flag_fr2_r17_e_       high_speed_meas_flag_fr2_r17;
  high_speed_deployment_type_fr2_r17_e_ high_speed_deployment_type_fr2_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SemiStaticChannelAccessConfig-r16 ::= SEQUENCE
struct semi_static_ch_access_cfg_r16_s {
  struct period_opts {
    enum options { ms1, ms2, ms2dot5, ms4, ms5, ms10, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using period_e_ = enumerated<period_opts>;

  // member variables
  period_e_ period;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UL-DL-ConfigCommon ::= SEQUENCE
struct tdd_ul_dl_cfg_common_s {
  bool                 ext              = false;
  bool                 pattern2_present = false;
  subcarrier_spacing_e ref_subcarrier_spacing;
  tdd_ul_dl_pattern_s  pattern1;
  tdd_ul_dl_pattern_s  pattern2;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkConfigCommonSIB ::= SEQUENCE
struct ul_cfg_common_sib_s {
  freq_info_ul_sib_s freq_info_ul;
  bwp_ul_common_s    init_ul_bwp;
  time_align_timer_e time_align_timer_common;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkConfigCommonSIB-v1700 ::= SEQUENCE
struct ul_cfg_common_sib_v1700_s {
  bool            init_ul_bwp_red_cap_r17_present = false;
  bwp_ul_common_s init_ul_bwp_red_cap_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServingCellConfigCommonSIB ::= SEQUENCE
struct serving_cell_cfg_common_sib_s {
  struct n_timing_advance_offset_opts {
    enum options { n0, n25600, n39936, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using n_timing_advance_offset_e_ = enumerated<n_timing_advance_offset_opts>;
  struct ssb_positions_in_burst_s_ {
    bool               group_presence_present = false;
    fixed_bitstring<8> in_one_group;
    fixed_bitstring<8> group_presence;
  };
  struct ssb_periodicity_serving_cell_opts {
    enum options { ms5, ms10, ms20, ms40, ms80, ms160, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ssb_periodicity_serving_cell_e_ = enumerated<ssb_periodicity_serving_cell_opts>;
  struct ch_access_mode_r16_c_ {
    struct types_opts {
      enum options { dyn, semi_static, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

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
  using discovery_burst_win_len_r16_e_ = enumerated<discovery_burst_win_len_r16_opts>;
  struct discovery_burst_win_len_v1700_opts {
    enum options { ms0dot125, ms0dot25, ms0dot5, ms0dot75, ms1, ms1dot25, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using discovery_burst_win_len_v1700_e_ = enumerated<discovery_burst_win_len_v1700_opts>;

  // member variables
  bool                            ext                             = false;
  bool                            ul_cfg_common_present           = false;
  bool                            supplementary_ul_present        = false;
  bool                            n_timing_advance_offset_present = false;
  bool                            tdd_ul_dl_cfg_common_present    = false;
  dl_cfg_common_sib_s             dl_cfg_common;
  ul_cfg_common_sib_s             ul_cfg_common;
  ul_cfg_common_sib_s             supplementary_ul;
  n_timing_advance_offset_e_      n_timing_advance_offset;
  ssb_positions_in_burst_s_       ssb_positions_in_burst;
  ssb_periodicity_serving_cell_e_ ssb_periodicity_serving_cell;
  tdd_ul_dl_cfg_common_s          tdd_ul_dl_cfg_common;
  int8_t                          ss_pbch_block_pwr = -60;
  // ...
  // group 0
  bool                            discovery_burst_win_len_r16_present = false;
  copy_ptr<ch_access_mode_r16_c_> ch_access_mode_r16;
  discovery_burst_win_len_r16_e_  discovery_burst_win_len_r16;
  copy_ptr<high_speed_cfg_r16_s>  high_speed_cfg_r16;
  // group 1
  bool                                ch_access_mode2_r17_present           = false;
  bool                                discovery_burst_win_len_v1700_present = false;
  discovery_burst_win_len_v1700_e_    discovery_burst_win_len_v1700;
  copy_ptr<high_speed_cfg_fr2_r17_s>  high_speed_cfg_fr2_r17;
  copy_ptr<ul_cfg_common_sib_v1700_s> ul_cfg_common_v1700;
  // group 2
  bool enhanced_meas_leo_r17_present = false;
  // group 3
  bool ra_ch_access_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AvailabilityCombination-r16 ::= SEQUENCE
struct availability_combination_r16_s {
  using res_availability_r16_l_ = dyn_array<uint8_t>;

  // member variables
  uint16_t                availability_combination_id_r16 = 0;
  res_availability_r16_l_ res_availability_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RB-SetGroup-r17 ::= SEQUENCE
struct rb_set_group_r17_s {
  using res_availability_r17_l_ = dyn_array<uint8_t>;
  using rb_sets_r17_l_          = bounded_array<uint8_t, 8>;

  // member variables
  res_availability_r17_l_ res_availability_r17;
  rb_sets_r17_l_          rb_sets_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AvailabilityCombinationRB-Groups-r17 ::= SEQUENCE
struct availability_combination_rb_groups_r17_s {
  using rb_set_groups_r17_l_    = dyn_array<rb_set_group_r17_s>;
  using res_availability_r17_l_ = dyn_array<uint8_t>;

  // member variables
  uint16_t                availability_combination_id_r17 = 0;
  rb_set_groups_r17_l_    rb_set_groups_r17;
  res_availability_r17_l_ res_availability_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AvailabilityCombinationsPerCell-r16 ::= SEQUENCE
struct availability_combinations_per_cell_r16_s {
  using availability_combinations_r16_l_           = dyn_array<availability_combination_r16_s>;
  using availability_combinations_rb_groups_r17_l_ = dyn_array<availability_combination_rb_groups_r17_s>;

  // member variables
  bool                             ext                                        = false;
  bool                             position_in_dci_ai_r16_present             = false;
  uint16_t                         availability_combinations_per_cell_idx_r16 = 0;
  fixed_bitstring<36>              iab_du_cell_id_r16;
  uint8_t                          position_in_dci_ai_r16 = 0;
  availability_combinations_r16_l_ availability_combinations_r16;
  // ...
  // group 0
  copy_ptr<availability_combinations_rb_groups_r17_l_> availability_combinations_rb_groups_r17;
  // group 1
  bool    position_in_dci_ai_rb_groups_v1720_present = false;
  uint8_t position_in_dci_ai_rb_groups_v1720         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AvailabilityIndicator-r16 ::= SEQUENCE
struct availability_ind_r16_s {
  using available_comb_to_add_mod_list_r16_l_ = dyn_array<availability_combinations_per_cell_r16_s>;
  using available_comb_to_release_list_r16_l_ = dyn_array<uint16_t>;

  // member variables
  bool                                  ext                     = false;
  uint32_t                              ai_rnti_r16             = 0;
  uint8_t                               dci_payload_size_ai_r16 = 1;
  available_comb_to_add_mod_list_r16_l_ available_comb_to_add_mod_list_r16;
  available_comb_to_release_list_r16_l_ available_comb_to_release_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AvailableRB-SetsPerCell-r16 ::= SEQUENCE
struct available_rb_sets_per_cell_r16_s {
  uint8_t serving_cell_id_r16 = 0;
  uint8_t position_in_dci_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NZP-CSI-RS-Pairing-r17 ::= SEQUENCE
struct nzp_csi_rs_pairing_r17_s {
  uint8_t nzp_csi_rs_res_id1_r17 = 1;
  uint8_t nzp_csi_rs_res_id2_r17 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CMRGroupingAndPairing-r17 ::= SEQUENCE
struct cmr_grouping_and_pairing_r17_s {
  bool                     pair1_of_nzp_csi_rs_r17_present = false;
  bool                     pair2_of_nzp_csi_rs_r17_present = false;
  uint8_t                  nrof_res_group1_r17             = 1;
  nzp_csi_rs_pairing_r17_s pair1_of_nzp_csi_rs_r17;
  nzp_csi_rs_pairing_r17_s pair2_of_nzp_csi_rs_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CO-DurationsPerCell-r16 ::= SEQUENCE
struct co_durs_per_cell_r16_s {
  using co_dur_list_r16_l_ = dyn_array<uint16_t>;

  // member variables
  uint8_t              serving_cell_id_r16 = 0;
  uint8_t              position_in_dci_r16 = 0;
  subcarrier_spacing_e subcarrier_spacing_r16;
  co_dur_list_r16_l_   co_dur_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CO-DurationsPerCell-r17 ::= SEQUENCE
struct co_durs_per_cell_r17_s {
  using co_dur_list_r17_l_ = dyn_array<uint16_t>;

  // member variables
  uint8_t              serving_cell_id_r17 = 0;
  uint8_t              position_in_dci_r17 = 0;
  subcarrier_spacing_e subcarrier_spacing_r17;
  co_dur_list_r17_l_   co_dur_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-AssociatedReportConfigInfo ::= SEQUENCE
struct csi_associated_report_cfg_info_s {
  struct res_for_ch_c_ {
    struct nzp_csi_rs_s_ {
      using qcl_info_l_ = bounded_array<uint8_t, 16>;

      // member variables
      uint8_t     res_set = 1;
      qcl_info_l_ qcl_info;
    };
    struct types_opts {
      enum options { nzp_csi_rs, csi_ssb_res_set, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    res_for_ch_c_() = default;
    res_for_ch_c_(const res_for_ch_c_& other);
    res_for_ch_c_& operator=(const res_for_ch_c_& other);
    ~res_for_ch_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nzp_csi_rs_s_& nzp_csi_rs()
    {
      assert_choice_type(types::nzp_csi_rs, type_, "resourcesForChannel");
      return c.get<nzp_csi_rs_s_>();
    }
    uint8_t& csi_ssb_res_set()
    {
      assert_choice_type(types::csi_ssb_res_set, type_, "resourcesForChannel");
      return c.get<uint8_t>();
    }
    const nzp_csi_rs_s_& nzp_csi_rs() const
    {
      assert_choice_type(types::nzp_csi_rs, type_, "resourcesForChannel");
      return c.get<nzp_csi_rs_s_>();
    }
    const uint8_t& csi_ssb_res_set() const
    {
      assert_choice_type(types::csi_ssb_res_set, type_, "resourcesForChannel");
      return c.get<uint8_t>();
    }
    nzp_csi_rs_s_& set_nzp_csi_rs();
    uint8_t&       set_csi_ssb_res_set();

  private:
    types                          type_;
    choice_buffer_t<nzp_csi_rs_s_> c;

    void destroy_();
  };
  struct res_for_ch2_r17_c_ {
    struct nzp_csi_rs2_r17_s_ {
      using qcl_info2_r17_l_ = bounded_array<uint8_t, 16>;

      // member variables
      uint8_t          res_set2_r17 = 1;
      qcl_info2_r17_l_ qcl_info2_r17;
    };
    struct types_opts {
      enum options { nzp_csi_rs2_r17, csi_ssb_res_set2_r17, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    res_for_ch2_r17_c_() = default;
    res_for_ch2_r17_c_(const res_for_ch2_r17_c_& other);
    res_for_ch2_r17_c_& operator=(const res_for_ch2_r17_c_& other);
    ~res_for_ch2_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nzp_csi_rs2_r17_s_& nzp_csi_rs2_r17()
    {
      assert_choice_type(types::nzp_csi_rs2_r17, type_, "resourcesForChannel2-r17");
      return c.get<nzp_csi_rs2_r17_s_>();
    }
    uint8_t& csi_ssb_res_set2_r17()
    {
      assert_choice_type(types::csi_ssb_res_set2_r17, type_, "resourcesForChannel2-r17");
      return c.get<uint8_t>();
    }
    const nzp_csi_rs2_r17_s_& nzp_csi_rs2_r17() const
    {
      assert_choice_type(types::nzp_csi_rs2_r17, type_, "resourcesForChannel2-r17");
      return c.get<nzp_csi_rs2_r17_s_>();
    }
    const uint8_t& csi_ssb_res_set2_r17() const
    {
      assert_choice_type(types::csi_ssb_res_set2_r17, type_, "resourcesForChannel2-r17");
      return c.get<uint8_t>();
    }
    nzp_csi_rs2_r17_s_& set_nzp_csi_rs2_r17();
    uint8_t&            set_csi_ssb_res_set2_r17();

  private:
    types                               type_;
    choice_buffer_t<nzp_csi_rs2_r17_s_> c;

    void destroy_();
  };

  // member variables
  bool          ext                                     = false;
  bool          csi_im_res_for_interference_present     = false;
  bool          nzp_csi_rs_res_for_interference_present = false;
  uint8_t       report_cfg_id                           = 0;
  res_for_ch_c_ res_for_ch;
  uint8_t       csi_im_res_for_interference     = 1;
  uint8_t       nzp_csi_rs_res_for_interference = 1;
  // ...
  // group 0
  bool                         csi_ssb_res_set_ext_present = false;
  copy_ptr<res_for_ch2_r17_c_> res_for_ch2_r17;
  uint8_t                      csi_ssb_res_set_ext = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-AperiodicTriggerState ::= SEQUENCE
struct csi_aperiodic_trigger_state_s {
  using associated_report_cfg_info_list_l_ = dyn_array<csi_associated_report_cfg_info_s>;

  // member variables
  bool                               ext = false;
  associated_report_cfg_info_list_l_ associated_report_cfg_info_list;
  // ...
  // group 0
  bool ap_csi_mux_mode_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-AperiodicTriggerStateList ::= SEQUENCE (SIZE (1..128)) OF CSI-AperiodicTriggerState
using csi_aperiodic_trigger_state_list_l = dyn_array<csi_aperiodic_trigger_state_s>;

// CSI-IM-Resource ::= SEQUENCE
struct csi_im_res_s {
  struct csi_im_res_elem_pattern_c_ {
    struct pattern0_s_ {
      struct subcarrier_location_p0_opts {
        enum options { s0, s2, s4, s6, s8, s10, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using subcarrier_location_p0_e_ = enumerated<subcarrier_location_p0_opts>;

      // member variables
      subcarrier_location_p0_e_ subcarrier_location_p0;
      uint8_t                   symbol_location_p0 = 0;
    };
    struct pattern1_s_ {
      struct subcarrier_location_p1_opts {
        enum options { s0, s4, s8, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using subcarrier_location_p1_e_ = enumerated<subcarrier_location_p1_opts>;

      // member variables
      subcarrier_location_p1_e_ subcarrier_location_p1;
      uint8_t                   symbol_location_p1 = 0;
    };
    struct types_opts {
      enum options { pattern0, pattern1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    csi_im_res_elem_pattern_c_() = default;
    csi_im_res_elem_pattern_c_(const csi_im_res_elem_pattern_c_& other);
    csi_im_res_elem_pattern_c_& operator=(const csi_im_res_elem_pattern_c_& other);
    ~csi_im_res_elem_pattern_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pattern0_s_& pattern0()
    {
      assert_choice_type(types::pattern0, type_, "csi-IM-ResourceElementPattern");
      return c.get<pattern0_s_>();
    }
    pattern1_s_& pattern1()
    {
      assert_choice_type(types::pattern1, type_, "csi-IM-ResourceElementPattern");
      return c.get<pattern1_s_>();
    }
    const pattern0_s_& pattern0() const
    {
      assert_choice_type(types::pattern0, type_, "csi-IM-ResourceElementPattern");
      return c.get<pattern0_s_>();
    }
    const pattern1_s_& pattern1() const
    {
      assert_choice_type(types::pattern1, type_, "csi-IM-ResourceElementPattern");
      return c.get<pattern1_s_>();
    }
    pattern0_s_& set_pattern0();
    pattern1_s_& set_pattern1();

  private:
    types                                     type_;
    choice_buffer_t<pattern0_s_, pattern1_s_> c;

    void destroy_();
  };

  // member variables
  bool                             ext                             = false;
  bool                             csi_im_res_elem_pattern_present = false;
  bool                             freq_band_present               = false;
  bool                             periodicity_and_offset_present  = false;
  uint8_t                          csi_im_res_id                   = 0;
  csi_im_res_elem_pattern_c_       csi_im_res_elem_pattern;
  csi_freq_occupation_s            freq_band;
  csi_res_periodicity_and_offset_c periodicity_and_offset;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-IM-ResourceSet ::= SEQUENCE
struct csi_im_res_set_s {
  using csi_im_res_l_ = bounded_array<uint8_t, 8>;

  // member variables
  bool          ext               = false;
  uint8_t       csi_im_res_set_id = 0;
  csi_im_res_l_ csi_im_res;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-ReportPeriodicityAndOffset ::= CHOICE
struct csi_report_periodicity_and_offset_c {
  struct types_opts {
    enum options {
      slots4,
      slots5,
      slots8,
      slots10,
      slots16,
      slots20,
      slots40,
      slots80,
      slots160,
      slots320,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  csi_report_periodicity_and_offset_c() = default;
  csi_report_periodicity_and_offset_c(const csi_report_periodicity_and_offset_c& other);
  csi_report_periodicity_and_offset_c& operator=(const csi_report_periodicity_and_offset_c& other);
  ~csi_report_periodicity_and_offset_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& slots4()
  {
    assert_choice_type(types::slots4, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots5()
  {
    assert_choice_type(types::slots5, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots8()
  {
    assert_choice_type(types::slots8, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots10()
  {
    assert_choice_type(types::slots10, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots16()
  {
    assert_choice_type(types::slots16, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots20()
  {
    assert_choice_type(types::slots20, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots40()
  {
    assert_choice_type(types::slots40, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots80()
  {
    assert_choice_type(types::slots80, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots160()
  {
    assert_choice_type(types::slots160, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint16_t& slots320()
  {
    assert_choice_type(types::slots320, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint8_t& slots4() const
  {
    assert_choice_type(types::slots4, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots5() const
  {
    assert_choice_type(types::slots5, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots8() const
  {
    assert_choice_type(types::slots8, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots10() const
  {
    assert_choice_type(types::slots10, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots16() const
  {
    assert_choice_type(types::slots16, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots20() const
  {
    assert_choice_type(types::slots20, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots40() const
  {
    assert_choice_type(types::slots40, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots80() const
  {
    assert_choice_type(types::slots80, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots160() const
  {
    assert_choice_type(types::slots160, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint16_t& slots320() const
  {
    assert_choice_type(types::slots320, type_, "CSI-ReportPeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  uint8_t&  set_slots4();
  uint8_t&  set_slots5();
  uint8_t&  set_slots8();
  uint8_t&  set_slots10();
  uint8_t&  set_slots16();
  uint8_t&  set_slots20();
  uint8_t&  set_slots40();
  uint8_t&  set_slots80();
  uint8_t&  set_slots160();
  uint16_t& set_slots320();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// CSI-SemiPersistentOnPUSCH-TriggerState ::= SEQUENCE
struct csi_semi_persistent_on_pusch_trigger_state_s {
  bool    ext                        = false;
  uint8_t associated_report_cfg_info = 0;
  // ...
  // group 0
  bool sp_csi_mux_mode_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookConfig ::= SEQUENCE
struct codebook_cfg_s {
  struct codebook_type_c_ {
    struct type1_s_ {
      struct sub_type_c_ {
        struct type_i_single_panel_s_ {
          struct nr_of_ant_ports_c_ {
            struct two_s_ {
              fixed_bitstring<6> two_tx_codebook_subset_restrict;
            };
            struct more_than_two_s_ {
              struct n1_n2_c_ {
                struct types_opts {
                  enum options {
                    two_one_type_i_single_panel_restrict,
                    two_two_type_i_single_panel_restrict,
                    four_one_type_i_single_panel_restrict,
                    three_two_type_i_single_panel_restrict,
                    six_one_type_i_single_panel_restrict,
                    four_two_type_i_single_panel_restrict,
                    eight_one_type_i_single_panel_restrict,
                    four_three_type_i_single_panel_restrict,
                    six_two_type_i_single_panel_restrict,
                    twelve_one_type_i_single_panel_restrict,
                    four_four_type_i_single_panel_restrict,
                    eight_two_type_i_single_panel_restrict,
                    sixteen_one_type_i_single_panel_restrict,
                    nulltype
                  } value;

                  const char* to_string() const;
                };
                using types = enumerated<types_opts>;

                // choice methods
                n1_n2_c_() = default;
                n1_n2_c_(const n1_n2_c_& other);
                n1_n2_c_& operator=(const n1_n2_c_& other);
                ~n1_n2_c_() { destroy_(); }
                void        set(types::options e = types::nulltype);
                types       type() const { return type_; }
                SRSASN_CODE pack(bit_ref& bref) const;
                SRSASN_CODE unpack(cbit_ref& bref);
                void        to_json(json_writer& j) const;
                // getters
                fixed_bitstring<8>& two_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::two_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<8>>();
                }
                fixed_bitstring<64>& two_two_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::two_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<64>>();
                }
                fixed_bitstring<16>& four_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::four_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<16>>();
                }
                fixed_bitstring<96>& three_two_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::three_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<96>>();
                }
                fixed_bitstring<24>& six_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::six_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<24>>();
                }
                fixed_bitstring<128>& four_two_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::four_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<128>>();
                }
                fixed_bitstring<32>& eight_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::eight_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<32>>();
                }
                fixed_bitstring<192>& four_three_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::four_three_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<192>>();
                }
                fixed_bitstring<192>& six_two_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::six_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<192>>();
                }
                fixed_bitstring<48>& twelve_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::twelve_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<48>>();
                }
                fixed_bitstring<256>& four_four_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::four_four_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<256>>();
                }
                fixed_bitstring<256>& eight_two_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::eight_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<256>>();
                }
                fixed_bitstring<64>& sixteen_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::sixteen_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<64>>();
                }
                const fixed_bitstring<8>& two_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::two_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<8>>();
                }
                const fixed_bitstring<64>& two_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::two_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<64>>();
                }
                const fixed_bitstring<16>& four_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::four_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<16>>();
                }
                const fixed_bitstring<96>& three_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::three_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<96>>();
                }
                const fixed_bitstring<24>& six_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::six_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<24>>();
                }
                const fixed_bitstring<128>& four_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::four_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<128>>();
                }
                const fixed_bitstring<32>& eight_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::eight_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<32>>();
                }
                const fixed_bitstring<192>& four_three_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::four_three_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<192>>();
                }
                const fixed_bitstring<192>& six_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::six_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<192>>();
                }
                const fixed_bitstring<48>& twelve_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::twelve_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<48>>();
                }
                const fixed_bitstring<256>& four_four_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::four_four_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<256>>();
                }
                const fixed_bitstring<256>& eight_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::eight_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<256>>();
                }
                const fixed_bitstring<64>& sixteen_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::sixteen_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<64>>();
                }
                fixed_bitstring<8>&   set_two_one_type_i_single_panel_restrict();
                fixed_bitstring<64>&  set_two_two_type_i_single_panel_restrict();
                fixed_bitstring<16>&  set_four_one_type_i_single_panel_restrict();
                fixed_bitstring<96>&  set_three_two_type_i_single_panel_restrict();
                fixed_bitstring<24>&  set_six_one_type_i_single_panel_restrict();
                fixed_bitstring<128>& set_four_two_type_i_single_panel_restrict();
                fixed_bitstring<32>&  set_eight_one_type_i_single_panel_restrict();
                fixed_bitstring<192>& set_four_three_type_i_single_panel_restrict();
                fixed_bitstring<192>& set_six_two_type_i_single_panel_restrict();
                fixed_bitstring<48>&  set_twelve_one_type_i_single_panel_restrict();
                fixed_bitstring<256>& set_four_four_type_i_single_panel_restrict();
                fixed_bitstring<256>& set_eight_two_type_i_single_panel_restrict();
                fixed_bitstring<64>&  set_sixteen_one_type_i_single_panel_restrict();

              private:
                types                                 type_;
                choice_buffer_t<fixed_bitstring<256>> c;

                void destroy_();
              };

              // member variables
              bool                type_i_single_panel_codebook_subset_restrict_i2_present = false;
              n1_n2_c_            n1_n2;
              fixed_bitstring<16> type_i_single_panel_codebook_subset_restrict_i2;
            };
            struct types_opts {
              enum options { two, more_than_two, nulltype } value;
              typedef uint8_t number_type;

              const char* to_string() const;
              uint8_t     to_number() const;
            };
            using types = enumerated<types_opts>;

            // choice methods
            nr_of_ant_ports_c_() = default;
            nr_of_ant_ports_c_(const nr_of_ant_ports_c_& other);
            nr_of_ant_ports_c_& operator=(const nr_of_ant_ports_c_& other);
            ~nr_of_ant_ports_c_() { destroy_(); }
            void        set(types::options e = types::nulltype);
            types       type() const { return type_; }
            SRSASN_CODE pack(bit_ref& bref) const;
            SRSASN_CODE unpack(cbit_ref& bref);
            void        to_json(json_writer& j) const;
            // getters
            two_s_& two()
            {
              assert_choice_type(types::two, type_, "nrOfAntennaPorts");
              return c.get<two_s_>();
            }
            more_than_two_s_& more_than_two()
            {
              assert_choice_type(types::more_than_two, type_, "nrOfAntennaPorts");
              return c.get<more_than_two_s_>();
            }
            const two_s_& two() const
            {
              assert_choice_type(types::two, type_, "nrOfAntennaPorts");
              return c.get<two_s_>();
            }
            const more_than_two_s_& more_than_two() const
            {
              assert_choice_type(types::more_than_two, type_, "nrOfAntennaPorts");
              return c.get<more_than_two_s_>();
            }
            two_s_&           set_two();
            more_than_two_s_& set_more_than_two();

          private:
            types                                     type_;
            choice_buffer_t<more_than_two_s_, two_s_> c;

            void destroy_();
          };

          // member variables
          nr_of_ant_ports_c_ nr_of_ant_ports;
          fixed_bitstring<8> type_i_single_panel_ri_restrict;
        };
        struct type_i_multi_panel_s_ {
          struct ng_n1_n2_c_ {
            struct types_opts {
              enum options {
                two_two_one_type_i_multi_panel_restrict,
                two_four_one_type_i_multi_panel_restrict,
                four_two_one_type_i_multi_panel_restrict,
                two_two_two_type_i_multi_panel_restrict,
                two_eight_one_type_i_multi_panel_restrict,
                four_four_one_type_i_multi_panel_restrict,
                two_four_two_type_i_multi_panel_restrict,
                four_two_two_type_i_multi_panel_restrict,
                nulltype
              } value;

              const char* to_string() const;
            };
            using types = enumerated<types_opts>;

            // choice methods
            ng_n1_n2_c_() = default;
            ng_n1_n2_c_(const ng_n1_n2_c_& other);
            ng_n1_n2_c_& operator=(const ng_n1_n2_c_& other);
            ~ng_n1_n2_c_() { destroy_(); }
            void        set(types::options e = types::nulltype);
            types       type() const { return type_; }
            SRSASN_CODE pack(bit_ref& bref) const;
            SRSASN_CODE unpack(cbit_ref& bref);
            void        to_json(json_writer& j) const;
            // getters
            fixed_bitstring<8>& two_two_one_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::two_two_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<8>>();
            }
            fixed_bitstring<16>& two_four_one_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::two_four_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<16>>();
            }
            fixed_bitstring<8>& four_two_one_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::four_two_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<8>>();
            }
            fixed_bitstring<64>& two_two_two_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::two_two_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<64>>();
            }
            fixed_bitstring<32>& two_eight_one_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::two_eight_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<32>>();
            }
            fixed_bitstring<16>& four_four_one_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::four_four_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<16>>();
            }
            fixed_bitstring<128>& two_four_two_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::two_four_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<128>>();
            }
            fixed_bitstring<64>& four_two_two_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::four_two_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<64>>();
            }
            const fixed_bitstring<8>& two_two_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::two_two_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<8>>();
            }
            const fixed_bitstring<16>& two_four_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::two_four_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<16>>();
            }
            const fixed_bitstring<8>& four_two_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::four_two_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<8>>();
            }
            const fixed_bitstring<64>& two_two_two_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::two_two_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<64>>();
            }
            const fixed_bitstring<32>& two_eight_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::two_eight_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<32>>();
            }
            const fixed_bitstring<16>& four_four_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::four_four_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<16>>();
            }
            const fixed_bitstring<128>& two_four_two_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::two_four_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<128>>();
            }
            const fixed_bitstring<64>& four_two_two_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::four_two_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<64>>();
            }
            fixed_bitstring<8>&   set_two_two_one_type_i_multi_panel_restrict();
            fixed_bitstring<16>&  set_two_four_one_type_i_multi_panel_restrict();
            fixed_bitstring<8>&   set_four_two_one_type_i_multi_panel_restrict();
            fixed_bitstring<64>&  set_two_two_two_type_i_multi_panel_restrict();
            fixed_bitstring<32>&  set_two_eight_one_type_i_multi_panel_restrict();
            fixed_bitstring<16>&  set_four_four_one_type_i_multi_panel_restrict();
            fixed_bitstring<128>& set_two_four_two_type_i_multi_panel_restrict();
            fixed_bitstring<64>&  set_four_two_two_type_i_multi_panel_restrict();

          private:
            types                                 type_;
            choice_buffer_t<fixed_bitstring<128>> c;

            void destroy_();
          };

          // member variables
          ng_n1_n2_c_        ng_n1_n2;
          fixed_bitstring<4> ri_restrict;
        };
        struct types_opts {
          enum options { type_i_single_panel, type_i_multi_panel, nulltype } value;

          const char* to_string() const;
        };
        using types = enumerated<types_opts>;

        // choice methods
        sub_type_c_() = default;
        sub_type_c_(const sub_type_c_& other);
        sub_type_c_& operator=(const sub_type_c_& other);
        ~sub_type_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        type_i_single_panel_s_& type_i_single_panel()
        {
          assert_choice_type(types::type_i_single_panel, type_, "subType");
          return c.get<type_i_single_panel_s_>();
        }
        type_i_multi_panel_s_& type_i_multi_panel()
        {
          assert_choice_type(types::type_i_multi_panel, type_, "subType");
          return c.get<type_i_multi_panel_s_>();
        }
        const type_i_single_panel_s_& type_i_single_panel() const
        {
          assert_choice_type(types::type_i_single_panel, type_, "subType");
          return c.get<type_i_single_panel_s_>();
        }
        const type_i_multi_panel_s_& type_i_multi_panel() const
        {
          assert_choice_type(types::type_i_multi_panel, type_, "subType");
          return c.get<type_i_multi_panel_s_>();
        }
        type_i_single_panel_s_& set_type_i_single_panel();
        type_i_multi_panel_s_&  set_type_i_multi_panel();

      private:
        types                                                          type_;
        choice_buffer_t<type_i_multi_panel_s_, type_i_single_panel_s_> c;

        void destroy_();
      };

      // member variables
      sub_type_c_ sub_type;
      uint8_t     codebook_mode = 1;
    };
    struct type2_s_ {
      struct sub_type_c_ {
        struct type_ii_s_ {
          struct n1_n2_codebook_subset_restrict_c_ {
            struct types_opts {
              enum options {
                two_one,
                two_two,
                four_one,
                three_two,
                six_one,
                four_two,
                eight_one,
                four_three,
                six_two,
                twelve_one,
                four_four,
                eight_two,
                sixteen_one,
                nulltype
              } value;

              const char* to_string() const;
            };
            using types = enumerated<types_opts>;

            // choice methods
            n1_n2_codebook_subset_restrict_c_() = default;
            n1_n2_codebook_subset_restrict_c_(const n1_n2_codebook_subset_restrict_c_& other);
            n1_n2_codebook_subset_restrict_c_& operator=(const n1_n2_codebook_subset_restrict_c_& other);
            ~n1_n2_codebook_subset_restrict_c_() { destroy_(); }
            void        set(types::options e = types::nulltype);
            types       type() const { return type_; }
            SRSASN_CODE pack(bit_ref& bref) const;
            SRSASN_CODE unpack(cbit_ref& bref);
            void        to_json(json_writer& j) const;
            // getters
            fixed_bitstring<16>& two_one()
            {
              assert_choice_type(types::two_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<16>>();
            }
            fixed_bitstring<43>& two_two()
            {
              assert_choice_type(types::two_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<43>>();
            }
            fixed_bitstring<32>& four_one()
            {
              assert_choice_type(types::four_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<32>>();
            }
            fixed_bitstring<59>& three_two()
            {
              assert_choice_type(types::three_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<59>>();
            }
            fixed_bitstring<48>& six_one()
            {
              assert_choice_type(types::six_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<48>>();
            }
            fixed_bitstring<75>& four_two()
            {
              assert_choice_type(types::four_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<75>>();
            }
            fixed_bitstring<64>& eight_one()
            {
              assert_choice_type(types::eight_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<64>>();
            }
            fixed_bitstring<107>& four_three()
            {
              assert_choice_type(types::four_three, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<107>>();
            }
            fixed_bitstring<107>& six_two()
            {
              assert_choice_type(types::six_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<107>>();
            }
            fixed_bitstring<96>& twelve_one()
            {
              assert_choice_type(types::twelve_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<96>>();
            }
            fixed_bitstring<139>& four_four()
            {
              assert_choice_type(types::four_four, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<139>>();
            }
            fixed_bitstring<139>& eight_two()
            {
              assert_choice_type(types::eight_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<139>>();
            }
            fixed_bitstring<128>& sixteen_one()
            {
              assert_choice_type(types::sixteen_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<128>>();
            }
            const fixed_bitstring<16>& two_one() const
            {
              assert_choice_type(types::two_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<16>>();
            }
            const fixed_bitstring<43>& two_two() const
            {
              assert_choice_type(types::two_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<43>>();
            }
            const fixed_bitstring<32>& four_one() const
            {
              assert_choice_type(types::four_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<32>>();
            }
            const fixed_bitstring<59>& three_two() const
            {
              assert_choice_type(types::three_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<59>>();
            }
            const fixed_bitstring<48>& six_one() const
            {
              assert_choice_type(types::six_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<48>>();
            }
            const fixed_bitstring<75>& four_two() const
            {
              assert_choice_type(types::four_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<75>>();
            }
            const fixed_bitstring<64>& eight_one() const
            {
              assert_choice_type(types::eight_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<64>>();
            }
            const fixed_bitstring<107>& four_three() const
            {
              assert_choice_type(types::four_three, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<107>>();
            }
            const fixed_bitstring<107>& six_two() const
            {
              assert_choice_type(types::six_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<107>>();
            }
            const fixed_bitstring<96>& twelve_one() const
            {
              assert_choice_type(types::twelve_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<96>>();
            }
            const fixed_bitstring<139>& four_four() const
            {
              assert_choice_type(types::four_four, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<139>>();
            }
            const fixed_bitstring<139>& eight_two() const
            {
              assert_choice_type(types::eight_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<139>>();
            }
            const fixed_bitstring<128>& sixteen_one() const
            {
              assert_choice_type(types::sixteen_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<128>>();
            }
            fixed_bitstring<16>&  set_two_one();
            fixed_bitstring<43>&  set_two_two();
            fixed_bitstring<32>&  set_four_one();
            fixed_bitstring<59>&  set_three_two();
            fixed_bitstring<48>&  set_six_one();
            fixed_bitstring<75>&  set_four_two();
            fixed_bitstring<64>&  set_eight_one();
            fixed_bitstring<107>& set_four_three();
            fixed_bitstring<107>& set_six_two();
            fixed_bitstring<96>&  set_twelve_one();
            fixed_bitstring<139>& set_four_four();
            fixed_bitstring<139>& set_eight_two();
            fixed_bitstring<128>& set_sixteen_one();

          private:
            types                                 type_;
            choice_buffer_t<fixed_bitstring<139>> c;

            void destroy_();
          };

          // member variables
          n1_n2_codebook_subset_restrict_c_ n1_n2_codebook_subset_restrict;
          fixed_bitstring<2>                type_ii_ri_restrict;
        };
        struct type_ii_port_sel_s_ {
          struct port_sel_sampling_size_opts {
            enum options { n1, n2, n3, n4, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using port_sel_sampling_size_e_ = enumerated<port_sel_sampling_size_opts>;

          // member variables
          bool                      port_sel_sampling_size_present = false;
          port_sel_sampling_size_e_ port_sel_sampling_size;
          fixed_bitstring<2>        type_ii_port_sel_ri_restrict;
        };
        struct types_opts {
          enum options { type_ii, type_ii_port_sel, nulltype } value;

          const char* to_string() const;
        };
        using types = enumerated<types_opts>;

        // choice methods
        sub_type_c_() = default;
        sub_type_c_(const sub_type_c_& other);
        sub_type_c_& operator=(const sub_type_c_& other);
        ~sub_type_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        type_ii_s_& type_ii()
        {
          assert_choice_type(types::type_ii, type_, "subType");
          return c.get<type_ii_s_>();
        }
        type_ii_port_sel_s_& type_ii_port_sel()
        {
          assert_choice_type(types::type_ii_port_sel, type_, "subType");
          return c.get<type_ii_port_sel_s_>();
        }
        const type_ii_s_& type_ii() const
        {
          assert_choice_type(types::type_ii, type_, "subType");
          return c.get<type_ii_s_>();
        }
        const type_ii_port_sel_s_& type_ii_port_sel() const
        {
          assert_choice_type(types::type_ii_port_sel, type_, "subType");
          return c.get<type_ii_port_sel_s_>();
        }
        type_ii_s_&          set_type_ii();
        type_ii_port_sel_s_& set_type_ii_port_sel();

      private:
        types                                            type_;
        choice_buffer_t<type_ii_port_sel_s_, type_ii_s_> c;

        void destroy_();
      };
      struct phase_alphabet_size_opts {
        enum options { n4, n8, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using phase_alphabet_size_e_ = enumerated<phase_alphabet_size_opts>;
      struct nof_beams_opts {
        enum options { two, three, four, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using nof_beams_e_ = enumerated<nof_beams_opts>;

      // member variables
      sub_type_c_            sub_type;
      phase_alphabet_size_e_ phase_alphabet_size;
      bool                   subband_amplitude = false;
      nof_beams_e_           nof_beams;
    };
    struct types_opts {
      enum options { type1, type2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    codebook_type_c_() = default;
    codebook_type_c_(const codebook_type_c_& other);
    codebook_type_c_& operator=(const codebook_type_c_& other);
    ~codebook_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    type1_s_& type1()
    {
      assert_choice_type(types::type1, type_, "codebookType");
      return c.get<type1_s_>();
    }
    type2_s_& type2()
    {
      assert_choice_type(types::type2, type_, "codebookType");
      return c.get<type2_s_>();
    }
    const type1_s_& type1() const
    {
      assert_choice_type(types::type1, type_, "codebookType");
      return c.get<type1_s_>();
    }
    const type2_s_& type2() const
    {
      assert_choice_type(types::type2, type_, "codebookType");
      return c.get<type2_s_>();
    }
    type1_s_& set_type1();
    type2_s_& set_type2();

  private:
    types                               type_;
    choice_buffer_t<type1_s_, type2_s_> c;

    void destroy_();
  };

  // member variables
  codebook_type_c_ codebook_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookConfig-r16 ::= SEQUENCE
struct codebook_cfg_r16_s {
  struct codebook_type_c_ {
    struct type2_s_ {
      struct sub_type_c_ {
        struct type_ii_r16_s_ {
          struct n1_n2_codebook_subset_restrict_r16_c_ {
            struct types_opts {
              enum options {
                two_one,
                two_two,
                four_one,
                three_two,
                six_one,
                four_two,
                eight_one,
                four_three,
                six_two,
                twelve_one,
                four_four,
                eight_two,
                sixteen_one,
                nulltype
              } value;

              const char* to_string() const;
            };
            using types = enumerated<types_opts>;

            // choice methods
            n1_n2_codebook_subset_restrict_r16_c_() = default;
            n1_n2_codebook_subset_restrict_r16_c_(const n1_n2_codebook_subset_restrict_r16_c_& other);
            n1_n2_codebook_subset_restrict_r16_c_& operator=(const n1_n2_codebook_subset_restrict_r16_c_& other);
            ~n1_n2_codebook_subset_restrict_r16_c_() { destroy_(); }
            void        set(types::options e = types::nulltype);
            types       type() const { return type_; }
            SRSASN_CODE pack(bit_ref& bref) const;
            SRSASN_CODE unpack(cbit_ref& bref);
            void        to_json(json_writer& j) const;
            // getters
            fixed_bitstring<16>& two_one()
            {
              assert_choice_type(types::two_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<16>>();
            }
            fixed_bitstring<43>& two_two()
            {
              assert_choice_type(types::two_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<43>>();
            }
            fixed_bitstring<32>& four_one()
            {
              assert_choice_type(types::four_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<32>>();
            }
            fixed_bitstring<59>& three_two()
            {
              assert_choice_type(types::three_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<59>>();
            }
            fixed_bitstring<48>& six_one()
            {
              assert_choice_type(types::six_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<48>>();
            }
            fixed_bitstring<75>& four_two()
            {
              assert_choice_type(types::four_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<75>>();
            }
            fixed_bitstring<64>& eight_one()
            {
              assert_choice_type(types::eight_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<64>>();
            }
            fixed_bitstring<107>& four_three()
            {
              assert_choice_type(types::four_three, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<107>>();
            }
            fixed_bitstring<107>& six_two()
            {
              assert_choice_type(types::six_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<107>>();
            }
            fixed_bitstring<96>& twelve_one()
            {
              assert_choice_type(types::twelve_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<96>>();
            }
            fixed_bitstring<139>& four_four()
            {
              assert_choice_type(types::four_four, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<139>>();
            }
            fixed_bitstring<139>& eight_two()
            {
              assert_choice_type(types::eight_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<139>>();
            }
            fixed_bitstring<128>& sixteen_one()
            {
              assert_choice_type(types::sixteen_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<128>>();
            }
            const fixed_bitstring<16>& two_one() const
            {
              assert_choice_type(types::two_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<16>>();
            }
            const fixed_bitstring<43>& two_two() const
            {
              assert_choice_type(types::two_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<43>>();
            }
            const fixed_bitstring<32>& four_one() const
            {
              assert_choice_type(types::four_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<32>>();
            }
            const fixed_bitstring<59>& three_two() const
            {
              assert_choice_type(types::three_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<59>>();
            }
            const fixed_bitstring<48>& six_one() const
            {
              assert_choice_type(types::six_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<48>>();
            }
            const fixed_bitstring<75>& four_two() const
            {
              assert_choice_type(types::four_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<75>>();
            }
            const fixed_bitstring<64>& eight_one() const
            {
              assert_choice_type(types::eight_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<64>>();
            }
            const fixed_bitstring<107>& four_three() const
            {
              assert_choice_type(types::four_three, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<107>>();
            }
            const fixed_bitstring<107>& six_two() const
            {
              assert_choice_type(types::six_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<107>>();
            }
            const fixed_bitstring<96>& twelve_one() const
            {
              assert_choice_type(types::twelve_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<96>>();
            }
            const fixed_bitstring<139>& four_four() const
            {
              assert_choice_type(types::four_four, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<139>>();
            }
            const fixed_bitstring<139>& eight_two() const
            {
              assert_choice_type(types::eight_two, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<139>>();
            }
            const fixed_bitstring<128>& sixteen_one() const
            {
              assert_choice_type(types::sixteen_one, type_, "n1-n2-codebookSubsetRestriction-r16");
              return c.get<fixed_bitstring<128>>();
            }
            fixed_bitstring<16>&  set_two_one();
            fixed_bitstring<43>&  set_two_two();
            fixed_bitstring<32>&  set_four_one();
            fixed_bitstring<59>&  set_three_two();
            fixed_bitstring<48>&  set_six_one();
            fixed_bitstring<75>&  set_four_two();
            fixed_bitstring<64>&  set_eight_one();
            fixed_bitstring<107>& set_four_three();
            fixed_bitstring<107>& set_six_two();
            fixed_bitstring<96>&  set_twelve_one();
            fixed_bitstring<139>& set_four_four();
            fixed_bitstring<139>& set_eight_two();
            fixed_bitstring<128>& set_sixteen_one();

          private:
            types                                 type_;
            choice_buffer_t<fixed_bitstring<139>> c;

            void destroy_();
          };

          // member variables
          n1_n2_codebook_subset_restrict_r16_c_ n1_n2_codebook_subset_restrict_r16;
          fixed_bitstring<4>                    type_ii_ri_restrict_r16;
        };
        struct type_ii_port_sel_r16_s_ {
          struct port_sel_sampling_size_r16_opts {
            enum options { n1, n2, n3, n4, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using port_sel_sampling_size_r16_e_ = enumerated<port_sel_sampling_size_r16_opts>;

          // member variables
          port_sel_sampling_size_r16_e_ port_sel_sampling_size_r16;
          fixed_bitstring<4>            type_ii_port_sel_ri_restrict_r16;
        };
        struct types_opts {
          enum options { type_ii_r16, type_ii_port_sel_r16, nulltype } value;

          const char* to_string() const;
        };
        using types = enumerated<types_opts>;

        // choice methods
        sub_type_c_() = default;
        sub_type_c_(const sub_type_c_& other);
        sub_type_c_& operator=(const sub_type_c_& other);
        ~sub_type_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        type_ii_r16_s_& type_ii_r16()
        {
          assert_choice_type(types::type_ii_r16, type_, "subType");
          return c.get<type_ii_r16_s_>();
        }
        type_ii_port_sel_r16_s_& type_ii_port_sel_r16()
        {
          assert_choice_type(types::type_ii_port_sel_r16, type_, "subType");
          return c.get<type_ii_port_sel_r16_s_>();
        }
        const type_ii_r16_s_& type_ii_r16() const
        {
          assert_choice_type(types::type_ii_r16, type_, "subType");
          return c.get<type_ii_r16_s_>();
        }
        const type_ii_port_sel_r16_s_& type_ii_port_sel_r16() const
        {
          assert_choice_type(types::type_ii_port_sel_r16, type_, "subType");
          return c.get<type_ii_port_sel_r16_s_>();
        }
        type_ii_r16_s_&          set_type_ii_r16();
        type_ii_port_sel_r16_s_& set_type_ii_port_sel_r16();

      private:
        types                                                    type_;
        choice_buffer_t<type_ii_port_sel_r16_s_, type_ii_r16_s_> c;

        void destroy_();
      };

      // member variables
      sub_type_c_ sub_type;
      uint8_t     nof_pmi_subbands_per_cqi_subband_r16 = 1;
      uint8_t     param_combination_r16                = 1;
    };
    struct types_opts {
      enum options { type2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::type2; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    type2_s_&       type2() { return c; }
    const type2_s_& type2() const { return c; }

  private:
    type2_s_ c;
  };

  // member variables
  codebook_type_c_ codebook_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookConfig-r17 ::= SEQUENCE
struct codebook_cfg_r17_s {
  struct codebook_type_c_ {
    struct type1_s_ {
      struct type_i_single_panel_group1_r17_s_ {
        struct nr_of_ant_ports_c_ {
          struct two_s_ {
            fixed_bitstring<6> two_tx_codebook_subset_restrict1_r17;
          };
          struct more_than_two_s_ {
            struct n1_n2_c_ {
              struct types_opts {
                enum options {
                  two_one_type_i_single_panel_restrict1_r17,
                  two_two_type_i_single_panel_restrict1_r17,
                  four_one_type_i_single_panel_restrict1_r17,
                  three_two_type_i_single_panel_restrict1_r17,
                  six_one_type_i_single_panel_restrict1_r17,
                  four_two_type_i_single_panel_restrict1_r17,
                  eight_one_type_i_single_panel_restrict1_r17,
                  four_three_type_i_single_panel_restrict1_r17,
                  six_two_type_i_single_panel_restrict1_r17,
                  twelve_one_type_i_single_panel_restrict1_r17,
                  four_four_type_i_single_panel_restrict1_r17,
                  eight_two_type_i_single_panel_restrict1_r17,
                  sixteen_one_type_i_single_panel_restrict1_r17,
                  nulltype
                } value;

                const char* to_string() const;
              };
              using types = enumerated<types_opts>;

              // choice methods
              n1_n2_c_() = default;
              n1_n2_c_(const n1_n2_c_& other);
              n1_n2_c_& operator=(const n1_n2_c_& other);
              ~n1_n2_c_() { destroy_(); }
              void        set(types::options e = types::nulltype);
              types       type() const { return type_; }
              SRSASN_CODE pack(bit_ref& bref) const;
              SRSASN_CODE unpack(cbit_ref& bref);
              void        to_json(json_writer& j) const;
              // getters
              fixed_bitstring<8>& two_one_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::two_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<8>>();
              }
              fixed_bitstring<64>& two_two_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::two_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<64>>();
              }
              fixed_bitstring<16>& four_one_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::four_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<16>>();
              }
              fixed_bitstring<96>& three_two_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::three_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<96>>();
              }
              fixed_bitstring<24>& six_one_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::six_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<24>>();
              }
              fixed_bitstring<128>& four_two_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::four_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<128>>();
              }
              fixed_bitstring<32>& eight_one_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::eight_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<32>>();
              }
              fixed_bitstring<192>& four_three_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::four_three_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<192>>();
              }
              fixed_bitstring<192>& six_two_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::six_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<192>>();
              }
              fixed_bitstring<48>& twelve_one_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::twelve_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<48>>();
              }
              fixed_bitstring<256>& four_four_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::four_four_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<256>>();
              }
              fixed_bitstring<256>& eight_two_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::eight_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<256>>();
              }
              fixed_bitstring<64>& sixteen_one_type_i_single_panel_restrict1_r17()
              {
                assert_choice_type(types::sixteen_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<64>>();
              }
              const fixed_bitstring<8>& two_one_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::two_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<8>>();
              }
              const fixed_bitstring<64>& two_two_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::two_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<64>>();
              }
              const fixed_bitstring<16>& four_one_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::four_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<16>>();
              }
              const fixed_bitstring<96>& three_two_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::three_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<96>>();
              }
              const fixed_bitstring<24>& six_one_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::six_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<24>>();
              }
              const fixed_bitstring<128>& four_two_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::four_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<128>>();
              }
              const fixed_bitstring<32>& eight_one_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::eight_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<32>>();
              }
              const fixed_bitstring<192>& four_three_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::four_three_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<192>>();
              }
              const fixed_bitstring<192>& six_two_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::six_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<192>>();
              }
              const fixed_bitstring<48>& twelve_one_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::twelve_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<48>>();
              }
              const fixed_bitstring<256>& four_four_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::four_four_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<256>>();
              }
              const fixed_bitstring<256>& eight_two_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::eight_two_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<256>>();
              }
              const fixed_bitstring<64>& sixteen_one_type_i_single_panel_restrict1_r17() const
              {
                assert_choice_type(types::sixteen_one_type_i_single_panel_restrict1_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<64>>();
              }
              fixed_bitstring<8>&   set_two_one_type_i_single_panel_restrict1_r17();
              fixed_bitstring<64>&  set_two_two_type_i_single_panel_restrict1_r17();
              fixed_bitstring<16>&  set_four_one_type_i_single_panel_restrict1_r17();
              fixed_bitstring<96>&  set_three_two_type_i_single_panel_restrict1_r17();
              fixed_bitstring<24>&  set_six_one_type_i_single_panel_restrict1_r17();
              fixed_bitstring<128>& set_four_two_type_i_single_panel_restrict1_r17();
              fixed_bitstring<32>&  set_eight_one_type_i_single_panel_restrict1_r17();
              fixed_bitstring<192>& set_four_three_type_i_single_panel_restrict1_r17();
              fixed_bitstring<192>& set_six_two_type_i_single_panel_restrict1_r17();
              fixed_bitstring<48>&  set_twelve_one_type_i_single_panel_restrict1_r17();
              fixed_bitstring<256>& set_four_four_type_i_single_panel_restrict1_r17();
              fixed_bitstring<256>& set_eight_two_type_i_single_panel_restrict1_r17();
              fixed_bitstring<64>&  set_sixteen_one_type_i_single_panel_restrict1_r17();

            private:
              types                                 type_;
              choice_buffer_t<fixed_bitstring<256>> c;

              void destroy_();
            };

            // member variables
            n1_n2_c_ n1_n2;
          };
          struct types_opts {
            enum options { two, more_than_two, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using types = enumerated<types_opts>;

          // choice methods
          nr_of_ant_ports_c_() = default;
          nr_of_ant_ports_c_(const nr_of_ant_ports_c_& other);
          nr_of_ant_ports_c_& operator=(const nr_of_ant_ports_c_& other);
          ~nr_of_ant_ports_c_() { destroy_(); }
          void        set(types::options e = types::nulltype);
          types       type() const { return type_; }
          SRSASN_CODE pack(bit_ref& bref) const;
          SRSASN_CODE unpack(cbit_ref& bref);
          void        to_json(json_writer& j) const;
          // getters
          two_s_& two()
          {
            assert_choice_type(types::two, type_, "nrOfAntennaPorts");
            return c.get<two_s_>();
          }
          more_than_two_s_& more_than_two()
          {
            assert_choice_type(types::more_than_two, type_, "nrOfAntennaPorts");
            return c.get<more_than_two_s_>();
          }
          const two_s_& two() const
          {
            assert_choice_type(types::two, type_, "nrOfAntennaPorts");
            return c.get<two_s_>();
          }
          const more_than_two_s_& more_than_two() const
          {
            assert_choice_type(types::more_than_two, type_, "nrOfAntennaPorts");
            return c.get<more_than_two_s_>();
          }
          two_s_&           set_two();
          more_than_two_s_& set_more_than_two();

        private:
          types                                     type_;
          choice_buffer_t<more_than_two_s_, two_s_> c;

          void destroy_();
        };

        // member variables
        nr_of_ant_ports_c_ nr_of_ant_ports;
      };
      struct type_i_single_panel_group2_r17_s_ {
        struct nr_of_ant_ports_c_ {
          struct two_s_ {
            fixed_bitstring<6> two_tx_codebook_subset_restrict2_r17;
          };
          struct more_than_two_s_ {
            struct n1_n2_c_ {
              struct types_opts {
                enum options {
                  two_one_type_i_single_panel_restrict2_r17,
                  two_two_type_i_single_panel_restrict2_r17,
                  four_one_type_i_single_panel_restrict2_r17,
                  three_two_type_i_single_panel_restrict2_r17,
                  six_one_type_i_single_panel_restrict2_r17,
                  four_two_type_i_single_panel_restrict2_r17,
                  eight_one_type_i_single_panel_restrict2_r17,
                  four_three_type_i_single_panel_restrict2_r17,
                  six_two_type_i_single_panel_restrict2_r17,
                  twelve_one_type_i_single_panel_restrict2_r17,
                  four_four_type_i_single_panel_restrict2_r17,
                  eight_two_type_i_single_panel_restrict2_r17,
                  sixteen_one_type_i_single_panel_restrict2_r17,
                  nulltype
                } value;

                const char* to_string() const;
              };
              using types = enumerated<types_opts>;

              // choice methods
              n1_n2_c_() = default;
              n1_n2_c_(const n1_n2_c_& other);
              n1_n2_c_& operator=(const n1_n2_c_& other);
              ~n1_n2_c_() { destroy_(); }
              void        set(types::options e = types::nulltype);
              types       type() const { return type_; }
              SRSASN_CODE pack(bit_ref& bref) const;
              SRSASN_CODE unpack(cbit_ref& bref);
              void        to_json(json_writer& j) const;
              // getters
              fixed_bitstring<8>& two_one_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::two_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<8>>();
              }
              fixed_bitstring<64>& two_two_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::two_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<64>>();
              }
              fixed_bitstring<16>& four_one_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::four_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<16>>();
              }
              fixed_bitstring<96>& three_two_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::three_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<96>>();
              }
              fixed_bitstring<24>& six_one_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::six_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<24>>();
              }
              fixed_bitstring<128>& four_two_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::four_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<128>>();
              }
              fixed_bitstring<32>& eight_one_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::eight_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<32>>();
              }
              fixed_bitstring<192>& four_three_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::four_three_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<192>>();
              }
              fixed_bitstring<192>& six_two_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::six_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<192>>();
              }
              fixed_bitstring<48>& twelve_one_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::twelve_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<48>>();
              }
              fixed_bitstring<256>& four_four_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::four_four_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<256>>();
              }
              fixed_bitstring<256>& eight_two_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::eight_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<256>>();
              }
              fixed_bitstring<64>& sixteen_one_type_i_single_panel_restrict2_r17()
              {
                assert_choice_type(types::sixteen_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<64>>();
              }
              const fixed_bitstring<8>& two_one_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::two_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<8>>();
              }
              const fixed_bitstring<64>& two_two_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::two_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<64>>();
              }
              const fixed_bitstring<16>& four_one_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::four_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<16>>();
              }
              const fixed_bitstring<96>& three_two_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::three_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<96>>();
              }
              const fixed_bitstring<24>& six_one_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::six_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<24>>();
              }
              const fixed_bitstring<128>& four_two_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::four_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<128>>();
              }
              const fixed_bitstring<32>& eight_one_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::eight_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<32>>();
              }
              const fixed_bitstring<192>& four_three_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::four_three_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<192>>();
              }
              const fixed_bitstring<192>& six_two_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::six_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<192>>();
              }
              const fixed_bitstring<48>& twelve_one_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::twelve_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<48>>();
              }
              const fixed_bitstring<256>& four_four_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::four_four_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<256>>();
              }
              const fixed_bitstring<256>& eight_two_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::eight_two_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<256>>();
              }
              const fixed_bitstring<64>& sixteen_one_type_i_single_panel_restrict2_r17() const
              {
                assert_choice_type(types::sixteen_one_type_i_single_panel_restrict2_r17, type_, "n1-n2");
                return c.get<fixed_bitstring<64>>();
              }
              fixed_bitstring<8>&   set_two_one_type_i_single_panel_restrict2_r17();
              fixed_bitstring<64>&  set_two_two_type_i_single_panel_restrict2_r17();
              fixed_bitstring<16>&  set_four_one_type_i_single_panel_restrict2_r17();
              fixed_bitstring<96>&  set_three_two_type_i_single_panel_restrict2_r17();
              fixed_bitstring<24>&  set_six_one_type_i_single_panel_restrict2_r17();
              fixed_bitstring<128>& set_four_two_type_i_single_panel_restrict2_r17();
              fixed_bitstring<32>&  set_eight_one_type_i_single_panel_restrict2_r17();
              fixed_bitstring<192>& set_four_three_type_i_single_panel_restrict2_r17();
              fixed_bitstring<192>& set_six_two_type_i_single_panel_restrict2_r17();
              fixed_bitstring<48>&  set_twelve_one_type_i_single_panel_restrict2_r17();
              fixed_bitstring<256>& set_four_four_type_i_single_panel_restrict2_r17();
              fixed_bitstring<256>& set_eight_two_type_i_single_panel_restrict2_r17();
              fixed_bitstring<64>&  set_sixteen_one_type_i_single_panel_restrict2_r17();

            private:
              types                                 type_;
              choice_buffer_t<fixed_bitstring<256>> c;

              void destroy_();
            };

            // member variables
            n1_n2_c_ n1_n2;
          };
          struct types_opts {
            enum options { two, more_than_two, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          using types = enumerated<types_opts>;

          // choice methods
          nr_of_ant_ports_c_() = default;
          nr_of_ant_ports_c_(const nr_of_ant_ports_c_& other);
          nr_of_ant_ports_c_& operator=(const nr_of_ant_ports_c_& other);
          ~nr_of_ant_ports_c_() { destroy_(); }
          void        set(types::options e = types::nulltype);
          types       type() const { return type_; }
          SRSASN_CODE pack(bit_ref& bref) const;
          SRSASN_CODE unpack(cbit_ref& bref);
          void        to_json(json_writer& j) const;
          // getters
          two_s_& two()
          {
            assert_choice_type(types::two, type_, "nrOfAntennaPorts");
            return c.get<two_s_>();
          }
          more_than_two_s_& more_than_two()
          {
            assert_choice_type(types::more_than_two, type_, "nrOfAntennaPorts");
            return c.get<more_than_two_s_>();
          }
          const two_s_& two() const
          {
            assert_choice_type(types::two, type_, "nrOfAntennaPorts");
            return c.get<two_s_>();
          }
          const more_than_two_s_& more_than_two() const
          {
            assert_choice_type(types::more_than_two, type_, "nrOfAntennaPorts");
            return c.get<more_than_two_s_>();
          }
          two_s_&           set_two();
          more_than_two_s_& set_more_than_two();

        private:
          types                                     type_;
          choice_buffer_t<more_than_two_s_, two_s_> c;

          void destroy_();
        };

        // member variables
        nr_of_ant_ports_c_ nr_of_ant_ports;
      };

      // member variables
      bool                              type_i_single_panel_group1_r17_present           = false;
      bool                              type_i_single_panel_group2_r17_present           = false;
      bool                              type_i_single_panel_ri_restrict_strp_r17_present = false;
      bool                              type_i_single_panel_ri_restrict_sdm_r17_present  = false;
      type_i_single_panel_group1_r17_s_ type_i_single_panel_group1_r17;
      type_i_single_panel_group2_r17_s_ type_i_single_panel_group2_r17;
      fixed_bitstring<8>                type_i_single_panel_ri_restrict_strp_r17;
      fixed_bitstring<4>                type_i_single_panel_ri_restrict_sdm_r17;
    };
    struct type2_s_ {
      struct type_ii_port_sel_r17_s_ {
        struct value_of_n_r17_opts {
          enum options { n2, n4, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        using value_of_n_r17_e_ = enumerated<value_of_n_r17_opts>;

        // member variables
        bool               value_of_n_r17_present                       = false;
        bool               nof_pmi_subbands_per_cqi_subband_r17_present = false;
        uint8_t            param_combination_r17                        = 1;
        value_of_n_r17_e_  value_of_n_r17;
        uint8_t            nof_pmi_subbands_per_cqi_subband_r17 = 1;
        fixed_bitstring<4> type_ii_port_sel_ri_restrict_r17;
      };

      // member variables
      type_ii_port_sel_r17_s_ type_ii_port_sel_r17;
    };
    struct types_opts {
      enum options { type1, type2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    codebook_type_c_() = default;
    codebook_type_c_(const codebook_type_c_& other);
    codebook_type_c_& operator=(const codebook_type_c_& other);
    ~codebook_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    type1_s_& type1()
    {
      assert_choice_type(types::type1, type_, "codebookType");
      return c.get<type1_s_>();
    }
    type2_s_& type2()
    {
      assert_choice_type(types::type2, type_, "codebookType");
      return c.get<type2_s_>();
    }
    const type1_s_& type1() const
    {
      assert_choice_type(types::type1, type_, "codebookType");
      return c.get<type1_s_>();
    }
    const type2_s_& type2() const
    {
      assert_choice_type(types::type2, type_, "codebookType");
      return c.get<type2_s_>();
    }
    type1_s_& set_type1();
    type2_s_& set_type2();

  private:
    types                               type_;
    choice_buffer_t<type1_s_, type2_s_> c;

    void destroy_();
  };

  // member variables
  codebook_type_c_ codebook_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CodebookConfig-v1730 ::= SEQUENCE
struct codebook_cfg_v1730_s {
  struct codebook_type_c_ {
    struct type1_s_ {
      bool    codebook_mode_present = false;
      uint8_t codebook_mode         = 1;
    };
    struct types_opts {
      enum options { type1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::type1; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    type1_s_&       type1() { return c; }
    const type1_s_& type1() const { return c; }

  private:
    type1_s_ c;
  };

  // member variables
  codebook_type_c_ codebook_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-CSI-Resource ::= SEQUENCE
struct pucch_csi_res_s {
  uint8_t ul_bw_part_id = 0;
  uint8_t pucch_res     = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PortIndexFor8Ranks ::= CHOICE
struct port_idx_for8_ranks_c {
  struct port_idx8_s_ {
    using rank2_8_l_ = std::array<uint8_t, 2>;
    using rank3_8_l_ = std::array<uint8_t, 3>;
    using rank4_8_l_ = std::array<uint8_t, 4>;
    using rank5_8_l_ = std::array<uint8_t, 5>;
    using rank6_8_l_ = std::array<uint8_t, 6>;
    using rank7_8_l_ = std::array<uint8_t, 7>;
    using rank8_8_l_ = std::array<uint8_t, 8>;

    // member variables
    bool       rank1_8_present = false;
    bool       rank2_8_present = false;
    bool       rank3_8_present = false;
    bool       rank4_8_present = false;
    bool       rank5_8_present = false;
    bool       rank6_8_present = false;
    bool       rank7_8_present = false;
    bool       rank8_8_present = false;
    uint8_t    rank1_8         = 0;
    rank2_8_l_ rank2_8;
    rank3_8_l_ rank3_8;
    rank4_8_l_ rank4_8;
    rank5_8_l_ rank5_8;
    rank6_8_l_ rank6_8;
    rank7_8_l_ rank7_8;
    rank8_8_l_ rank8_8;
  };
  struct port_idx4_s_ {
    using rank2_4_l_ = std::array<uint8_t, 2>;
    using rank3_4_l_ = std::array<uint8_t, 3>;
    using rank4_4_l_ = std::array<uint8_t, 4>;

    // member variables
    bool       rank1_4_present = false;
    bool       rank2_4_present = false;
    bool       rank3_4_present = false;
    bool       rank4_4_present = false;
    uint8_t    rank1_4         = 0;
    rank2_4_l_ rank2_4;
    rank3_4_l_ rank3_4;
    rank4_4_l_ rank4_4;
  };
  struct port_idx2_s_ {
    using rank2_2_l_ = std::array<uint8_t, 2>;

    // member variables
    bool       rank1_2_present = false;
    bool       rank2_2_present = false;
    uint8_t    rank1_2         = 0;
    rank2_2_l_ rank2_2;
  };
  struct types_opts {
    enum options { port_idx8, port_idx4, port_idx2, port_idx1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  port_idx_for8_ranks_c() = default;
  port_idx_for8_ranks_c(const port_idx_for8_ranks_c& other);
  port_idx_for8_ranks_c& operator=(const port_idx_for8_ranks_c& other);
  ~port_idx_for8_ranks_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  port_idx8_s_& port_idx8()
  {
    assert_choice_type(types::port_idx8, type_, "PortIndexFor8Ranks");
    return c.get<port_idx8_s_>();
  }
  port_idx4_s_& port_idx4()
  {
    assert_choice_type(types::port_idx4, type_, "PortIndexFor8Ranks");
    return c.get<port_idx4_s_>();
  }
  port_idx2_s_& port_idx2()
  {
    assert_choice_type(types::port_idx2, type_, "PortIndexFor8Ranks");
    return c.get<port_idx2_s_>();
  }
  const port_idx8_s_& port_idx8() const
  {
    assert_choice_type(types::port_idx8, type_, "PortIndexFor8Ranks");
    return c.get<port_idx8_s_>();
  }
  const port_idx4_s_& port_idx4() const
  {
    assert_choice_type(types::port_idx4, type_, "PortIndexFor8Ranks");
    return c.get<port_idx4_s_>();
  }
  const port_idx2_s_& port_idx2() const
  {
    assert_choice_type(types::port_idx2, type_, "PortIndexFor8Ranks");
    return c.get<port_idx2_s_>();
  }
  port_idx8_s_& set_port_idx8();
  port_idx4_s_& set_port_idx4();
  port_idx2_s_& set_port_idx2();
  void          set_port_idx1();

private:
  types                                                     type_;
  choice_buffer_t<port_idx2_s_, port_idx4_s_, port_idx8_s_> c;

  void destroy_();
};

// CSI-ReportConfig ::= SEQUENCE
struct csi_report_cfg_s {
  struct report_cfg_type_c_ {
    struct periodic_s_ {
      using pucch_csi_res_list_l_ = dyn_array<pucch_csi_res_s>;

      // member variables
      csi_report_periodicity_and_offset_c report_slot_cfg;
      pucch_csi_res_list_l_               pucch_csi_res_list;
    };
    struct semi_persistent_on_pucch_s_ {
      using pucch_csi_res_list_l_ = dyn_array<pucch_csi_res_s>;

      // member variables
      csi_report_periodicity_and_offset_c report_slot_cfg;
      pucch_csi_res_list_l_               pucch_csi_res_list;
    };
    struct semi_persistent_on_pusch_s_ {
      struct report_slot_cfg_opts {
        enum options { sl5, sl10, sl20, sl40, sl80, sl160, sl320, nulltype } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      using report_slot_cfg_e_         = enumerated<report_slot_cfg_opts>;
      using report_slot_offset_list_l_ = bounded_array<uint8_t, 16>;

      // member variables
      report_slot_cfg_e_         report_slot_cfg;
      report_slot_offset_list_l_ report_slot_offset_list;
      uint8_t                    p0alpha = 0;
    };
    struct aperiodic_s_ {
      using report_slot_offset_list_l_ = bounded_array<uint8_t, 16>;

      // member variables
      report_slot_offset_list_l_ report_slot_offset_list;
    };
    struct types_opts {
      enum options { periodic, semi_persistent_on_pucch, semi_persistent_on_pusch, aperiodic, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    report_cfg_type_c_() = default;
    report_cfg_type_c_(const report_cfg_type_c_& other);
    report_cfg_type_c_& operator=(const report_cfg_type_c_& other);
    ~report_cfg_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    periodic_s_& periodic()
    {
      assert_choice_type(types::periodic, type_, "reportConfigType");
      return c.get<periodic_s_>();
    }
    semi_persistent_on_pucch_s_& semi_persistent_on_pucch()
    {
      assert_choice_type(types::semi_persistent_on_pucch, type_, "reportConfigType");
      return c.get<semi_persistent_on_pucch_s_>();
    }
    semi_persistent_on_pusch_s_& semi_persistent_on_pusch()
    {
      assert_choice_type(types::semi_persistent_on_pusch, type_, "reportConfigType");
      return c.get<semi_persistent_on_pusch_s_>();
    }
    aperiodic_s_& aperiodic()
    {
      assert_choice_type(types::aperiodic, type_, "reportConfigType");
      return c.get<aperiodic_s_>();
    }
    const periodic_s_& periodic() const
    {
      assert_choice_type(types::periodic, type_, "reportConfigType");
      return c.get<periodic_s_>();
    }
    const semi_persistent_on_pucch_s_& semi_persistent_on_pucch() const
    {
      assert_choice_type(types::semi_persistent_on_pucch, type_, "reportConfigType");
      return c.get<semi_persistent_on_pucch_s_>();
    }
    const semi_persistent_on_pusch_s_& semi_persistent_on_pusch() const
    {
      assert_choice_type(types::semi_persistent_on_pusch, type_, "reportConfigType");
      return c.get<semi_persistent_on_pusch_s_>();
    }
    const aperiodic_s_& aperiodic() const
    {
      assert_choice_type(types::aperiodic, type_, "reportConfigType");
      return c.get<aperiodic_s_>();
    }
    periodic_s_&                 set_periodic();
    semi_persistent_on_pucch_s_& set_semi_persistent_on_pucch();
    semi_persistent_on_pusch_s_& set_semi_persistent_on_pusch();
    aperiodic_s_&                set_aperiodic();

  private:
    types                                                                                                type_;
    choice_buffer_t<aperiodic_s_, periodic_s_, semi_persistent_on_pucch_s_, semi_persistent_on_pusch_s_> c;

    void destroy_();
  };
  struct report_quant_c_ {
    struct cri_ri_i1_cqi_s_ {
      struct pdsch_bundle_size_for_csi_opts {
        enum options { n2, n4, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using pdsch_bundle_size_for_csi_e_ = enumerated<pdsch_bundle_size_for_csi_opts>;

      // member variables
      bool                         pdsch_bundle_size_for_csi_present = false;
      pdsch_bundle_size_for_csi_e_ pdsch_bundle_size_for_csi;
    };
    struct types_opts {
      enum options {
        none,
        cri_ri_pmi_cqi,
        cri_ri_i1,
        cri_ri_i1_cqi,
        cri_ri_cqi,
        cri_rsrp,
        ssb_idx_rsrp,
        cri_ri_li_pmi_cqi,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    report_quant_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cri_ri_i1_cqi_s_& cri_ri_i1_cqi()
    {
      assert_choice_type(types::cri_ri_i1_cqi, type_, "reportQuantity");
      return c;
    }
    const cri_ri_i1_cqi_s_& cri_ri_i1_cqi() const
    {
      assert_choice_type(types::cri_ri_i1_cqi, type_, "reportQuantity");
      return c;
    }
    void              set_none();
    void              set_cri_ri_pmi_cqi();
    void              set_cri_ri_i1();
    cri_ri_i1_cqi_s_& set_cri_ri_i1_cqi();
    void              set_cri_ri_cqi();
    void              set_cri_rsrp();
    void              set_ssb_idx_rsrp();
    void              set_cri_ri_li_pmi_cqi();

  private:
    types            type_;
    cri_ri_i1_cqi_s_ c;
  };
  struct report_freq_cfg_s_ {
    struct cqi_format_ind_opts {
      enum options { wideband_cqi, subband_cqi, nulltype } value;

      const char* to_string() const;
    };
    using cqi_format_ind_e_ = enumerated<cqi_format_ind_opts>;
    struct pmi_format_ind_opts {
      enum options { wideband_pmi, subband_pmi, nulltype } value;

      const char* to_string() const;
    };
    using pmi_format_ind_e_ = enumerated<pmi_format_ind_opts>;
    struct csi_report_band_c_ {
      struct types_opts {
        enum options {
          subbands3,
          subbands4,
          subbands5,
          subbands6,
          subbands7,
          subbands8,
          subbands9,
          subbands10,
          subbands11,
          subbands12,
          subbands13,
          subbands14,
          subbands15,
          subbands16,
          subbands17,
          subbands18,
          // ...
          subbands19_v1530,
          nulltype
        } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using types = enumerated<types_opts, true, 1>;

      // choice methods
      csi_report_band_c_() = default;
      csi_report_band_c_(const csi_report_band_c_& other);
      csi_report_band_c_& operator=(const csi_report_band_c_& other);
      ~csi_report_band_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      fixed_bitstring<3>& subbands3()
      {
        assert_choice_type(types::subbands3, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<3>>();
      }
      fixed_bitstring<4>& subbands4()
      {
        assert_choice_type(types::subbands4, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<4>>();
      }
      fixed_bitstring<5>& subbands5()
      {
        assert_choice_type(types::subbands5, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<5>>();
      }
      fixed_bitstring<6>& subbands6()
      {
        assert_choice_type(types::subbands6, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<6>>();
      }
      fixed_bitstring<7>& subbands7()
      {
        assert_choice_type(types::subbands7, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<7>>();
      }
      fixed_bitstring<8>& subbands8()
      {
        assert_choice_type(types::subbands8, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<8>>();
      }
      fixed_bitstring<9>& subbands9()
      {
        assert_choice_type(types::subbands9, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<9>>();
      }
      fixed_bitstring<10>& subbands10()
      {
        assert_choice_type(types::subbands10, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<10>>();
      }
      fixed_bitstring<11>& subbands11()
      {
        assert_choice_type(types::subbands11, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<11>>();
      }
      fixed_bitstring<12>& subbands12()
      {
        assert_choice_type(types::subbands12, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<12>>();
      }
      fixed_bitstring<13>& subbands13()
      {
        assert_choice_type(types::subbands13, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<13>>();
      }
      fixed_bitstring<14>& subbands14()
      {
        assert_choice_type(types::subbands14, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<14>>();
      }
      fixed_bitstring<15>& subbands15()
      {
        assert_choice_type(types::subbands15, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<15>>();
      }
      fixed_bitstring<16>& subbands16()
      {
        assert_choice_type(types::subbands16, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<16>>();
      }
      fixed_bitstring<17>& subbands17()
      {
        assert_choice_type(types::subbands17, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<17>>();
      }
      fixed_bitstring<18>& subbands18()
      {
        assert_choice_type(types::subbands18, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<18>>();
      }
      fixed_bitstring<19>& subbands19_v1530()
      {
        assert_choice_type(types::subbands19_v1530, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<19>>();
      }
      const fixed_bitstring<3>& subbands3() const
      {
        assert_choice_type(types::subbands3, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<3>>();
      }
      const fixed_bitstring<4>& subbands4() const
      {
        assert_choice_type(types::subbands4, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<4>>();
      }
      const fixed_bitstring<5>& subbands5() const
      {
        assert_choice_type(types::subbands5, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<5>>();
      }
      const fixed_bitstring<6>& subbands6() const
      {
        assert_choice_type(types::subbands6, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<6>>();
      }
      const fixed_bitstring<7>& subbands7() const
      {
        assert_choice_type(types::subbands7, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<7>>();
      }
      const fixed_bitstring<8>& subbands8() const
      {
        assert_choice_type(types::subbands8, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<8>>();
      }
      const fixed_bitstring<9>& subbands9() const
      {
        assert_choice_type(types::subbands9, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<9>>();
      }
      const fixed_bitstring<10>& subbands10() const
      {
        assert_choice_type(types::subbands10, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<10>>();
      }
      const fixed_bitstring<11>& subbands11() const
      {
        assert_choice_type(types::subbands11, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<11>>();
      }
      const fixed_bitstring<12>& subbands12() const
      {
        assert_choice_type(types::subbands12, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<12>>();
      }
      const fixed_bitstring<13>& subbands13() const
      {
        assert_choice_type(types::subbands13, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<13>>();
      }
      const fixed_bitstring<14>& subbands14() const
      {
        assert_choice_type(types::subbands14, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<14>>();
      }
      const fixed_bitstring<15>& subbands15() const
      {
        assert_choice_type(types::subbands15, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<15>>();
      }
      const fixed_bitstring<16>& subbands16() const
      {
        assert_choice_type(types::subbands16, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<16>>();
      }
      const fixed_bitstring<17>& subbands17() const
      {
        assert_choice_type(types::subbands17, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<17>>();
      }
      const fixed_bitstring<18>& subbands18() const
      {
        assert_choice_type(types::subbands18, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<18>>();
      }
      const fixed_bitstring<19>& subbands19_v1530() const
      {
        assert_choice_type(types::subbands19_v1530, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<19>>();
      }
      fixed_bitstring<3>&  set_subbands3();
      fixed_bitstring<4>&  set_subbands4();
      fixed_bitstring<5>&  set_subbands5();
      fixed_bitstring<6>&  set_subbands6();
      fixed_bitstring<7>&  set_subbands7();
      fixed_bitstring<8>&  set_subbands8();
      fixed_bitstring<9>&  set_subbands9();
      fixed_bitstring<10>& set_subbands10();
      fixed_bitstring<11>& set_subbands11();
      fixed_bitstring<12>& set_subbands12();
      fixed_bitstring<13>& set_subbands13();
      fixed_bitstring<14>& set_subbands14();
      fixed_bitstring<15>& set_subbands15();
      fixed_bitstring<16>& set_subbands16();
      fixed_bitstring<17>& set_subbands17();
      fixed_bitstring<18>& set_subbands18();
      fixed_bitstring<19>& set_subbands19_v1530();

    private:
      types                                type_;
      choice_buffer_t<fixed_bitstring<19>> c;

      void destroy_();
    };

    // member variables
    bool               cqi_format_ind_present  = false;
    bool               pmi_format_ind_present  = false;
    bool               csi_report_band_present = false;
    cqi_format_ind_e_  cqi_format_ind;
    pmi_format_ind_e_  pmi_format_ind;
    csi_report_band_c_ csi_report_band;
  };
  struct time_restrict_for_ch_meass_opts {
    enum options { cfg, not_cfg, nulltype } value;

    const char* to_string() const;
  };
  using time_restrict_for_ch_meass_e_ = enumerated<time_restrict_for_ch_meass_opts>;
  struct time_restrict_for_interference_meass_opts {
    enum options { cfg, not_cfg, nulltype } value;

    const char* to_string() const;
  };
  using time_restrict_for_interference_meass_e_ = enumerated<time_restrict_for_interference_meass_opts>;
  struct dummy_opts {
    enum options { n1, n2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using dummy_e_ = enumerated<dummy_opts>;
  struct group_based_beam_report_c_ {
    struct disabled_s_ {
      struct nrof_reported_rs_opts {
        enum options { n1, n2, n3, n4, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using nrof_reported_rs_e_ = enumerated<nrof_reported_rs_opts>;

      // member variables
      bool                nrof_reported_rs_present = false;
      nrof_reported_rs_e_ nrof_reported_rs;
    };
    struct types_opts {
      enum options { enabled, disabled, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    group_based_beam_report_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    disabled_s_& disabled()
    {
      assert_choice_type(types::disabled, type_, "groupBasedBeamReporting");
      return c;
    }
    const disabled_s_& disabled() const
    {
      assert_choice_type(types::disabled, type_, "groupBasedBeamReporting");
      return c;
    }
    void         set_enabled();
    disabled_s_& set_disabled();

  private:
    types       type_;
    disabled_s_ c;
  };
  struct cqi_table_opts {
    enum options { table1, table2, table3, table4_r17, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using cqi_table_e_ = enumerated<cqi_table_opts>;
  struct subband_size_opts {
    enum options { value1, value2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using subband_size_e_     = enumerated<subband_size_opts>;
  using non_pmi_port_ind_l_ = dyn_array<port_idx_for8_ranks_c>;
  struct semi_persistent_on_pusch_v1530_s_ {
    struct report_slot_cfg_v1530_opts {
      enum options { sl4, sl8, sl16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using report_slot_cfg_v1530_e_ = enumerated<report_slot_cfg_v1530_opts>;

    // member variables
    report_slot_cfg_v1530_e_ report_slot_cfg_v1530;
  };
  struct semi_persistent_on_pusch_v1610_s_ {
    using report_slot_offset_list_dci_0_2_r16_l_ = dyn_array<uint8_t>;
    using report_slot_offset_list_dci_0_1_r16_l_ = dyn_array<uint8_t>;

    // member variables
    report_slot_offset_list_dci_0_2_r16_l_ report_slot_offset_list_dci_0_2_r16;
    report_slot_offset_list_dci_0_1_r16_l_ report_slot_offset_list_dci_0_1_r16;
  };
  struct aperiodic_v1610_s_ {
    using report_slot_offset_list_dci_0_2_r16_l_ = dyn_array<uint8_t>;
    using report_slot_offset_list_dci_0_1_r16_l_ = dyn_array<uint8_t>;

    // member variables
    report_slot_offset_list_dci_0_2_r16_l_ report_slot_offset_list_dci_0_2_r16;
    report_slot_offset_list_dci_0_1_r16_l_ report_slot_offset_list_dci_0_1_r16;
  };
  struct report_quant_r16_c_ {
    struct types_opts {
      enum options { cri_sinr_r16, ssb_idx_sinr_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    report_quant_r16_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    void set_cri_sinr_r16();
    void set_ssb_idx_sinr_r16();

  private:
    types type_;
  };
  struct group_based_beam_report_v1710_s_ {
    struct nrof_reported_groups_r17_opts {
      enum options { n1, n2, n3, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using nrof_reported_groups_r17_e_ = enumerated<nrof_reported_groups_r17_opts>;

    // member variables
    nrof_reported_groups_r17_e_ nrof_reported_groups_r17;
  };
  struct csi_report_mode_r17_opts {
    enum options { mode1, mode2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using csi_report_mode_r17_e_ = enumerated<csi_report_mode_r17_opts>;
  struct nof_single_trp_csi_mode1_r17_opts {
    enum options { n0, n1, n2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nof_single_trp_csi_mode1_r17_e_ = enumerated<nof_single_trp_csi_mode1_r17_opts>;
  struct report_quant_r17_c_ {
    struct types_opts {
      enum options { cri_rsrp_idx_r17, ssb_idx_rsrp_idx_r17, cri_sinr_idx_r17, ssb_idx_sinr_idx_r17, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    report_quant_r17_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    void set_cri_rsrp_idx_r17();
    void set_ssb_idx_rsrp_idx_r17();
    void set_cri_sinr_idx_r17();
    void set_ssb_idx_sinr_idx_r17();

  private:
    types type_;
  };
  struct semi_persistent_on_pusch_v1720_s_ {
    using report_slot_offset_list_r17_l_         = dyn_array<uint8_t>;
    using report_slot_offset_list_dci_0_2_r17_l_ = dyn_array<uint8_t>;
    using report_slot_offset_list_dci_0_1_r17_l_ = dyn_array<uint8_t>;

    // member variables
    report_slot_offset_list_r17_l_         report_slot_offset_list_r17;
    report_slot_offset_list_dci_0_2_r17_l_ report_slot_offset_list_dci_0_2_r17;
    report_slot_offset_list_dci_0_1_r17_l_ report_slot_offset_list_dci_0_1_r17;
  };
  struct aperiodic_v1720_s_ {
    using report_slot_offset_list_r17_l_         = dyn_array<uint8_t>;
    using report_slot_offset_list_dci_0_2_r17_l_ = dyn_array<uint8_t>;
    using report_slot_offset_list_dci_0_1_r17_l_ = dyn_array<uint8_t>;

    // member variables
    report_slot_offset_list_r17_l_         report_slot_offset_list_r17;
    report_slot_offset_list_dci_0_2_r17_l_ report_slot_offset_list_dci_0_2_r17;
    report_slot_offset_list_dci_0_1_r17_l_ report_slot_offset_list_dci_0_1_r17;
  };

  // member variables
  bool                                    ext                                     = false;
  bool                                    carrier_present                         = false;
  bool                                    csi_im_res_for_interference_present     = false;
  bool                                    nzp_csi_rs_res_for_interference_present = false;
  bool                                    report_freq_cfg_present                 = false;
  bool                                    codebook_cfg_present                    = false;
  bool                                    dummy_present                           = false;
  bool                                    cqi_table_present                       = false;
  uint8_t                                 report_cfg_id                           = 0;
  uint8_t                                 carrier                                 = 0;
  uint8_t                                 res_for_ch_meas                         = 0;
  uint8_t                                 csi_im_res_for_interference             = 0;
  uint8_t                                 nzp_csi_rs_res_for_interference         = 0;
  report_cfg_type_c_                      report_cfg_type;
  report_quant_c_                         report_quant;
  report_freq_cfg_s_                      report_freq_cfg;
  time_restrict_for_ch_meass_e_           time_restrict_for_ch_meass;
  time_restrict_for_interference_meass_e_ time_restrict_for_interference_meass;
  codebook_cfg_s                          codebook_cfg;
  dummy_e_                                dummy;
  group_based_beam_report_c_              group_based_beam_report;
  cqi_table_e_                            cqi_table;
  subband_size_e_                         subband_size;
  non_pmi_port_ind_l_                     non_pmi_port_ind;
  // ...
  // group 0
  copy_ptr<semi_persistent_on_pusch_v1530_s_> semi_persistent_on_pusch_v1530;
  // group 1
  copy_ptr<semi_persistent_on_pusch_v1610_s_> semi_persistent_on_pusch_v1610;
  copy_ptr<aperiodic_v1610_s_>                aperiodic_v1610;
  copy_ptr<report_quant_r16_c_>               report_quant_r16;
  copy_ptr<codebook_cfg_r16_s>                codebook_cfg_r16;
  // group 2
  bool                                       cqi_bits_per_subband_r17_present     = false;
  bool                                       shared_cmr_r17_present               = false;
  bool                                       csi_report_mode_r17_present          = false;
  bool                                       nof_single_trp_csi_mode1_r17_present = false;
  copy_ptr<group_based_beam_report_v1710_s_> group_based_beam_report_v1710;
  copy_ptr<codebook_cfg_r17_s>               codebook_cfg_r17;
  csi_report_mode_r17_e_                     csi_report_mode_r17;
  nof_single_trp_csi_mode1_r17_e_            nof_single_trp_csi_mode1_r17;
  copy_ptr<report_quant_r17_c_>              report_quant_r17;
  // group 3
  copy_ptr<semi_persistent_on_pusch_v1720_s_> semi_persistent_on_pusch_v1720;
  copy_ptr<aperiodic_v1720_s_>                aperiodic_v1720;
  // group 4
  copy_ptr<codebook_cfg_v1730_s> codebook_cfg_v1730;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-ResourceConfig ::= SEQUENCE
struct csi_res_cfg_s {
  struct csi_rs_res_set_list_c_ {
    struct nzp_csi_rs_ssb_s_ {
      using nzp_csi_rs_res_set_list_l_ = bounded_array<uint8_t, 16>;
      using csi_ssb_res_set_list_l_    = std::array<uint8_t, 1>;

      // member variables
      bool                       csi_ssb_res_set_list_present = false;
      nzp_csi_rs_res_set_list_l_ nzp_csi_rs_res_set_list;
      csi_ssb_res_set_list_l_    csi_ssb_res_set_list;
    };
    using csi_im_res_set_list_l_ = bounded_array<uint8_t, 16>;
    struct types_opts {
      enum options { nzp_csi_rs_ssb, csi_im_res_set_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    csi_rs_res_set_list_c_() = default;
    csi_rs_res_set_list_c_(const csi_rs_res_set_list_c_& other);
    csi_rs_res_set_list_c_& operator=(const csi_rs_res_set_list_c_& other);
    ~csi_rs_res_set_list_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nzp_csi_rs_ssb_s_& nzp_csi_rs_ssb()
    {
      assert_choice_type(types::nzp_csi_rs_ssb, type_, "csi-RS-ResourceSetList");
      return c.get<nzp_csi_rs_ssb_s_>();
    }
    csi_im_res_set_list_l_& csi_im_res_set_list()
    {
      assert_choice_type(types::csi_im_res_set_list, type_, "csi-RS-ResourceSetList");
      return c.get<csi_im_res_set_list_l_>();
    }
    const nzp_csi_rs_ssb_s_& nzp_csi_rs_ssb() const
    {
      assert_choice_type(types::nzp_csi_rs_ssb, type_, "csi-RS-ResourceSetList");
      return c.get<nzp_csi_rs_ssb_s_>();
    }
    const csi_im_res_set_list_l_& csi_im_res_set_list() const
    {
      assert_choice_type(types::csi_im_res_set_list, type_, "csi-RS-ResourceSetList");
      return c.get<csi_im_res_set_list_l_>();
    }
    nzp_csi_rs_ssb_s_&      set_nzp_csi_rs_ssb();
    csi_im_res_set_list_l_& set_csi_im_res_set_list();

  private:
    types                                                      type_;
    choice_buffer_t<csi_im_res_set_list_l_, nzp_csi_rs_ssb_s_> c;

    void destroy_();
  };
  struct res_type_opts {
    enum options { aperiodic, semi_persistent, periodic, nulltype } value;

    const char* to_string() const;
  };
  using res_type_e_ = enumerated<res_type_opts>;

  // member variables
  bool                   ext            = false;
  uint8_t                csi_res_cfg_id = 0;
  csi_rs_res_set_list_c_ csi_rs_res_set_list;
  uint8_t                bwp_id = 0;
  res_type_e_            res_type;
  // ...
  // group 0
  bool    csi_ssb_res_set_list_ext_r17_present = false;
  uint8_t csi_ssb_res_set_list_ext_r17         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-SSB-ResourceSet ::= SEQUENCE
struct csi_ssb_res_set_s {
  using csi_ssb_res_list_l_         = dyn_array<uint8_t>;
  using serving_add_pci_list_r17_l_ = dyn_array<uint8_t>;

  // member variables
  bool                ext                = false;
  uint8_t             csi_ssb_res_set_id = 0;
  csi_ssb_res_list_l_ csi_ssb_res_list;
  // ...
  // group 0
  copy_ptr<serving_add_pci_list_r17_l_> serving_add_pci_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-SemiPersistentOnPUSCH-TriggerStateList ::= SEQUENCE (SIZE (1..64)) OF CSI-SemiPersistentOnPUSCH-TriggerState
using csi_semi_persistent_on_pusch_trigger_state_list_l = dyn_array<csi_semi_persistent_on_pusch_trigger_state_s>;

// NZP-CSI-RS-Resource ::= SEQUENCE
struct nzp_csi_rs_res_s {
  struct pwr_ctrl_offset_ss_opts {
    enum options { db_neg3, db0, db3, db6, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using pwr_ctrl_offset_ss_e_ = enumerated<pwr_ctrl_offset_ss_opts>;

  // member variables
  bool                             ext                              = false;
  bool                             pwr_ctrl_offset_ss_present       = false;
  bool                             periodicity_and_offset_present   = false;
  bool                             qcl_info_periodic_csi_rs_present = false;
  uint8_t                          nzp_csi_rs_res_id                = 0;
  csi_rs_res_map_s                 res_map;
  int8_t                           pwr_ctrl_offset = -8;
  pwr_ctrl_offset_ss_e_            pwr_ctrl_offset_ss;
  uint16_t                         scrambling_id = 0;
  csi_res_periodicity_and_offset_c periodicity_and_offset;
  uint8_t                          qcl_info_periodic_csi_rs = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NZP-CSI-RS-ResourceSet ::= SEQUENCE
struct nzp_csi_rs_res_set_s {
  using nzp_csi_rs_res_l_ = dyn_array<uint8_t>;
  struct repeat_opts {
    enum options { on, off, nulltype } value;

    const char* to_string() const;
  };
  using repeat_e_ = enumerated<repeat_opts>;

  // member variables
  bool              ext                              = false;
  bool              repeat_present                   = false;
  bool              aperiodic_trigger_offset_present = false;
  bool              trs_info_present                 = false;
  uint8_t           nzp_csi_res_set_id               = 0;
  nzp_csi_rs_res_l_ nzp_csi_rs_res;
  repeat_e_         repeat;
  uint8_t           aperiodic_trigger_offset = 0;
  // ...
  // group 0
  bool    aperiodic_trigger_offset_r16_present = false;
  uint8_t aperiodic_trigger_offset_r16         = 0;
  // group 1
  bool                                     pdc_info_r17_present                    = false;
  bool                                     aperiodic_trigger_offset_r17_present    = false;
  bool                                     aperiodic_trigger_offset_l2_r17_present = false;
  copy_ptr<cmr_grouping_and_pairing_r17_s> cmr_grouping_and_pairing_r17;
  uint8_t                                  aperiodic_trigger_offset_r17    = 0;
  uint8_t                                  aperiodic_trigger_offset_l2_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCellActivationRS-Config-r17 ::= SEQUENCE
struct scell_activation_rs_cfg_r17_s {
  bool     ext                            = false;
  bool     gap_between_bursts_r17_present = false;
  uint16_t scell_activation_rs_id_r17     = 1;
  uint8_t  res_set_r17                    = 0;
  uint8_t  gap_between_bursts_r17         = 2;
  uint8_t  qcl_info_r17                   = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-MeasConfig ::= SEQUENCE
struct csi_meas_cfg_s {
  using nzp_csi_rs_res_to_add_mod_list_l_              = dyn_array<nzp_csi_rs_res_s>;
  using nzp_csi_rs_res_to_release_list_l_              = dyn_array<uint8_t>;
  using nzp_csi_rs_res_set_to_add_mod_list_l_          = dyn_array<nzp_csi_rs_res_set_s>;
  using nzp_csi_rs_res_set_to_release_list_l_          = dyn_array<uint8_t>;
  using csi_im_res_to_add_mod_list_l_                  = dyn_array<csi_im_res_s>;
  using csi_im_res_to_release_list_l_                  = bounded_array<uint8_t, 32>;
  using csi_im_res_set_to_add_mod_list_l_              = dyn_array<csi_im_res_set_s>;
  using csi_im_res_set_to_release_list_l_              = dyn_array<uint8_t>;
  using csi_ssb_res_set_to_add_mod_list_l_             = dyn_array<csi_ssb_res_set_s>;
  using csi_ssb_res_set_to_release_list_l_             = dyn_array<uint8_t>;
  using csi_res_cfg_to_add_mod_list_l_                 = dyn_array<csi_res_cfg_s>;
  using csi_res_cfg_to_release_list_l_                 = dyn_array<uint8_t>;
  using csi_report_cfg_to_add_mod_list_l_              = dyn_array<csi_report_cfg_s>;
  using csi_report_cfg_to_release_list_l_              = dyn_array<uint8_t>;
  using scell_activation_rs_cfg_to_add_mod_list_r17_l_ = dyn_array<scell_activation_rs_cfg_r17_s>;
  using scell_activation_rs_cfg_to_release_list_r17_l_ = dyn_array<uint16_t>;

  // member variables
  bool                                  ext                                                 = false;
  bool                                  report_trigger_size_present                         = false;
  bool                                  aperiodic_trigger_state_list_present                = false;
  bool                                  semi_persistent_on_pusch_trigger_state_list_present = false;
  nzp_csi_rs_res_to_add_mod_list_l_     nzp_csi_rs_res_to_add_mod_list;
  nzp_csi_rs_res_to_release_list_l_     nzp_csi_rs_res_to_release_list;
  nzp_csi_rs_res_set_to_add_mod_list_l_ nzp_csi_rs_res_set_to_add_mod_list;
  nzp_csi_rs_res_set_to_release_list_l_ nzp_csi_rs_res_set_to_release_list;
  csi_im_res_to_add_mod_list_l_         csi_im_res_to_add_mod_list;
  csi_im_res_to_release_list_l_         csi_im_res_to_release_list;
  csi_im_res_set_to_add_mod_list_l_     csi_im_res_set_to_add_mod_list;
  csi_im_res_set_to_release_list_l_     csi_im_res_set_to_release_list;
  csi_ssb_res_set_to_add_mod_list_l_    csi_ssb_res_set_to_add_mod_list;
  csi_ssb_res_set_to_release_list_l_    csi_ssb_res_set_to_release_list;
  csi_res_cfg_to_add_mod_list_l_        csi_res_cfg_to_add_mod_list;
  csi_res_cfg_to_release_list_l_        csi_res_cfg_to_release_list;
  csi_report_cfg_to_add_mod_list_l_     csi_report_cfg_to_add_mod_list;
  csi_report_cfg_to_release_list_l_     csi_report_cfg_to_release_list;
  uint8_t                               report_trigger_size = 0;
  setup_release_c<dyn_seq_of<csi_aperiodic_trigger_state_s, 1, 128>> aperiodic_trigger_state_list;
  setup_release_c<dyn_seq_of<csi_semi_persistent_on_pusch_trigger_state_s, 1, 64>>
      semi_persistent_on_pusch_trigger_state_list;
  // ...
  // group 0
  bool    report_trigger_size_dci_0_2_r16_present = false;
  uint8_t report_trigger_size_dci_0_2_r16         = 0;
  // group 1
  copy_ptr<scell_activation_rs_cfg_to_add_mod_list_r17_l_> scell_activation_rs_cfg_to_add_mod_list_r17;
  copy_ptr<scell_activation_rs_cfg_to_release_list_r17_l_> scell_activation_rs_cfg_to_release_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-PRS-QCL-Info-r17 ::= CHOICE
struct dl_prs_qcl_info_r17_c {
  struct ssb_r17_s_ {
    struct rs_type_r17_opts {
      enum options { type_c, type_d, type_c_plus_type_d, nulltype } value;

      const char* to_string() const;
    };
    using rs_type_r17_e_ = enumerated<rs_type_r17_opts>;

    // member variables
    bool           ext         = false;
    uint8_t        ssb_idx_r17 = 0;
    rs_type_r17_e_ rs_type_r17;
    // ...
  };
  struct dl_prs_r17_s_ {
    bool    ext                   = false;
    uint8_t qcl_dl_prs_res_id_r17 = 0;
    // ...
  };
  struct types_opts {
    enum options { ssb_r17, dl_prs_r17, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  dl_prs_qcl_info_r17_c() = default;
  dl_prs_qcl_info_r17_c(const dl_prs_qcl_info_r17_c& other);
  dl_prs_qcl_info_r17_c& operator=(const dl_prs_qcl_info_r17_c& other);
  ~dl_prs_qcl_info_r17_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ssb_r17_s_& ssb_r17()
  {
    assert_choice_type(types::ssb_r17, type_, "DL-PRS-QCL-Info-r17");
    return c.get<ssb_r17_s_>();
  }
  dl_prs_r17_s_& dl_prs_r17()
  {
    assert_choice_type(types::dl_prs_r17, type_, "DL-PRS-QCL-Info-r17");
    return c.get<dl_prs_r17_s_>();
  }
  const ssb_r17_s_& ssb_r17() const
  {
    assert_choice_type(types::ssb_r17, type_, "DL-PRS-QCL-Info-r17");
    return c.get<ssb_r17_s_>();
  }
  const dl_prs_r17_s_& dl_prs_r17() const
  {
    assert_choice_type(types::dl_prs_r17, type_, "DL-PRS-QCL-Info-r17");
    return c.get<dl_prs_r17_s_>();
  }
  ssb_r17_s_&    set_ssb_r17();
  dl_prs_r17_s_& set_dl_prs_r17();

private:
  types                                      type_;
  choice_buffer_t<dl_prs_r17_s_, ssb_r17_s_> c;

  void destroy_();
};

// SRS-CC-SetIndex ::= SEQUENCE
struct srs_cc_set_idx_s {
  bool    cc_set_idx_present           = false;
  bool    cc_idx_in_one_cc_set_present = false;
  uint8_t cc_set_idx                   = 0;
  uint8_t cc_idx_in_one_cc_set         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SlotFormatCombination ::= SEQUENCE
struct slot_format_combination_s {
  using slot_formats_l_ = dyn_array<uint16_t>;

  // member variables
  uint16_t        slot_format_combination_id = 0;
  slot_formats_l_ slot_formats;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17 ::= CHOICE
struct nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c {
  struct scs15_r17_c_ {
    struct types_opts {
      enum options {
        n4_r17,
        n5_r17,
        n8_r17,
        n10_r17,
        n16_r17,
        n20_r17,
        n32_r17,
        n40_r17,
        n64_r17,
        n80_r17,
        n160_r17,
        n320_r17,
        n640_r17,
        n1280_r17,
        n2560_r17,
        n5120_r17,
        n10240_r17,
        // ...
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    scs15_r17_c_() = default;
    scs15_r17_c_(const scs15_r17_c_& other);
    scs15_r17_c_& operator=(const scs15_r17_c_& other);
    ~scs15_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& n4_r17()
    {
      assert_choice_type(types::n4_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n5_r17()
    {
      assert_choice_type(types::n5_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n8_r17()
    {
      assert_choice_type(types::n8_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n10_r17()
    {
      assert_choice_type(types::n10_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n16_r17()
    {
      assert_choice_type(types::n16_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n20_r17()
    {
      assert_choice_type(types::n20_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n32_r17()
    {
      assert_choice_type(types::n32_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n40_r17()
    {
      assert_choice_type(types::n40_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n64_r17()
    {
      assert_choice_type(types::n64_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n80_r17()
    {
      assert_choice_type(types::n80_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n160_r17()
    {
      assert_choice_type(types::n160_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    uint16_t& n320_r17()
    {
      assert_choice_type(types::n320_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n640_r17()
    {
      assert_choice_type(types::n640_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n1280_r17()
    {
      assert_choice_type(types::n1280_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n2560_r17()
    {
      assert_choice_type(types::n2560_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n5120_r17()
    {
      assert_choice_type(types::n5120_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n10240_r17()
    {
      assert_choice_type(types::n10240_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    const uint8_t& n4_r17() const
    {
      assert_choice_type(types::n4_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n5_r17() const
    {
      assert_choice_type(types::n5_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n8_r17() const
    {
      assert_choice_type(types::n8_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n10_r17() const
    {
      assert_choice_type(types::n10_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n16_r17() const
    {
      assert_choice_type(types::n16_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n20_r17() const
    {
      assert_choice_type(types::n20_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n32_r17() const
    {
      assert_choice_type(types::n32_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n40_r17() const
    {
      assert_choice_type(types::n40_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n64_r17() const
    {
      assert_choice_type(types::n64_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n80_r17() const
    {
      assert_choice_type(types::n80_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n160_r17() const
    {
      assert_choice_type(types::n160_r17, type_, "scs15-r17");
      return c.get<uint8_t>();
    }
    const uint16_t& n320_r17() const
    {
      assert_choice_type(types::n320_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n640_r17() const
    {
      assert_choice_type(types::n640_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n1280_r17() const
    {
      assert_choice_type(types::n1280_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n2560_r17() const
    {
      assert_choice_type(types::n2560_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n5120_r17() const
    {
      assert_choice_type(types::n5120_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n10240_r17() const
    {
      assert_choice_type(types::n10240_r17, type_, "scs15-r17");
      return c.get<uint16_t>();
    }
    uint8_t&  set_n4_r17();
    uint8_t&  set_n5_r17();
    uint8_t&  set_n8_r17();
    uint8_t&  set_n10_r17();
    uint8_t&  set_n16_r17();
    uint8_t&  set_n20_r17();
    uint8_t&  set_n32_r17();
    uint8_t&  set_n40_r17();
    uint8_t&  set_n64_r17();
    uint8_t&  set_n80_r17();
    uint8_t&  set_n160_r17();
    uint16_t& set_n320_r17();
    uint16_t& set_n640_r17();
    uint16_t& set_n1280_r17();
    uint16_t& set_n2560_r17();
    uint16_t& set_n5120_r17();
    uint16_t& set_n10240_r17();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct scs30_r17_c_ {
    struct types_opts {
      enum options {
        n8_r17,
        n10_r17,
        n16_r17,
        n20_r17,
        n32_r17,
        n40_r17,
        n64_r17,
        n80_r17,
        n128_r17,
        n160_r17,
        n320_r17,
        n640_r17,
        n1280_r17,
        n2560_r17,
        n5120_r17,
        n10240_r17,
        n20480_r17,
        // ...
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    scs30_r17_c_() = default;
    scs30_r17_c_(const scs30_r17_c_& other);
    scs30_r17_c_& operator=(const scs30_r17_c_& other);
    ~scs30_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& n8_r17()
    {
      assert_choice_type(types::n8_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n10_r17()
    {
      assert_choice_type(types::n10_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n16_r17()
    {
      assert_choice_type(types::n16_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n20_r17()
    {
      assert_choice_type(types::n20_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n32_r17()
    {
      assert_choice_type(types::n32_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n40_r17()
    {
      assert_choice_type(types::n40_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n64_r17()
    {
      assert_choice_type(types::n64_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n80_r17()
    {
      assert_choice_type(types::n80_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n128_r17()
    {
      assert_choice_type(types::n128_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n160_r17()
    {
      assert_choice_type(types::n160_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    uint16_t& n320_r17()
    {
      assert_choice_type(types::n320_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n640_r17()
    {
      assert_choice_type(types::n640_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n1280_r17()
    {
      assert_choice_type(types::n1280_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n2560_r17()
    {
      assert_choice_type(types::n2560_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n5120_r17()
    {
      assert_choice_type(types::n5120_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n10240_r17()
    {
      assert_choice_type(types::n10240_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n20480_r17()
    {
      assert_choice_type(types::n20480_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    const uint8_t& n8_r17() const
    {
      assert_choice_type(types::n8_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n10_r17() const
    {
      assert_choice_type(types::n10_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n16_r17() const
    {
      assert_choice_type(types::n16_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n20_r17() const
    {
      assert_choice_type(types::n20_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n32_r17() const
    {
      assert_choice_type(types::n32_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n40_r17() const
    {
      assert_choice_type(types::n40_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n64_r17() const
    {
      assert_choice_type(types::n64_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n80_r17() const
    {
      assert_choice_type(types::n80_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n128_r17() const
    {
      assert_choice_type(types::n128_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n160_r17() const
    {
      assert_choice_type(types::n160_r17, type_, "scs30-r17");
      return c.get<uint8_t>();
    }
    const uint16_t& n320_r17() const
    {
      assert_choice_type(types::n320_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n640_r17() const
    {
      assert_choice_type(types::n640_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n1280_r17() const
    {
      assert_choice_type(types::n1280_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n2560_r17() const
    {
      assert_choice_type(types::n2560_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n5120_r17() const
    {
      assert_choice_type(types::n5120_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n10240_r17() const
    {
      assert_choice_type(types::n10240_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n20480_r17() const
    {
      assert_choice_type(types::n20480_r17, type_, "scs30-r17");
      return c.get<uint16_t>();
    }
    uint8_t&  set_n8_r17();
    uint8_t&  set_n10_r17();
    uint8_t&  set_n16_r17();
    uint8_t&  set_n20_r17();
    uint8_t&  set_n32_r17();
    uint8_t&  set_n40_r17();
    uint8_t&  set_n64_r17();
    uint8_t&  set_n80_r17();
    uint8_t&  set_n128_r17();
    uint8_t&  set_n160_r17();
    uint16_t& set_n320_r17();
    uint16_t& set_n640_r17();
    uint16_t& set_n1280_r17();
    uint16_t& set_n2560_r17();
    uint16_t& set_n5120_r17();
    uint16_t& set_n10240_r17();
    uint16_t& set_n20480_r17();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct scs60_r17_c_ {
    struct types_opts {
      enum options {
        n16_r17,
        n20_r17,
        n32_r17,
        n40_r17,
        n64_r17,
        n80_r17,
        n128_r17,
        n160_r17,
        n256_r17,
        n320_r17,
        n640_r17,
        n1280_r17,
        n2560_r17,
        n5120_r17,
        n10240_r17,
        n20480_r17,
        n40960_r17,
        // ...
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    scs60_r17_c_() = default;
    scs60_r17_c_(const scs60_r17_c_& other);
    scs60_r17_c_& operator=(const scs60_r17_c_& other);
    ~scs60_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& n16_r17()
    {
      assert_choice_type(types::n16_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n20_r17()
    {
      assert_choice_type(types::n20_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n32_r17()
    {
      assert_choice_type(types::n32_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n40_r17()
    {
      assert_choice_type(types::n40_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n64_r17()
    {
      assert_choice_type(types::n64_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n80_r17()
    {
      assert_choice_type(types::n80_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n128_r17()
    {
      assert_choice_type(types::n128_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n160_r17()
    {
      assert_choice_type(types::n160_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    uint16_t& n256_r17()
    {
      assert_choice_type(types::n256_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n320_r17()
    {
      assert_choice_type(types::n320_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n640_r17()
    {
      assert_choice_type(types::n640_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n1280_r17()
    {
      assert_choice_type(types::n1280_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n2560_r17()
    {
      assert_choice_type(types::n2560_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n5120_r17()
    {
      assert_choice_type(types::n5120_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n10240_r17()
    {
      assert_choice_type(types::n10240_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n20480_r17()
    {
      assert_choice_type(types::n20480_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n40960_r17()
    {
      assert_choice_type(types::n40960_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint8_t& n16_r17() const
    {
      assert_choice_type(types::n16_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n20_r17() const
    {
      assert_choice_type(types::n20_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n32_r17() const
    {
      assert_choice_type(types::n32_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n40_r17() const
    {
      assert_choice_type(types::n40_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n64_r17() const
    {
      assert_choice_type(types::n64_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n80_r17() const
    {
      assert_choice_type(types::n80_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n128_r17() const
    {
      assert_choice_type(types::n128_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n160_r17() const
    {
      assert_choice_type(types::n160_r17, type_, "scs60-r17");
      return c.get<uint8_t>();
    }
    const uint16_t& n256_r17() const
    {
      assert_choice_type(types::n256_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n320_r17() const
    {
      assert_choice_type(types::n320_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n640_r17() const
    {
      assert_choice_type(types::n640_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n1280_r17() const
    {
      assert_choice_type(types::n1280_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n2560_r17() const
    {
      assert_choice_type(types::n2560_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n5120_r17() const
    {
      assert_choice_type(types::n5120_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n10240_r17() const
    {
      assert_choice_type(types::n10240_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n20480_r17() const
    {
      assert_choice_type(types::n20480_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n40960_r17() const
    {
      assert_choice_type(types::n40960_r17, type_, "scs60-r17");
      return c.get<uint16_t>();
    }
    uint8_t&  set_n16_r17();
    uint8_t&  set_n20_r17();
    uint8_t&  set_n32_r17();
    uint8_t&  set_n40_r17();
    uint8_t&  set_n64_r17();
    uint8_t&  set_n80_r17();
    uint8_t&  set_n128_r17();
    uint8_t&  set_n160_r17();
    uint16_t& set_n256_r17();
    uint16_t& set_n320_r17();
    uint16_t& set_n640_r17();
    uint16_t& set_n1280_r17();
    uint16_t& set_n2560_r17();
    uint16_t& set_n5120_r17();
    uint16_t& set_n10240_r17();
    uint16_t& set_n20480_r17();
    uint16_t& set_n40960_r17();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct scs120_r17_c_ {
    struct types_opts {
      enum options {
        n32_r17,
        n40_r17,
        n64_r17,
        n80_r17,
        n128_r17,
        n160_r17,
        n256_r17,
        n320_r17,
        n512_r17,
        n640_r17,
        n1280_r17,
        n2560_r17,
        n5120_r17,
        n10240_r17,
        n20480_r17,
        n40960_r17,
        n81920_r17,
        // ...
        nulltype
      } value;
      typedef uint32_t number_type;

      const char* to_string() const;
      uint32_t    to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    scs120_r17_c_() = default;
    scs120_r17_c_(const scs120_r17_c_& other);
    scs120_r17_c_& operator=(const scs120_r17_c_& other);
    ~scs120_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& n32_r17()
    {
      assert_choice_type(types::n32_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n40_r17()
    {
      assert_choice_type(types::n40_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n64_r17()
    {
      assert_choice_type(types::n64_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n80_r17()
    {
      assert_choice_type(types::n80_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n128_r17()
    {
      assert_choice_type(types::n128_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n160_r17()
    {
      assert_choice_type(types::n160_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    uint16_t& n256_r17()
    {
      assert_choice_type(types::n256_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n320_r17()
    {
      assert_choice_type(types::n320_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n512_r17()
    {
      assert_choice_type(types::n512_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n640_r17()
    {
      assert_choice_type(types::n640_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n1280_r17()
    {
      assert_choice_type(types::n1280_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n2560_r17()
    {
      assert_choice_type(types::n2560_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n5120_r17()
    {
      assert_choice_type(types::n5120_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n10240_r17()
    {
      assert_choice_type(types::n10240_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n20480_r17()
    {
      assert_choice_type(types::n20480_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint16_t& n40960_r17()
    {
      assert_choice_type(types::n40960_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    uint32_t& n81920_r17()
    {
      assert_choice_type(types::n81920_r17, type_, "scs120-r17");
      return c.get<uint32_t>();
    }
    const uint8_t& n32_r17() const
    {
      assert_choice_type(types::n32_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n40_r17() const
    {
      assert_choice_type(types::n40_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n64_r17() const
    {
      assert_choice_type(types::n64_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n80_r17() const
    {
      assert_choice_type(types::n80_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n128_r17() const
    {
      assert_choice_type(types::n128_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n160_r17() const
    {
      assert_choice_type(types::n160_r17, type_, "scs120-r17");
      return c.get<uint8_t>();
    }
    const uint16_t& n256_r17() const
    {
      assert_choice_type(types::n256_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n320_r17() const
    {
      assert_choice_type(types::n320_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n512_r17() const
    {
      assert_choice_type(types::n512_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n640_r17() const
    {
      assert_choice_type(types::n640_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n1280_r17() const
    {
      assert_choice_type(types::n1280_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n2560_r17() const
    {
      assert_choice_type(types::n2560_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n5120_r17() const
    {
      assert_choice_type(types::n5120_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n10240_r17() const
    {
      assert_choice_type(types::n10240_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n20480_r17() const
    {
      assert_choice_type(types::n20480_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& n40960_r17() const
    {
      assert_choice_type(types::n40960_r17, type_, "scs120-r17");
      return c.get<uint16_t>();
    }
    const uint32_t& n81920_r17() const
    {
      assert_choice_type(types::n81920_r17, type_, "scs120-r17");
      return c.get<uint32_t>();
    }
    uint8_t&  set_n32_r17();
    uint8_t&  set_n40_r17();
    uint8_t&  set_n64_r17();
    uint8_t&  set_n80_r17();
    uint8_t&  set_n128_r17();
    uint8_t&  set_n160_r17();
    uint16_t& set_n256_r17();
    uint16_t& set_n320_r17();
    uint16_t& set_n512_r17();
    uint16_t& set_n640_r17();
    uint16_t& set_n1280_r17();
    uint16_t& set_n2560_r17();
    uint16_t& set_n5120_r17();
    uint16_t& set_n10240_r17();
    uint16_t& set_n20480_r17();
    uint16_t& set_n40960_r17();
    uint32_t& set_n81920_r17();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct types_opts {
    enum options { scs15_r17, scs30_r17, scs60_r17, scs120_r17, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c() = default;
  nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c(const nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c& other);
  nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c&
  operator=(const nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c& other);
  ~nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  scs15_r17_c_& scs15_r17()
  {
    assert_choice_type(types::scs15_r17, type_, "NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17");
    return c.get<scs15_r17_c_>();
  }
  scs30_r17_c_& scs30_r17()
  {
    assert_choice_type(types::scs30_r17, type_, "NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17");
    return c.get<scs30_r17_c_>();
  }
  scs60_r17_c_& scs60_r17()
  {
    assert_choice_type(types::scs60_r17, type_, "NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17");
    return c.get<scs60_r17_c_>();
  }
  scs120_r17_c_& scs120_r17()
  {
    assert_choice_type(types::scs120_r17, type_, "NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17");
    return c.get<scs120_r17_c_>();
  }
  const scs15_r17_c_& scs15_r17() const
  {
    assert_choice_type(types::scs15_r17, type_, "NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17");
    return c.get<scs15_r17_c_>();
  }
  const scs30_r17_c_& scs30_r17() const
  {
    assert_choice_type(types::scs30_r17, type_, "NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17");
    return c.get<scs30_r17_c_>();
  }
  const scs60_r17_c_& scs60_r17() const
  {
    assert_choice_type(types::scs60_r17, type_, "NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17");
    return c.get<scs60_r17_c_>();
  }
  const scs120_r17_c_& scs120_r17() const
  {
    assert_choice_type(types::scs120_r17, type_, "NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17");
    return c.get<scs120_r17_c_>();
  }
  scs15_r17_c_&  set_scs15_r17();
  scs30_r17_c_&  set_scs30_r17();
  scs60_r17_c_&  set_scs60_r17();
  scs120_r17_c_& set_scs120_r17();

private:
  types                                                                    type_;
  choice_buffer_t<scs120_r17_c_, scs15_r17_c_, scs30_r17_c_, scs60_r17_c_> c;

  void destroy_();
};

// NR-DL-PRS-Resource-r17 ::= SEQUENCE
struct nr_dl_prs_res_r17_s {
  struct dl_prs_comb_size_n_and_re_offset_r17_c_ {
    struct types_opts {
      enum options { n2_r17, n4_r17, n6_r17, n12_r17, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    dl_prs_comb_size_n_and_re_offset_r17_c_() = default;
    dl_prs_comb_size_n_and_re_offset_r17_c_(const dl_prs_comb_size_n_and_re_offset_r17_c_& other);
    dl_prs_comb_size_n_and_re_offset_r17_c_& operator=(const dl_prs_comb_size_n_and_re_offset_r17_c_& other);
    ~dl_prs_comb_size_n_and_re_offset_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& n2_r17()
    {
      assert_choice_type(types::n2_r17, type_, "dl-PRS-CombSizeN-AndReOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n4_r17()
    {
      assert_choice_type(types::n4_r17, type_, "dl-PRS-CombSizeN-AndReOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n6_r17()
    {
      assert_choice_type(types::n6_r17, type_, "dl-PRS-CombSizeN-AndReOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& n12_r17()
    {
      assert_choice_type(types::n12_r17, type_, "dl-PRS-CombSizeN-AndReOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n2_r17() const
    {
      assert_choice_type(types::n2_r17, type_, "dl-PRS-CombSizeN-AndReOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n4_r17() const
    {
      assert_choice_type(types::n4_r17, type_, "dl-PRS-CombSizeN-AndReOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n6_r17() const
    {
      assert_choice_type(types::n6_r17, type_, "dl-PRS-CombSizeN-AndReOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& n12_r17() const
    {
      assert_choice_type(types::n12_r17, type_, "dl-PRS-CombSizeN-AndReOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& set_n2_r17();
    uint8_t& set_n4_r17();
    uint8_t& set_n6_r17();
    uint8_t& set_n12_r17();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                                    ext                         = false;
  bool                                    dl_prs_qcl_info_r17_present = false;
  uint8_t                                 nr_dl_prs_res_id_r17        = 0;
  uint16_t                                dl_prs_seq_id_r17           = 0;
  dl_prs_comb_size_n_and_re_offset_r17_c_ dl_prs_comb_size_n_and_re_offset_r17;
  uint16_t                                dl_prs_res_slot_offset_r17   = 0;
  uint8_t                                 dl_prs_res_symbol_offset_r17 = 0;
  dl_prs_qcl_info_r17_c                   dl_prs_qcl_info_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// P0AlphaSet-r17 ::= SEQUENCE
struct p0_alpha_set_r17_s {
  struct closed_loop_idx_r17_opts {
    enum options { i0, i1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using closed_loop_idx_r17_e_ = enumerated<closed_loop_idx_r17_opts>;

  // member variables
  bool                   p0_r17_present    = false;
  bool                   alpha_r17_present = false;
  int8_t                 p0_r17            = -16;
  alpha_e                alpha_r17;
  closed_loop_idx_r17_e_ closed_loop_idx_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-CodeBlockGroupTransmission ::= SEQUENCE
struct pdsch_code_block_group_tx_s {
  struct max_code_block_groups_per_transport_block_opts {
    enum options { n2, n4, n6, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_code_block_groups_per_transport_block_e_ = enumerated<max_code_block_groups_per_transport_block_opts>;

  // member variables
  bool                                         ext = false;
  max_code_block_groups_per_transport_block_e_ max_code_block_groups_per_transport_block;
  bool                                         code_block_group_flush_ind = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-CodeBlockGroupTransmission ::= SEQUENCE
struct pusch_code_block_group_tx_s {
  struct max_code_block_groups_per_transport_block_opts {
    enum options { n2, n4, n6, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_code_block_groups_per_transport_block_e_ = enumerated<max_code_block_groups_per_transport_block_opts>;

  // member variables
  bool                                         ext = false;
  max_code_block_groups_per_transport_block_e_ max_code_block_groups_per_transport_block;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RepFactorAndTimeGap-r17 ::= SEQUENCE
struct rep_factor_and_time_gap_r17_s {
  struct repeat_factor_r17_opts {
    enum options { n2, n4, n6, n8, n16, n32, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using repeat_factor_r17_e_ = enumerated<repeat_factor_r17_opts>;
  struct time_gap_r17_opts {
    enum options { s1, s2, s4, s8, s16, s32, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using time_gap_r17_e_ = enumerated<time_gap_r17_opts>;

  // member variables
  repeat_factor_r17_e_ repeat_factor_r17;
  time_gap_r17_e_      time_gap_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-TPC-PDCCH-Config ::= SEQUENCE
struct srs_tpc_pdcch_cfg_s {
  using srs_cc_set_idxlist_l_ = dyn_array<srs_cc_set_idx_s>;

  // member variables
  srs_cc_set_idxlist_l_ srs_cc_set_idxlist;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SearchSpaceSwitchTrigger-r16 ::= SEQUENCE
struct search_space_switch_trigger_r16_s {
  uint8_t serving_cell_id_r16 = 0;
  uint8_t position_in_dci_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SlotFormatCombinationsPerCell ::= SEQUENCE
struct slot_format_combinations_per_cell_s {
  using slot_format_combinations_l_ = dyn_array<slot_format_combination_s>;

  // member variables
  bool                        ext                         = false;
  bool                        subcarrier_spacing2_present = false;
  bool                        position_in_dci_present     = false;
  uint8_t                     serving_cell_id             = 0;
  subcarrier_spacing_e        subcarrier_spacing;
  subcarrier_spacing_e        subcarrier_spacing2;
  slot_format_combinations_l_ slot_format_combinations;
  uint8_t                     position_in_dci = 0;
  // ...
  // group 0
  bool enable_cfg_ul_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GuardBand-r16 ::= SEQUENCE
struct guard_band_r16_s {
  uint16_t start_crb_r16  = 0;
  uint8_t  nrof_c_rbs_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LTE-NeighCellsCRS-AssistInfo-r17 ::= SEQUENCE
struct lte_neigh_cells_crs_assist_info_r17_s {
  struct neigh_carrier_bw_dl_r17_opts {
    enum options { n6, n15, n25, n50, n75, n100, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using neigh_carrier_bw_dl_r17_e_ = enumerated<neigh_carrier_bw_dl_r17_opts>;
  struct neigh_nrof_crs_ports_r17_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using neigh_nrof_crs_ports_r17_e_ = enumerated<neigh_nrof_crs_ports_r17_opts>;
  struct neigh_v_shift_r17_opts {
    enum options { n0, n1, n2, n3, n4, n5, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using neigh_v_shift_r17_e_ = enumerated<neigh_v_shift_r17_opts>;

  // member variables
  bool                        neigh_carrier_bw_dl_r17_present   = false;
  bool                        neigh_carrier_freq_dl_r17_present = false;
  bool                        neigh_cell_id_r17_present         = false;
  bool                        neigh_crs_muting_r17_present      = false;
  bool                        neigh_nrof_crs_ports_r17_present  = false;
  bool                        neigh_v_shift_r17_present         = false;
  neigh_carrier_bw_dl_r17_e_  neigh_carrier_bw_dl_r17;
  uint16_t                    neigh_carrier_freq_dl_r17 = 0;
  uint16_t                    neigh_cell_id_r17         = 0;
  eutra_mbsfn_sf_cfg_list_l   neigh_mbsfn_sf_cfg_list_r17;
  neigh_nrof_crs_ports_r17_e_ neigh_nrof_crs_ports_r17;
  neigh_v_shift_r17_e_        neigh_v_shift_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-DL-PRS-PDC-ResourceSet-r17 ::= SEQUENCE
struct nr_dl_prs_pdc_res_set_r17_s {
  struct num_symbols_r17_opts {
    enum options { n2, n4, n6, n12, spare4, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using num_symbols_r17_e_ = enumerated<num_symbols_r17_opts>;
  using res_list_r17_l_    = dyn_array<nr_dl_prs_res_r17_s>;

  // member variables
  bool                                                ext                                 = false;
  bool                                                rep_factor_and_time_gap_r17_present = false;
  nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c periodicity_and_offset_r17;
  num_symbols_r17_e_                                  num_symbols_r17;
  uint8_t                                             dl_prs_res_bw_r17    = 1;
  uint16_t                                            dl_prs_start_prb_r17 = 0;
  res_list_r17_l_                                     res_list_r17;
  rep_factor_and_time_gap_r17_s                       rep_factor_and_time_gap_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OutsideActiveTimeConfig-r16 ::= SEQUENCE
struct outside_active_time_cfg_r16_s {
  bool    first_outside_active_time_bwp_id_r16_present   = false;
  bool    dormancy_group_outside_active_time_r16_present = false;
  uint8_t first_outside_active_time_bwp_id_r16           = 0;
  uint8_t dormancy_group_outside_active_time_r16         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-CodeBlockGroupTransmissionList-r16 ::= SEQUENCE (SIZE (1..2)) OF PDSCH-CodeBlockGroupTransmission
using pdsch_code_block_group_tx_list_r16_l = dyn_array<pdsch_code_block_group_tx_s>;

// PUSCH-ServingCellConfig ::= SEQUENCE
struct pusch_serving_cell_cfg_s {
  struct xoverhead_opts {
    enum options { xoh6, xoh12, xoh18, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using xoverhead_e_ = enumerated<xoverhead_opts>;

  // member variables
  bool                                         ext                         = false;
  bool                                         code_block_group_tx_present = false;
  bool                                         rate_matching_present       = false;
  bool                                         xoverhead_present           = false;
  setup_release_c<pusch_code_block_group_tx_s> code_block_group_tx;
  xoverhead_e_                                 xoverhead;
  // ...
  // group 0
  bool    max_mimo_layers_present          = false;
  bool    processing_type2_enabled_present = false;
  uint8_t max_mimo_layers                  = 1;
  bool    processing_type2_enabled         = false;
  // group 1
  copy_ptr<setup_release_c<integer<uint8_t, 1, 4>>> max_mimo_layers_dci_0_2_r16;
  // group 2
  bool                                           nrof_harq_processes_for_pusch_r17_present = false;
  copy_ptr<setup_release_c<fixed_bitstring<32>>> ul_harq_mode_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-CarrierSwitching ::= SEQUENCE
struct srs_carrier_switching_s {
  struct srs_switch_from_carrier_opts {
    enum options { sul, nul, nulltype } value;

    const char* to_string() const;
  };
  using srs_switch_from_carrier_e_ = enumerated<srs_switch_from_carrier_opts>;
  struct srs_tpc_pdcch_group_c_ {
    using type_a_l_ = dyn_array<srs_tpc_pdcch_cfg_s>;
    struct types_opts {
      enum options { type_a, type_b, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    srs_tpc_pdcch_group_c_() = default;
    srs_tpc_pdcch_group_c_(const srs_tpc_pdcch_group_c_& other);
    srs_tpc_pdcch_group_c_& operator=(const srs_tpc_pdcch_group_c_& other);
    ~srs_tpc_pdcch_group_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    type_a_l_& type_a()
    {
      assert_choice_type(types::type_a, type_, "srs-TPC-PDCCH-Group");
      return c.get<type_a_l_>();
    }
    srs_tpc_pdcch_cfg_s& type_b()
    {
      assert_choice_type(types::type_b, type_, "srs-TPC-PDCCH-Group");
      return c.get<srs_tpc_pdcch_cfg_s>();
    }
    const type_a_l_& type_a() const
    {
      assert_choice_type(types::type_a, type_, "srs-TPC-PDCCH-Group");
      return c.get<type_a_l_>();
    }
    const srs_tpc_pdcch_cfg_s& type_b() const
    {
      assert_choice_type(types::type_b, type_, "srs-TPC-PDCCH-Group");
      return c.get<srs_tpc_pdcch_cfg_s>();
    }
    type_a_l_&           set_type_a();
    srs_tpc_pdcch_cfg_s& set_type_b();

  private:
    types                                           type_;
    choice_buffer_t<srs_tpc_pdcch_cfg_s, type_a_l_> c;

    void destroy_();
  };
  using monitoring_cells_l_ = bounded_array<uint8_t, 32>;

  // member variables
  bool                       ext                                   = false;
  bool                       srs_switch_from_serv_cell_idx_present = false;
  bool                       srs_tpc_pdcch_group_present           = false;
  uint8_t                    srs_switch_from_serv_cell_idx         = 0;
  srs_switch_from_carrier_e_ srs_switch_from_carrier;
  srs_tpc_pdcch_group_c_     srs_tpc_pdcch_group;
  monitoring_cells_l_        monitoring_cells;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-MTC-AdditionalPCI-r17 ::= SEQUENCE
struct ssb_mtc_add_pci_r17_s {
  struct periodicity_r17_opts {
    enum options { ms5, ms10, ms20, ms40, ms80, ms160, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using periodicity_r17_e_ = enumerated<periodicity_r17_opts>;
  struct ssb_positions_in_burst_r17_c_ {
    struct types_opts {
      enum options { short_bitmap, medium_bitmap, long_bitmap, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ssb_positions_in_burst_r17_c_() = default;
    ssb_positions_in_burst_r17_c_(const ssb_positions_in_burst_r17_c_& other);
    ssb_positions_in_burst_r17_c_& operator=(const ssb_positions_in_burst_r17_c_& other);
    ~ssb_positions_in_burst_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<4>& short_bitmap()
    {
      assert_choice_type(types::short_bitmap, type_, "ssb-PositionsInBurst-r17");
      return c.get<fixed_bitstring<4>>();
    }
    fixed_bitstring<8>& medium_bitmap()
    {
      assert_choice_type(types::medium_bitmap, type_, "ssb-PositionsInBurst-r17");
      return c.get<fixed_bitstring<8>>();
    }
    fixed_bitstring<64>& long_bitmap()
    {
      assert_choice_type(types::long_bitmap, type_, "ssb-PositionsInBurst-r17");
      return c.get<fixed_bitstring<64>>();
    }
    const fixed_bitstring<4>& short_bitmap() const
    {
      assert_choice_type(types::short_bitmap, type_, "ssb-PositionsInBurst-r17");
      return c.get<fixed_bitstring<4>>();
    }
    const fixed_bitstring<8>& medium_bitmap() const
    {
      assert_choice_type(types::medium_bitmap, type_, "ssb-PositionsInBurst-r17");
      return c.get<fixed_bitstring<8>>();
    }
    const fixed_bitstring<64>& long_bitmap() const
    {
      assert_choice_type(types::long_bitmap, type_, "ssb-PositionsInBurst-r17");
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

  // member variables
  uint8_t                       add_pci_idx_r17 = 1;
  uint16_t                      add_pci_r17     = 0;
  periodicity_r17_e_            periodicity_r17;
  ssb_positions_in_burst_r17_c_ ssb_positions_in_burst_r17;
  int8_t                        ss_pbch_block_pwr_r17 = -60;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SlotFormatIndicator ::= SEQUENCE
struct slot_format_ind_s {
  using slot_format_comb_to_add_mod_list_l_            = dyn_array<slot_format_combinations_per_cell_s>;
  using slot_format_comb_to_release_list_l_            = bounded_array<uint8_t, 16>;
  using available_rb_sets_to_add_mod_list_r16_l_       = dyn_array<available_rb_sets_per_cell_r16_s>;
  using available_rb_sets_to_release_list_r16_l_       = bounded_array<uint8_t, 16>;
  using switch_trigger_to_add_mod_list_r16_l_          = dyn_array<search_space_switch_trigger_r16_s>;
  using switch_trigger_to_release_list_r16_l_          = bounded_array<uint8_t, 4>;
  using co_durs_per_cell_to_add_mod_list_r16_l_        = dyn_array<co_durs_per_cell_r16_s>;
  using co_durs_per_cell_to_release_list_r16_l_        = bounded_array<uint8_t, 16>;
  using switch_trigger_to_add_mod_list_size_ext_r16_l_ = dyn_array<search_space_switch_trigger_r16_s>;
  using switch_trigger_to_release_list_size_ext_r16_l_ = bounded_array<uint8_t, 12>;
  using co_durs_per_cell_to_add_mod_list_r17_l_        = dyn_array<co_durs_per_cell_r17_s>;

  // member variables
  bool                                ext              = false;
  uint32_t                            sfi_rnti         = 0;
  uint8_t                             dci_payload_size = 1;
  slot_format_comb_to_add_mod_list_l_ slot_format_comb_to_add_mod_list;
  slot_format_comb_to_release_list_l_ slot_format_comb_to_release_list;
  // ...
  // group 0
  copy_ptr<available_rb_sets_to_add_mod_list_r16_l_> available_rb_sets_to_add_mod_list_r16;
  copy_ptr<available_rb_sets_to_release_list_r16_l_> available_rb_sets_to_release_list_r16;
  copy_ptr<switch_trigger_to_add_mod_list_r16_l_>    switch_trigger_to_add_mod_list_r16;
  copy_ptr<switch_trigger_to_release_list_r16_l_>    switch_trigger_to_release_list_r16;
  copy_ptr<co_durs_per_cell_to_add_mod_list_r16_l_>  co_durs_per_cell_to_add_mod_list_r16;
  copy_ptr<co_durs_per_cell_to_release_list_r16_l_>  co_durs_per_cell_to_release_list_r16;
  // group 1
  copy_ptr<switch_trigger_to_add_mod_list_size_ext_r16_l_> switch_trigger_to_add_mod_list_size_ext_r16;
  copy_ptr<switch_trigger_to_release_list_size_ext_r16_l_> switch_trigger_to_release_list_size_ext_r16;
  // group 2
  copy_ptr<co_durs_per_cell_to_add_mod_list_r17_l_> co_durs_per_cell_to_add_mod_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UL-DL-SlotConfig ::= SEQUENCE
struct tdd_ul_dl_slot_cfg_s {
  struct symbols_c_ {
    struct explicit_s_ {
      bool    nrof_dl_symbols_present = false;
      bool    nrof_ul_symbols_present = false;
      uint8_t nrof_dl_symbols         = 1;
      uint8_t nrof_ul_symbols         = 1;
    };
    struct types_opts {
      enum options { all_dl, all_ul, explicit_type, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    symbols_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    explicit_s_& explicit_type()
    {
      assert_choice_type(types::explicit_type, type_, "symbols");
      return c;
    }
    const explicit_s_& explicit_type() const
    {
      assert_choice_type(types::explicit_type, type_, "symbols");
      return c;
    }
    void         set_all_dl();
    void         set_all_ul();
    explicit_s_& set_explicit_type();

  private:
    types       type_;
    explicit_s_ c;
  };

  // member variables
  uint16_t   slot_idx = 0;
  symbols_c_ symbols;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UL-DL-SlotConfig-IAB-MT-r16 ::= SEQUENCE
struct tdd_ul_dl_slot_cfg_iab_mt_r16_s {
  struct symbols_iab_mt_r16_c_ {
    struct explicit_r16_s_ {
      bool    nrof_dl_symbols_r16_present = false;
      bool    nrof_ul_symbols_r16_present = false;
      uint8_t nrof_dl_symbols_r16         = 1;
      uint8_t nrof_ul_symbols_r16         = 1;
    };
    struct explicit_iab_mt_r16_s_ {
      bool    nrof_dl_symbols_r16_present = false;
      bool    nrof_ul_symbols_r16_present = false;
      uint8_t nrof_dl_symbols_r16         = 1;
      uint8_t nrof_ul_symbols_r16         = 1;
    };
    struct types_opts {
      enum options { all_dl_r16, all_ul_r16, explicit_r16, explicit_iab_mt_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    symbols_iab_mt_r16_c_() = default;
    symbols_iab_mt_r16_c_(const symbols_iab_mt_r16_c_& other);
    symbols_iab_mt_r16_c_& operator=(const symbols_iab_mt_r16_c_& other);
    ~symbols_iab_mt_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    explicit_r16_s_& explicit_r16()
    {
      assert_choice_type(types::explicit_r16, type_, "symbols-IAB-MT-r16");
      return c.get<explicit_r16_s_>();
    }
    explicit_iab_mt_r16_s_& explicit_iab_mt_r16()
    {
      assert_choice_type(types::explicit_iab_mt_r16, type_, "symbols-IAB-MT-r16");
      return c.get<explicit_iab_mt_r16_s_>();
    }
    const explicit_r16_s_& explicit_r16() const
    {
      assert_choice_type(types::explicit_r16, type_, "symbols-IAB-MT-r16");
      return c.get<explicit_r16_s_>();
    }
    const explicit_iab_mt_r16_s_& explicit_iab_mt_r16() const
    {
      assert_choice_type(types::explicit_iab_mt_r16, type_, "symbols-IAB-MT-r16");
      return c.get<explicit_iab_mt_r16_s_>();
    }
    void                    set_all_dl_r16();
    void                    set_all_ul_r16();
    explicit_r16_s_&        set_explicit_r16();
    explicit_iab_mt_r16_s_& set_explicit_iab_mt_r16();

  private:
    types                                                    type_;
    choice_buffer_t<explicit_iab_mt_r16_s_, explicit_r16_s_> c;

    void destroy_();
  };

  // member variables
  uint16_t              slot_idx_r16 = 0;
  symbols_iab_mt_r16_c_ symbols_iab_mt_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Uplink-powerControl-r17 ::= SEQUENCE
struct ul_pwr_ctrl_r17_s {
  bool               p0_alpha_setfor_pusch_r17_present = false;
  bool               p0_alpha_setfor_pucch_r17_present = false;
  bool               p0_alpha_setfor_srs_r17_present   = false;
  uint8_t            ul_pwrctrl_id_r17                 = 1;
  p0_alpha_set_r17_s p0_alpha_setfor_pusch_r17;
  p0_alpha_set_r17_s p0_alpha_setfor_pucch_r17;
  p0_alpha_set_r17_s p0_alpha_setfor_srs_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkTxSwitching-r16 ::= SEQUENCE
struct ul_tx_switching_r16_s {
  struct ul_tx_switching_carrier_r16_opts {
    enum options { carrier1, carrier2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ul_tx_switching_carrier_r16_e_ = enumerated<ul_tx_switching_carrier_r16_opts>;

  // member variables
  bool                           ul_tx_switching_period_location_r16 = false;
  ul_tx_switching_carrier_r16_e_ ul_tx_switching_carrier_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WithinActiveTimeConfig-r16 ::= SEQUENCE
struct within_active_time_cfg_r16_s {
  bool    first_within_active_time_bwp_id_r16_present   = false;
  bool    dormancy_group_within_active_time_r16_present = false;
  uint8_t first_within_active_time_bwp_id_r16           = 0;
  uint8_t dormancy_group_within_active_time_r16         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ChannelAccessConfig-r16 ::= SEQUENCE
struct ch_access_cfg_r16_s {
  struct energy_detection_cfg_r16_c_ {
    struct types_opts {
      enum options { max_energy_detection_thres_r16, energy_detection_thres_offset_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    energy_detection_cfg_r16_c_() = default;
    energy_detection_cfg_r16_c_(const energy_detection_cfg_r16_c_& other);
    energy_detection_cfg_r16_c_& operator=(const energy_detection_cfg_r16_c_& other);
    ~energy_detection_cfg_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    int8_t& max_energy_detection_thres_r16()
    {
      assert_choice_type(types::max_energy_detection_thres_r16, type_, "energyDetectionConfig-r16");
      return c.get<int8_t>();
    }
    int8_t& energy_detection_thres_offset_r16()
    {
      assert_choice_type(types::energy_detection_thres_offset_r16, type_, "energyDetectionConfig-r16");
      return c.get<int8_t>();
    }
    const int8_t& max_energy_detection_thres_r16() const
    {
      assert_choice_type(types::max_energy_detection_thres_r16, type_, "energyDetectionConfig-r16");
      return c.get<int8_t>();
    }
    const int8_t& energy_detection_thres_offset_r16() const
    {
      assert_choice_type(types::energy_detection_thres_offset_r16, type_, "energyDetectionConfig-r16");
      return c.get<int8_t>();
    }
    int8_t& set_max_energy_detection_thres_r16();
    int8_t& set_energy_detection_thres_offset_r16();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                        energy_detection_cfg_r16_present            = false;
  bool                        ul_to_dl_cot_sharing_ed_thres_r16_present   = false;
  bool                        absence_of_any_other_technology_r16_present = false;
  energy_detection_cfg_r16_c_ energy_detection_cfg_r16;
  int8_t                      ul_to_dl_cot_sharing_ed_thres_r16 = -85;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CrossCarrierSchedulingConfig ::= SEQUENCE
struct cross_carrier_sched_cfg_s {
  struct sched_cell_info_c_ {
    struct own_s_ {
      bool cif_presence = false;
    };
    struct other_s_ {
      uint8_t sched_cell_id     = 0;
      uint8_t cif_in_sched_cell = 1;
    };
    struct types_opts {
      enum options { own, other, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    sched_cell_info_c_() = default;
    sched_cell_info_c_(const sched_cell_info_c_& other);
    sched_cell_info_c_& operator=(const sched_cell_info_c_& other);
    ~sched_cell_info_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    own_s_& own()
    {
      assert_choice_type(types::own, type_, "schedulingCellInfo");
      return c.get<own_s_>();
    }
    other_s_& other()
    {
      assert_choice_type(types::other, type_, "schedulingCellInfo");
      return c.get<other_s_>();
    }
    const own_s_& own() const
    {
      assert_choice_type(types::own, type_, "schedulingCellInfo");
      return c.get<own_s_>();
    }
    const other_s_& other() const
    {
      assert_choice_type(types::other, type_, "schedulingCellInfo");
      return c.get<other_s_>();
    }
    own_s_&   set_own();
    other_s_& set_other();

  private:
    types                             type_;
    choice_buffer_t<other_s_, own_s_> c;

    void destroy_();
  };
  struct carrier_ind_size_r16_s_ {
    uint8_t carrier_ind_size_dci_1_2_r16 = 0;
    uint8_t carrier_ind_size_dci_0_2_r16 = 0;
  };
  struct ccs_blind_detection_split_r17_opts {
    enum options {
      one_seventh,
      three_fourteenth,
      two_seventh,
      three_seventh,
      one_half,
      four_seventh,
      five_seventh,
      spare1,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using ccs_blind_detection_split_r17_e_ = enumerated<ccs_blind_detection_split_r17_opts>;

  // member variables
  bool               ext = false;
  sched_cell_info_c_ sched_cell_info;
  // ...
  // group 0
  bool                              enable_default_beam_for_cc_s_r16_present = false;
  copy_ptr<carrier_ind_size_r16_s_> carrier_ind_size_r16;
  // group 1
  bool                             ccs_blind_detection_split_r17_present = false;
  ccs_blind_detection_split_r17_e_ ccs_blind_detection_split_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DormantBWP-Config-r16 ::= SEQUENCE
struct dormant_bwp_cfg_r16_s {
  bool                                           dormant_bwp_id_r16_present          = false;
  bool                                           within_active_time_cfg_r16_present  = false;
  bool                                           outside_active_time_cfg_r16_present = false;
  uint8_t                                        dormant_bwp_id_r16                  = 0;
  setup_release_c<within_active_time_cfg_r16_s>  within_active_time_cfg_r16;
  setup_release_c<outside_active_time_cfg_r16_s> outside_active_time_cfg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DummyJ ::= SEQUENCE
struct dummy_j_s {
  bool   ul_to_dl_cot_sharing_ed_thres_r16_present   = false;
  bool   absence_of_any_other_technology_r16_present = false;
  int8_t max_energy_detection_thres_r16              = -85;
  int8_t energy_detection_thres_offset_r16           = -20;
  int8_t ul_to_dl_cot_sharing_ed_thres_r16           = -85;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraCellGuardBandsPerSCS-r16 ::= SEQUENCE
struct intra_cell_guard_bands_per_scs_r16_s {
  using intra_cell_guard_bands_r16_l_ = dyn_array<guard_band_r16_s>;

  // member variables
  subcarrier_spacing_e          guard_band_scs_r16;
  intra_cell_guard_bands_r16_l_ intra_cell_guard_bands_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LTE-CRS-PatternList-r16 ::= SEQUENCE (SIZE (1..3)) OF RateMatchPatternLTE-CRS
using lte_crs_pattern_list_r16_l = dyn_array<rate_match_pattern_lte_crs_s>;

// LTE-NeighCellsCRS-AssistInfoList-r17 ::= SEQUENCE (SIZE (1..8)) OF LTE-NeighCellsCRS-AssistInfo-r17
using lte_neigh_cells_crs_assist_info_list_r17_l = dyn_array<lte_neigh_cells_crs_assist_info_r17_s>;

// MIMOParam-r17 ::= SEQUENCE
struct mimo_param_r17_s {
  using add_pci_to_add_mod_list_r17_l_ = dyn_array<ssb_mtc_add_pci_r17_s>;
  using add_pci_to_release_list_r17_l_ = bounded_array<uint8_t, 7>;
  struct unified_tci_state_type_r17_opts {
    enum options { separate, joint, nulltype } value;

    const char* to_string() const;
  };
  using unified_tci_state_type_r17_e_      = enumerated<unified_tci_state_type_r17_opts>;
  using ul_pwr_ctrl_to_add_mod_list_r17_l_ = dyn_array<ul_pwr_ctrl_r17_s>;
  using ul_pwr_ctrl_to_release_list_r17_l_ = dyn_array<uint8_t>;
  struct sfn_schem_epdcch_r17_opts {
    enum options { sfn_scheme_a, sfn_scheme_b, nulltype } value;

    const char* to_string() const;
  };
  using sfn_schem_epdcch_r17_e_ = enumerated<sfn_schem_epdcch_r17_opts>;
  struct sfn_scheme_pdsch_r17_opts {
    enum options { sfn_scheme_a, sfn_scheme_b, nulltype } value;

    const char* to_string() const;
  };
  using sfn_scheme_pdsch_r17_e_ = enumerated<sfn_scheme_pdsch_r17_opts>;

  // member variables
  bool                               unified_tci_state_type_r17_present = false;
  bool                               sfn_schem_epdcch_r17_present       = false;
  bool                               sfn_scheme_pdsch_r17_present       = false;
  add_pci_to_add_mod_list_r17_l_     add_pci_to_add_mod_list_r17;
  add_pci_to_release_list_r17_l_     add_pci_to_release_list_r17;
  unified_tci_state_type_r17_e_      unified_tci_state_type_r17;
  ul_pwr_ctrl_to_add_mod_list_r17_l_ ul_pwr_ctrl_to_add_mod_list_r17;
  ul_pwr_ctrl_to_release_list_r17_l_ ul_pwr_ctrl_to_release_list_r17;
  sfn_schem_epdcch_r17_e_            sfn_schem_epdcch_r17;
  sfn_scheme_pdsch_r17_e_            sfn_scheme_pdsch_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-DL-PRS-PDC-Info-r17 ::= SEQUENCE
struct nr_dl_prs_pdc_info_r17_s {
  bool                        ext                               = false;
  bool                        nr_dl_prs_pdc_res_set_r17_present = false;
  nr_dl_prs_pdc_res_set_r17_s nr_dl_prs_pdc_res_set_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-ServingCellConfig ::= SEQUENCE
struct pdcch_serving_cell_cfg_s {
  bool                               ext                     = false;
  bool                               slot_format_ind_present = false;
  setup_release_c<slot_format_ind_s> slot_format_ind;
  // ...
  // group 0
  bool                                              search_space_switch_timer_r16_present = false;
  copy_ptr<setup_release_c<availability_ind_r16_s>> availability_ind_r16;
  uint8_t                                           search_space_switch_timer_r16 = 1;
  // group 1
  bool     search_space_switch_timer_v1710_present = false;
  uint16_t search_space_switch_timer_v1710         = 81;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-ServingCellConfig ::= SEQUENCE
struct pdsch_serving_cell_cfg_s {
  struct xoverhead_opts {
    enum options { xoh6, xoh12, xoh18, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using xoverhead_e_ = enumerated<xoverhead_opts>;
  struct nrof_harq_processes_for_pdsch_opts {
    enum options { n2, n4, n6, n10, n12, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrof_harq_processes_for_pdsch_e_ = enumerated<nrof_harq_processes_for_pdsch_opts>;

  // member variables
  bool                                         ext                                   = false;
  bool                                         code_block_group_tx_present           = false;
  bool                                         xoverhead_present                     = false;
  bool                                         nrof_harq_processes_for_pdsch_present = false;
  bool                                         pucch_cell_present                    = false;
  setup_release_c<pdsch_code_block_group_tx_s> code_block_group_tx;
  xoverhead_e_                                 xoverhead;
  nrof_harq_processes_for_pdsch_e_             nrof_harq_processes_for_pdsch;
  uint8_t                                      pucch_cell = 0;
  // ...
  // group 0
  bool    max_mimo_layers_present          = false;
  bool    processing_type2_enabled_present = false;
  uint8_t max_mimo_layers                  = 1;
  bool    processing_type2_enabled         = false;
  // group 1
  copy_ptr<setup_release_c<dyn_seq_of<pdsch_code_block_group_tx_s, 1, 2>>> pdsch_code_block_group_tx_list_r16;
  // group 2
  bool                                           nrof_harq_processes_for_pdsch_v1700_present = false;
  copy_ptr<setup_release_c<fixed_bitstring<32>>> dl_harq_feedback_disabled_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SemiStaticChannelAccessConfigUE-r17 ::= SEQUENCE
struct semi_static_ch_access_cfg_ue_r17_s {
  struct period_ue_r17_opts {
    enum options { ms1, ms2, ms2dot5, ms4, ms5, ms10, spare2, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using period_ue_r17_e_ = enumerated<period_ue_r17_opts>;

  // member variables
  period_ue_r17_e_ period_ue_r17;
  uint16_t         offset_ue_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TCI-ActivatedConfig-r17 ::= SEQUENCE
struct tci_activ_cfg_r17_s {
  using pdcch_tci_r17_l_ = bounded_array<uint8_t, 5>;

  // member variables
  pdcch_tci_r17_l_          pdcch_tci_r17;
  bounded_bitstring<1, 128> pdsch_tci_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UL-DL-ConfigDedicated ::= SEQUENCE
struct tdd_ul_dl_cfg_ded_s {
  using slot_specific_cfgs_to_add_mod_list_l_ = dyn_array<tdd_ul_dl_slot_cfg_s>;
  using slot_specific_cfgs_to_release_list_l_ = dyn_array<uint16_t>;

  // member variables
  bool                                  ext = false;
  slot_specific_cfgs_to_add_mod_list_l_ slot_specific_cfgs_to_add_mod_list;
  slot_specific_cfgs_to_release_list_l_ slot_specific_cfgs_to_release_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UL-DL-ConfigDedicated-IAB-MT-r16 ::= SEQUENCE
struct tdd_ul_dl_cfg_ded_iab_mt_r16_s {
  using slot_specific_cfgs_to_add_mod_list_iab_mt_r16_l_ = dyn_array<tdd_ul_dl_slot_cfg_iab_mt_r16_s>;
  using slot_specific_cfgs_to_release_list_iab_mt_r16_l_ = dyn_array<uint16_t>;

  // member variables
  bool                                             ext = false;
  slot_specific_cfgs_to_add_mod_list_iab_mt_r16_l_ slot_specific_cfgs_to_add_mod_list_iab_mt_r16;
  slot_specific_cfgs_to_release_list_iab_mt_r16_l_ slot_specific_cfgs_to_release_list_iab_mt_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkConfig ::= SEQUENCE
struct ul_cfg_s {
  using ul_bwp_to_release_list_l_ = bounded_array<uint8_t, 4>;
  using ul_bwp_to_add_mod_list_l_ = dyn_array<bwp_ul_s>;
  using ul_ch_bw_per_scs_list_l_  = dyn_array<scs_specific_carrier_s>;

  // member variables
  bool                                      ext                            = false;
  bool                                      init_ul_bwp_present            = false;
  bool                                      first_active_ul_bwp_id_present = false;
  bool                                      pusch_serving_cell_cfg_present = false;
  bool                                      carrier_switching_present      = false;
  bwp_ul_ded_s                              init_ul_bwp;
  ul_bwp_to_release_list_l_                 ul_bwp_to_release_list;
  ul_bwp_to_add_mod_list_l_                 ul_bwp_to_add_mod_list;
  uint8_t                                   first_active_ul_bwp_id = 0;
  setup_release_c<pusch_serving_cell_cfg_s> pusch_serving_cell_cfg;
  setup_release_c<srs_carrier_switching_s>  carrier_switching;
  // ...
  // group 0
  bool                               pwr_boost_pi2_bpsk_present = false;
  bool                               pwr_boost_pi2_bpsk         = false;
  copy_ptr<ul_ch_bw_per_scs_list_l_> ul_ch_bw_per_scs_list;
  // group 1
  bool                                             enable_pl_rs_upd_for_pusch_srs_r16_present      = false;
  bool                                             enable_default_beam_pl_for_pusch0_0_r16_present = false;
  bool                                             enable_default_beam_pl_for_pucch_r16_present    = false;
  bool                                             enable_default_beam_pl_for_srs_r16_present      = false;
  bool                                             mpr_pwr_boost_fr2_r16_present                   = false;
  copy_ptr<setup_release_c<ul_tx_switching_r16_s>> ul_tx_switching_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServingCellConfig ::= SEQUENCE
struct serving_cell_cfg_s {
  using dl_bwp_to_release_list_l_ = bounded_array<uint8_t, 4>;
  using dl_bwp_to_add_mod_list_l_ = dyn_array<bwp_dl_s>;
  struct bwp_inactivity_timer_opts {
    enum options {
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
  using bwp_inactivity_timer_e_ = enumerated<bwp_inactivity_timer_opts>;
  struct scell_deactivation_timer_opts {
    enum options {
      ms20,
      ms40,
      ms80,
      ms160,
      ms200,
      ms240,
      ms320,
      ms400,
      ms480,
      ms520,
      ms640,
      ms720,
      ms840,
      ms1280,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using scell_deactivation_timer_e_ = enumerated<scell_deactivation_timer_opts>;
  struct pathloss_ref_linking_opts {
    enum options { sp_cell, scell, nulltype } value;

    const char* to_string() const;
  };
  using pathloss_ref_linking_e_               = enumerated<pathloss_ref_linking_opts>;
  using rate_match_pattern_to_add_mod_list_l_ = dyn_array<rate_match_pattern_s>;
  using rate_match_pattern_to_release_list_l_ = bounded_array<uint8_t, 4>;
  using dl_ch_bw_per_scs_list_l_              = dyn_array<scs_specific_carrier_s>;
  struct ca_slot_offset_r16_c_ {
    struct types_opts {
      enum options { ref_scs15k_hz, ref_scs30_k_hz, ref_scs60_k_hz, ref_scs120_k_hz, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ca_slot_offset_r16_c_() = default;
    ca_slot_offset_r16_c_(const ca_slot_offset_r16_c_& other);
    ca_slot_offset_r16_c_& operator=(const ca_slot_offset_r16_c_& other);
    ~ca_slot_offset_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    int8_t& ref_scs15k_hz()
    {
      assert_choice_type(types::ref_scs15k_hz, type_, "ca-SlotOffset-r16");
      return c.get<int8_t>();
    }
    int8_t& ref_scs30_k_hz()
    {
      assert_choice_type(types::ref_scs30_k_hz, type_, "ca-SlotOffset-r16");
      return c.get<int8_t>();
    }
    int8_t& ref_scs60_k_hz()
    {
      assert_choice_type(types::ref_scs60_k_hz, type_, "ca-SlotOffset-r16");
      return c.get<int8_t>();
    }
    int8_t& ref_scs120_k_hz()
    {
      assert_choice_type(types::ref_scs120_k_hz, type_, "ca-SlotOffset-r16");
      return c.get<int8_t>();
    }
    const int8_t& ref_scs15k_hz() const
    {
      assert_choice_type(types::ref_scs15k_hz, type_, "ca-SlotOffset-r16");
      return c.get<int8_t>();
    }
    const int8_t& ref_scs30_k_hz() const
    {
      assert_choice_type(types::ref_scs30_k_hz, type_, "ca-SlotOffset-r16");
      return c.get<int8_t>();
    }
    const int8_t& ref_scs60_k_hz() const
    {
      assert_choice_type(types::ref_scs60_k_hz, type_, "ca-SlotOffset-r16");
      return c.get<int8_t>();
    }
    const int8_t& ref_scs120_k_hz() const
    {
      assert_choice_type(types::ref_scs120_k_hz, type_, "ca-SlotOffset-r16");
      return c.get<int8_t>();
    }
    int8_t& set_ref_scs15k_hz();
    int8_t& set_ref_scs30_k_hz();
    int8_t& set_ref_scs60_k_hz();
    int8_t& set_ref_scs120_k_hz();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  using intra_cell_guard_bands_dl_list_r16_l_ = dyn_array<intra_cell_guard_bands_per_scs_r16_s>;
  using intra_cell_guard_bands_ul_list_r16_l_ = dyn_array<intra_cell_guard_bands_per_scs_r16_s>;
  struct nrof_harq_bundling_groups_r17_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrof_harq_bundling_groups_r17_e_ = enumerated<nrof_harq_bundling_groups_r17_opts>;

  // member variables
  bool                                      ext                              = false;
  bool                                      tdd_ul_dl_cfg_ded_present        = false;
  bool                                      init_dl_bwp_present              = false;
  bool                                      first_active_dl_bwp_id_present   = false;
  bool                                      bwp_inactivity_timer_present     = false;
  bool                                      default_dl_bwp_id_present        = false;
  bool                                      ul_cfg_present                   = false;
  bool                                      supplementary_ul_present         = false;
  bool                                      pdcch_serving_cell_cfg_present   = false;
  bool                                      pdsch_serving_cell_cfg_present   = false;
  bool                                      csi_meas_cfg_present             = false;
  bool                                      scell_deactivation_timer_present = false;
  bool                                      cross_carrier_sched_cfg_present  = false;
  bool                                      dummy1_present                   = false;
  bool                                      pathloss_ref_linking_present     = false;
  bool                                      serving_cell_mo_present          = false;
  tdd_ul_dl_cfg_ded_s                       tdd_ul_dl_cfg_ded;
  bwp_dl_ded_s                              init_dl_bwp;
  dl_bwp_to_release_list_l_                 dl_bwp_to_release_list;
  dl_bwp_to_add_mod_list_l_                 dl_bwp_to_add_mod_list;
  uint8_t                                   first_active_dl_bwp_id = 0;
  bwp_inactivity_timer_e_                   bwp_inactivity_timer;
  uint8_t                                   default_dl_bwp_id = 0;
  ul_cfg_s                                  ul_cfg;
  ul_cfg_s                                  supplementary_ul;
  setup_release_c<pdcch_serving_cell_cfg_s> pdcch_serving_cell_cfg;
  setup_release_c<pdsch_serving_cell_cfg_s> pdsch_serving_cell_cfg;
  setup_release_c<csi_meas_cfg_s>           csi_meas_cfg;
  scell_deactivation_timer_e_               scell_deactivation_timer;
  cross_carrier_sched_cfg_s                 cross_carrier_sched_cfg;
  uint8_t                                   tag_id = 0;
  pathloss_ref_linking_e_                   pathloss_ref_linking;
  uint8_t                                   serving_cell_mo = 1;
  // ...
  // group 0
  copy_ptr<setup_release_c<rate_match_pattern_lte_crs_s>> lte_crs_to_match_around;
  copy_ptr<rate_match_pattern_to_add_mod_list_l_>         rate_match_pattern_to_add_mod_list;
  copy_ptr<rate_match_pattern_to_release_list_l_>         rate_match_pattern_to_release_list;
  copy_ptr<dl_ch_bw_per_scs_list_l_>                      dl_ch_bw_per_scs_list;
  // group 1
  bool                                             supplementary_ul_release_r16_present                      = false;
  bool                                             csi_rs_validation_with_dci_r16_present                    = false;
  bool                                             crs_rate_match_per_coreset_pool_idx_r16_present           = false;
  bool                                             enable_two_default_tci_states_r16_present                 = false;
  bool                                             enable_default_tci_state_per_coreset_pool_idx_r16_present = false;
  bool                                             enable_beam_switch_timing_r16_present                     = false;
  bool                                             cbg_tx_diff_t_bs_processing_type1_r16_present             = false;
  bool                                             cbg_tx_diff_t_bs_processing_type2_r16_present             = false;
  copy_ptr<tdd_ul_dl_cfg_ded_iab_mt_r16_s>         tdd_ul_dl_cfg_ded_iab_mt_r16;
  copy_ptr<setup_release_c<dormant_bwp_cfg_r16_s>> dormant_bwp_cfg_r16;
  copy_ptr<ca_slot_offset_r16_c_>                  ca_slot_offset_r16;
  copy_ptr<setup_release_c<dummy_j_s>>             dummy2;
  copy_ptr<intra_cell_guard_bands_dl_list_r16_l_>  intra_cell_guard_bands_dl_list_r16;
  copy_ptr<intra_cell_guard_bands_ul_list_r16_l_>  intra_cell_guard_bands_ul_list_r16;
  copy_ptr<setup_release_c<dyn_seq_of<rate_match_pattern_lte_crs_s, 1, 3>>> lte_crs_pattern_list1_r16;
  copy_ptr<setup_release_c<dyn_seq_of<rate_match_pattern_lte_crs_s, 1, 3>>> lte_crs_pattern_list2_r16;
  // group 2
  bool                                           directional_collision_handling_r16_present = false;
  copy_ptr<setup_release_c<ch_access_cfg_r16_s>> ch_access_cfg_r16;
  // group 3
  bool                                                          ch_access_mode2_r17_present                   = false;
  bool                                                          time_domain_harq_bundling_type1_r17_present   = false;
  bool                                                          nrof_harq_bundling_groups_r17_present         = false;
  bool                                                          fdmed_reception_multicast_r17_present         = false;
  bool                                                          more_than_one_nack_only_mode_r17_present      = false;
  bool                                                          directional_collision_handling_dc_r17_present = false;
  copy_ptr<setup_release_c<nr_dl_prs_pdc_info_r17_s>>           nr_dl_prs_pdc_info_r17;
  copy_ptr<setup_release_c<semi_static_ch_access_cfg_ue_r17_s>> semi_static_ch_access_cfg_ue_r17;
  copy_ptr<setup_release_c<mimo_param_r17_s>>                   mimo_param_r17;
  nrof_harq_bundling_groups_r17_e_                              nrof_harq_bundling_groups_r17;
  copy_ptr<tci_activ_cfg_r17_s>                                 tci_activ_cfg_r17;
  copy_ptr<setup_release_c<dyn_seq_of<lte_neigh_cells_crs_assist_info_r17_s, 1, 8>>>
      lte_neigh_cells_crs_assist_info_list_r17;
  // group 4
  bool lte_neigh_cells_crs_assumptions_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1

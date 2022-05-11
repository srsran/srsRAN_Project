/**
 *
 * \section COPYRIGHT
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

#ifndef SRSASN1_RRC_NR_SERVING_CELL_H
#define SRSASN1_RRC_NR_SERVING_CELL_H

#include "common.h"

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

// NR-NS-PmaxList ::= SEQUENCE (SIZE (1..8)) OF NR-NS-PmaxValue
using nr_ns_pmax_list_l = dyn_array<nr_ns_pmax_value_s>;

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

// PDSCH-TimeDomainResourceAllocation ::= SEQUENCE
struct pdsch_time_domain_res_alloc_s {
  struct map_type_opts {
    enum options { type_a, type_b, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<map_type_opts> map_type_e_;

  // member variables
  bool        k0_present = false;
  uint8_t     k0         = 0;
  map_type_e_ map_type;
  uint8_t     start_symbol_and_len = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-TimeDomainResourceAllocation ::= SEQUENCE
struct pusch_time_domain_res_alloc_s {
  struct map_type_opts {
    enum options { type_a, type_b, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<map_type_opts> map_type_e_;

  // member variables
  bool        k2_present = false;
  uint8_t     k2         = 0;
  map_type_e_ map_type;
  uint8_t     start_symbol_and_len = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ControlResourceSet ::= SEQUENCE
struct coreset_s {
  struct cce_reg_map_type_c_ {
    struct interleaved_s_ {
      struct reg_bundle_size_opts {
        enum options { n2, n3, n6, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<reg_bundle_size_opts> reg_bundle_size_e_;
      struct interleaver_size_opts {
        enum options { n2, n3, n6, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<interleaver_size_opts> interleaver_size_e_;

      // member variables
      bool                shift_idx_present = false;
      reg_bundle_size_e_  reg_bundle_size;
      interleaver_size_e_ interleaver_size;
      uint16_t            shift_idx = 0;
    };
    struct types_opts {
      enum options { interleaved, non_interleaved, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    cce_reg_map_type_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    interleaved_s_& interleaved()
    {
      assert_choice_type(types::interleaved, type_, "cce-REG-MappingType");
      return c;
    }
    const interleaved_s_& interleaved() const
    {
      assert_choice_type(types::interleaved, type_, "cce-REG-MappingType");
      return c;
    }
    interleaved_s_& set_interleaved();
    void            set_non_interleaved();

  private:
    types          type_;
    interleaved_s_ c;
  };
  struct precoder_granularity_opts {
    enum options { same_as_reg_bundle, all_contiguous_rbs, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<precoder_granularity_opts> precoder_granularity_e_;
  using tci_states_pdcch_to_add_list_l_     = dyn_array<uint8_t>;
  using tci_states_pdcch_to_release_list_l_ = dyn_array<uint8_t>;

  // member variables
  bool                                ext                              = false;
  bool                                tci_present_in_dci_present       = false;
  bool                                pdcch_dmrs_scrambling_id_present = false;
  uint8_t                             coreset_id                       = 0;
  fixed_bitstring<45>                 freq_domain_res;
  uint8_t                             dur = 1;
  cce_reg_map_type_c_                 cce_reg_map_type;
  precoder_granularity_e_             precoder_granularity;
  tci_states_pdcch_to_add_list_l_     tci_states_pdcch_to_add_list;
  tci_states_pdcch_to_release_list_l_ tci_states_pdcch_to_release_list;
  uint32_t                            pdcch_dmrs_scrambling_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MultiFrequencyBandListNR-SIB ::= SEQUENCE (SIZE (1..8)) OF NR-MultiBandInfo
using multi_freq_band_list_nr_sib_l = dyn_array<nr_multi_band_info_s>;

// PDSCH-TimeDomainResourceAllocationList ::= SEQUENCE (SIZE (1..16)) OF PDSCH-TimeDomainResourceAllocation
using pdsch_time_domain_res_alloc_list_l = dyn_array<pdsch_time_domain_res_alloc_s>;

// PUSCH-TimeDomainResourceAllocationList ::= SEQUENCE (SIZE (1..16)) OF PUSCH-TimeDomainResourceAllocation
using pusch_time_domain_res_alloc_list_l = dyn_array<pusch_time_domain_res_alloc_s>;

// RACH-ConfigGeneric ::= SEQUENCE
struct rach_cfg_generic_s {
  struct msg1_fdm_opts {
    enum options { one, two, four, eight, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<msg1_fdm_opts> msg1_fdm_e_;
  struct preamb_trans_max_opts {
    enum options { n3, n4, n5, n6, n7, n8, n10, n20, n50, n100, n200, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<preamb_trans_max_opts> preamb_trans_max_e_;
  struct pwr_ramp_step_opts {
    enum options { db0, db2, db4, db6, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pwr_ramp_step_opts> pwr_ramp_step_e_;
  struct ra_resp_win_opts {
    enum options { sl1, sl2, sl4, sl8, sl10, sl20, sl40, sl80, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ra_resp_win_opts> ra_resp_win_e_;

  // member variables
  bool                ext           = false;
  uint16_t            prach_cfg_idx = 0;
  msg1_fdm_e_         msg1_fdm;
  uint16_t            msg1_freq_start           = 0;
  uint8_t             zero_correlation_zone_cfg = 0;
  int16_t             preamb_rx_target_pwr      = -202;
  preamb_trans_max_e_ preamb_trans_max;
  pwr_ramp_step_e_    pwr_ramp_step;
  ra_resp_win_e_      ra_resp_win;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SearchSpace ::= SEQUENCE
struct search_space_s {
  struct monitoring_slot_periodicity_and_offset_c_ {
    struct types_opts {
      enum options {
        sl1,
        sl2,
        sl4,
        sl5,
        sl8,
        sl10,
        sl16,
        sl20,
        sl40,
        sl80,
        sl160,
        sl320,
        sl640,
        sl1280,
        sl2560,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    monitoring_slot_periodicity_and_offset_c_() = default;
    monitoring_slot_periodicity_and_offset_c_(const monitoring_slot_periodicity_and_offset_c_& other);
    monitoring_slot_periodicity_and_offset_c_& operator=(const monitoring_slot_periodicity_and_offset_c_& other);
    ~monitoring_slot_periodicity_and_offset_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sl2()
    {
      assert_choice_type(types::sl2, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl4()
    {
      assert_choice_type(types::sl4, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl5()
    {
      assert_choice_type(types::sl5, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl8()
    {
      assert_choice_type(types::sl8, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl10()
    {
      assert_choice_type(types::sl10, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl16()
    {
      assert_choice_type(types::sl16, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl20()
    {
      assert_choice_type(types::sl20, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl40()
    {
      assert_choice_type(types::sl40, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl80()
    {
      assert_choice_type(types::sl80, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl160()
    {
      assert_choice_type(types::sl160, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint16_t& sl320()
    {
      assert_choice_type(types::sl320, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    uint16_t& sl640()
    {
      assert_choice_type(types::sl640, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    uint16_t& sl1280()
    {
      assert_choice_type(types::sl1280, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    uint16_t& sl2560()
    {
      assert_choice_type(types::sl2560, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint8_t& sl2() const
    {
      assert_choice_type(types::sl2, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl4() const
    {
      assert_choice_type(types::sl4, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl5() const
    {
      assert_choice_type(types::sl5, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl8() const
    {
      assert_choice_type(types::sl8, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl10() const
    {
      assert_choice_type(types::sl10, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl16() const
    {
      assert_choice_type(types::sl16, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl20() const
    {
      assert_choice_type(types::sl20, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl40() const
    {
      assert_choice_type(types::sl40, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl80() const
    {
      assert_choice_type(types::sl80, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl160() const
    {
      assert_choice_type(types::sl160, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint16_t& sl320() const
    {
      assert_choice_type(types::sl320, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& sl640() const
    {
      assert_choice_type(types::sl640, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& sl1280() const
    {
      assert_choice_type(types::sl1280, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& sl2560() const
    {
      assert_choice_type(types::sl2560, type_, "monitoringSlotPeriodicityAndOffset");
      return c.get<uint16_t>();
    }
    void      set_sl1();
    uint8_t&  set_sl2();
    uint8_t&  set_sl4();
    uint8_t&  set_sl5();
    uint8_t&  set_sl8();
    uint8_t&  set_sl10();
    uint8_t&  set_sl16();
    uint8_t&  set_sl20();
    uint8_t&  set_sl40();
    uint8_t&  set_sl80();
    uint8_t&  set_sl160();
    uint16_t& set_sl320();
    uint16_t& set_sl640();
    uint16_t& set_sl1280();
    uint16_t& set_sl2560();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct nrof_candidates_s_ {
    struct aggregation_level1_opts {
      enum options { n0, n1, n2, n3, n4, n5, n6, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<aggregation_level1_opts> aggregation_level1_e_;
    struct aggregation_level2_opts {
      enum options { n0, n1, n2, n3, n4, n5, n6, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<aggregation_level2_opts> aggregation_level2_e_;
    struct aggregation_level4_opts {
      enum options { n0, n1, n2, n3, n4, n5, n6, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<aggregation_level4_opts> aggregation_level4_e_;
    struct aggregation_level8_opts {
      enum options { n0, n1, n2, n3, n4, n5, n6, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<aggregation_level8_opts> aggregation_level8_e_;
    struct aggregation_level16_opts {
      enum options { n0, n1, n2, n3, n4, n5, n6, n8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<aggregation_level16_opts> aggregation_level16_e_;

    // member variables
    aggregation_level1_e_  aggregation_level1;
    aggregation_level2_e_  aggregation_level2;
    aggregation_level4_e_  aggregation_level4;
    aggregation_level8_e_  aggregation_level8;
    aggregation_level16_e_ aggregation_level16;
  };
  struct search_space_type_c_ {
    struct common_s_ {
      struct dci_format0_minus0_and_format1_minus0_s_ {
        bool ext = false;
        // ...
      };
      struct dci_format2_minus0_s_ {
        struct nrof_candidates_sfi_s_ {
          struct aggregation_level1_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          typedef enumerated<aggregation_level1_opts> aggregation_level1_e_;
          struct aggregation_level2_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          typedef enumerated<aggregation_level2_opts> aggregation_level2_e_;
          struct aggregation_level4_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          typedef enumerated<aggregation_level4_opts> aggregation_level4_e_;
          struct aggregation_level8_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          typedef enumerated<aggregation_level8_opts> aggregation_level8_e_;
          struct aggregation_level16_opts {
            enum options { n1, n2, nulltype } value;
            typedef uint8_t number_type;

            const char* to_string() const;
            uint8_t     to_number() const;
          };
          typedef enumerated<aggregation_level16_opts> aggregation_level16_e_;

          // member variables
          bool                   aggregation_level1_present  = false;
          bool                   aggregation_level2_present  = false;
          bool                   aggregation_level4_present  = false;
          bool                   aggregation_level8_present  = false;
          bool                   aggregation_level16_present = false;
          aggregation_level1_e_  aggregation_level1;
          aggregation_level2_e_  aggregation_level2;
          aggregation_level4_e_  aggregation_level4;
          aggregation_level8_e_  aggregation_level8;
          aggregation_level16_e_ aggregation_level16;
        };

        // member variables
        bool                   ext = false;
        nrof_candidates_sfi_s_ nrof_candidates_sfi;
        // ...
      };
      struct dci_format2_minus1_s_ {
        bool ext = false;
        // ...
      };
      struct dci_format2_minus2_s_ {
        bool ext = false;
        // ...
      };
      struct dci_format2_minus3_s_ {
        struct dummy1_opts {
          enum options { sl1, sl2, sl4, sl5, sl8, sl10, sl16, sl20, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<dummy1_opts> dummy1_e_;
        struct dummy2_opts {
          enum options { n1, n2, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<dummy2_opts> dummy2_e_;

        // member variables
        bool      ext            = false;
        bool      dummy1_present = false;
        dummy1_e_ dummy1;
        dummy2_e_ dummy2;
        // ...
      };

      // member variables
      bool                                     dci_format0_minus0_and_format1_minus0_present = false;
      bool                                     dci_format2_minus0_present                    = false;
      bool                                     dci_format2_minus1_present                    = false;
      bool                                     dci_format2_minus2_present                    = false;
      bool                                     dci_format2_minus3_present                    = false;
      dci_format0_minus0_and_format1_minus0_s_ dci_format0_minus0_and_format1_minus0;
      dci_format2_minus0_s_                    dci_format2_minus0;
      dci_format2_minus1_s_                    dci_format2_minus1;
      dci_format2_minus2_s_                    dci_format2_minus2;
      dci_format2_minus3_s_                    dci_format2_minus3;
    };
    struct ue_specific_s_ {
      struct dci_formats_opts {
        enum options { formats0_minus0_and_minus1_minus0, formats0_minus1_and_minus1_minus1, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<dci_formats_opts> dci_formats_e_;

      // member variables
      bool           ext = false;
      dci_formats_e_ dci_formats;
      // ...
    };
    struct types_opts {
      enum options { common, ue_specific, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    search_space_type_c_() = default;
    search_space_type_c_(const search_space_type_c_& other);
    search_space_type_c_& operator=(const search_space_type_c_& other);
    ~search_space_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    common_s_& common()
    {
      assert_choice_type(types::common, type_, "searchSpaceType");
      return c.get<common_s_>();
    }
    ue_specific_s_& ue_specific()
    {
      assert_choice_type(types::ue_specific, type_, "searchSpaceType");
      return c.get<ue_specific_s_>();
    }
    const common_s_& common() const
    {
      assert_choice_type(types::common, type_, "searchSpaceType");
      return c.get<common_s_>();
    }
    const ue_specific_s_& ue_specific() const
    {
      assert_choice_type(types::ue_specific, type_, "searchSpaceType");
      return c.get<ue_specific_s_>();
    }
    common_s_&      set_common();
    ue_specific_s_& set_ue_specific();

  private:
    types                                      type_;
    choice_buffer_t<common_s_, ue_specific_s_> c;

    void destroy_();
  };

  // member variables
  bool                                      coreset_id_present                             = false;
  bool                                      monitoring_slot_periodicity_and_offset_present = false;
  bool                                      dur_present                                    = false;
  bool                                      monitoring_symbols_within_slot_present         = false;
  bool                                      nrof_candidates_present                        = false;
  bool                                      search_space_type_present                      = false;
  uint8_t                                   search_space_id                                = 0;
  uint8_t                                   coreset_id                                     = 0;
  monitoring_slot_periodicity_and_offset_c_ monitoring_slot_periodicity_and_offset;
  uint16_t                                  dur = 2;
  fixed_bitstring<14>                       monitoring_symbols_within_slot;
  nrof_candidates_s_                        nrof_candidates;
  search_space_type_c_                      search_space_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP ::= SEQUENCE
struct bwp_s {
  bool                 cp_present      = false;
  uint16_t             location_and_bw = 0;
  subcarrier_spacing_e subcarrier_spacing;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-ConfigCommon ::= SEQUENCE
struct pdcch_cfg_common_s {
  using common_search_space_list_l_ = dyn_array<search_space_s>;
  struct first_pdcch_monitoring_occasion_of_po_c_ {
    using scs15_kh_zone_t_l_                                                              = bounded_array<uint8_t, 4>;
    using scs30_kh_zone_t_scs15_kh_zhalf_t_l_                                             = bounded_array<uint16_t, 4>;
    using scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_                         = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_ = bounded_array<uint16_t, 4>;
    using scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_ =
        bounded_array<uint16_t, 4>;
    using scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_ = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_                     = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_sixteenth_t_l_                                            = bounded_array<uint16_t, 4>;
    struct types_opts {
      enum options {
        scs15_kh_zone_t,
        scs30_kh_zone_t_scs15_kh_zhalf_t,
        scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t,
        scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t,
        scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t,
        scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
        scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t,
        scs120_kh_zone_sixteenth_t,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t()
    {
      assert_choice_type(types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_& scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t()
    {
      assert_choice_type(
          types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zone_sixteenth_t_l_& scs120_kh_zone_sixteenth_t()
    {
      assert_choice_type(types::scs120_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_sixteenth_t_l_>();
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
    const scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t() const
    {
      assert_choice_type(types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
    }
    const scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t() const
    {
      assert_choice_type(
          types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
    }
    const scs120_kh_zone_sixteenth_t_l_& scs120_kh_zone_sixteenth_t() const
    {
      assert_choice_type(types::scs120_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_sixteenth_t_l_>();
    }
    scs15_kh_zone_t_l_&                                      set_scs15_kh_zone_t();
    scs30_kh_zone_t_scs15_kh_zhalf_t_l_&                     set_scs30_kh_zone_t_scs15_kh_zhalf_t();
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_& set_scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t();
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    set_scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t();
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    set_scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t();
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
                                                          set_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t();
    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_& set_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t();
    scs120_kh_zone_sixteenth_t_l_&                        set_scs120_kh_zone_sixteenth_t();

  private:
    types type_;
    choice_buffer_t<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_,
                    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_,
                    scs15_kh_zone_t_l_,
                    scs30_kh_zone_t_scs15_kh_zhalf_t_l_,
                    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>
        c;

    void destroy_();
  };

  // member variables
  bool                        ext                                 = false;
  bool                        coreset_zero_present                = false;
  bool                        common_coreset_present              = false;
  bool                        search_space_zero_present           = false;
  bool                        search_space_sib1_present           = false;
  bool                        search_space_other_sys_info_present = false;
  bool                        paging_search_space_present         = false;
  bool                        ra_search_space_present             = false;
  uint8_t                     coreset_zero                        = 0;
  coreset_s                   common_coreset;
  uint8_t                     search_space_zero = 0;
  common_search_space_list_l_ common_search_space_list;
  uint8_t                     search_space_sib1           = 0;
  uint8_t                     search_space_other_sys_info = 0;
  uint8_t                     paging_search_space         = 0;
  uint8_t                     ra_search_space             = 0;
  // ...
  // group 0
  copy_ptr<first_pdcch_monitoring_occasion_of_po_c_> first_pdcch_monitoring_occasion_of_po;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-ConfigCommon ::= SEQUENCE
struct pdsch_cfg_common_s {
  bool                               ext = false;
  pdsch_time_domain_res_alloc_list_l pdsch_time_domain_alloc_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-ConfigCommon ::= SEQUENCE
struct pucch_cfg_common_s {
  struct pucch_group_hop_opts {
    enum options { neither, enable, disable, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<pucch_group_hop_opts> pucch_group_hop_e_;

  // member variables
  bool               ext                      = false;
  bool               pucch_res_common_present = false;
  bool               hop_id_present           = false;
  bool               p0_nominal_present       = false;
  uint8_t            pucch_res_common         = 0;
  pucch_group_hop_e_ pucch_group_hop;
  uint16_t           hop_id     = 0;
  int16_t            p0_nominal = -202;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-ConfigCommon ::= SEQUENCE
struct pusch_cfg_common_s {
  bool                               ext                                           = false;
  bool                               group_hop_enabled_transform_precoding_present = false;
  bool                               msg3_delta_preamb_present                     = false;
  bool                               p0_nominal_with_grant_present                 = false;
  pusch_time_domain_res_alloc_list_l pusch_time_domain_alloc_list;
  int8_t                             msg3_delta_preamb     = -1;
  int16_t                            p0_nominal_with_grant = -202;
  // ...

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
typedef enumerated<paging_cycle_opts> paging_cycle_e;

// RACH-ConfigCommon ::= SEQUENCE
struct rach_cfg_common_s {
  struct ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_ {
    struct one_eighth_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<one_eighth_opts> one_eighth_e_;
    struct one_fourth_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<one_fourth_opts> one_fourth_e_;
    struct one_half_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<one_half_opts> one_half_e_;
    struct one_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<one_opts> one_e_;
    struct two_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<two_opts> two_e_;
    struct types_opts {
      enum options { one_eighth, one_fourth, one_half, one, two, four, eight, sixteen, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_() = default;
    ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_(const ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_& other);
    ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_&
    operator=(const ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_& other);
    ~ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    one_eighth_e_& one_eighth()
    {
      assert_choice_type(types::one_eighth, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_eighth_e_>();
    }
    one_fourth_e_& one_fourth()
    {
      assert_choice_type(types::one_fourth, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_fourth_e_>();
    }
    one_half_e_& one_half()
    {
      assert_choice_type(types::one_half, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_half_e_>();
    }
    one_e_& one()
    {
      assert_choice_type(types::one, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_e_>();
    }
    two_e_& two()
    {
      assert_choice_type(types::two, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<two_e_>();
    }
    uint8_t& four()
    {
      assert_choice_type(types::four, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    uint8_t& eight()
    {
      assert_choice_type(types::eight, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    uint8_t& sixteen()
    {
      assert_choice_type(types::sixteen, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    const one_eighth_e_& one_eighth() const
    {
      assert_choice_type(types::one_eighth, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_eighth_e_>();
    }
    const one_fourth_e_& one_fourth() const
    {
      assert_choice_type(types::one_fourth, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_fourth_e_>();
    }
    const one_half_e_& one_half() const
    {
      assert_choice_type(types::one_half, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_half_e_>();
    }
    const one_e_& one() const
    {
      assert_choice_type(types::one, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<one_e_>();
    }
    const two_e_& two() const
    {
      assert_choice_type(types::two, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<two_e_>();
    }
    const uint8_t& four() const
    {
      assert_choice_type(types::four, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    const uint8_t& eight() const
    {
      assert_choice_type(types::eight, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    const uint8_t& sixteen() const
    {
      assert_choice_type(types::sixteen, type_, "ssb-perRACH-OccasionAndCB-PreamblesPerSSB");
      return c.get<uint8_t>();
    }
    one_eighth_e_& set_one_eighth();
    one_fourth_e_& set_one_fourth();
    one_half_e_&   set_one_half();
    one_e_&        set_one();
    two_e_&        set_two();
    uint8_t&       set_four();
    uint8_t&       set_eight();
    uint8_t&       set_sixteen();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct group_bcfgured_s_ {
    struct ra_msg3_size_group_a_opts {
      enum options {
        b56,
        b144,
        b208,
        b256,
        b282,
        b480,
        b640,
        b800,
        b1000,
        b72,
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
    typedef enumerated<ra_msg3_size_group_a_opts> ra_msg3_size_group_a_e_;
    struct msg_pwr_offset_group_b_opts {
      enum options { minusinfinity, db0, db5, db8, db10, db12, db15, db18, nulltype } value;
      typedef int8_t number_type;

      const char* to_string() const;
      int8_t      to_number() const;
    };
    typedef enumerated<msg_pwr_offset_group_b_opts> msg_pwr_offset_group_b_e_;

    // member variables
    ra_msg3_size_group_a_e_   ra_msg3_size_group_a;
    msg_pwr_offset_group_b_e_ msg_pwr_offset_group_b;
    uint8_t                   nof_ra_preambs_group_a = 1;
  };
  struct ra_contention_resolution_timer_opts {
    enum options { sf8, sf16, sf24, sf32, sf40, sf48, sf56, sf64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ra_contention_resolution_timer_opts> ra_contention_resolution_timer_e_;
  struct prach_root_seq_idx_c_ {
    struct types_opts {
      enum options { l839, l139, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    prach_root_seq_idx_c_() = default;
    prach_root_seq_idx_c_(const prach_root_seq_idx_c_& other);
    prach_root_seq_idx_c_& operator=(const prach_root_seq_idx_c_& other);
    ~prach_root_seq_idx_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t& l839()
    {
      assert_choice_type(types::l839, type_, "prach-RootSequenceIndex");
      return c.get<uint16_t>();
    }
    uint8_t& l139()
    {
      assert_choice_type(types::l139, type_, "prach-RootSequenceIndex");
      return c.get<uint8_t>();
    }
    const uint16_t& l839() const
    {
      assert_choice_type(types::l839, type_, "prach-RootSequenceIndex");
      return c.get<uint16_t>();
    }
    const uint8_t& l139() const
    {
      assert_choice_type(types::l139, type_, "prach-RootSequenceIndex");
      return c.get<uint8_t>();
    }
    uint16_t& set_l839();
    uint8_t&  set_l139();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct restricted_set_cfg_opts {
    enum options { unrestricted_set, restricted_set_type_a, restricted_set_type_b, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<restricted_set_cfg_opts> restricted_set_cfg_e_;

  // member variables
  bool                                            ext                                                  = false;
  bool                                            total_nof_ra_preambs_present                         = false;
  bool                                            ssb_per_rach_occasion_and_cb_preambs_per_ssb_present = false;
  bool                                            group_bcfgured_present                               = false;
  bool                                            rsrp_thres_ssb_present                               = false;
  bool                                            rsrp_thres_ssb_sul_present                           = false;
  bool                                            msg1_subcarrier_spacing_present                      = false;
  bool                                            msg3_transform_precoder_present                      = false;
  rach_cfg_generic_s                              rach_cfg_generic;
  uint8_t                                         total_nof_ra_preambs = 1;
  ssb_per_rach_occasion_and_cb_preambs_per_ssb_c_ ssb_per_rach_occasion_and_cb_preambs_per_ssb;
  group_bcfgured_s_                               group_bcfgured;
  ra_contention_resolution_timer_e_               ra_contention_resolution_timer;
  uint8_t                                         rsrp_thres_ssb     = 0;
  uint8_t                                         rsrp_thres_ssb_sul = 0;
  prach_root_seq_idx_c_                           prach_root_seq_idx;
  subcarrier_spacing_e                            msg1_subcarrier_spacing;
  restricted_set_cfg_e_                           restricted_set_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// BCCH-Config ::= SEQUENCE
struct bcch_cfg_s {
  struct mod_period_coeff_opts {
    enum options { n2, n4, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<mod_period_coeff_opts> mod_period_coeff_e_;

  // member variables
  bool                ext = false;
  mod_period_coeff_e_ mod_period_coeff;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-DownlinkCommon ::= SEQUENCE
struct bwp_dl_common_s {
  bool                                ext                      = false;
  bool                                pdcch_cfg_common_present = false;
  bool                                pdsch_cfg_common_present = false;
  bwp_s                               generic_params;
  setup_release_c<pdcch_cfg_common_s> pdcch_cfg_common;
  setup_release_c<pdsch_cfg_common_s> pdsch_cfg_common;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-UplinkCommon ::= SEQUENCE
struct bwp_ul_common_s {
  bool                                ext                      = false;
  bool                                rach_cfg_common_present  = false;
  bool                                pusch_cfg_common_present = false;
  bool                                pucch_cfg_common_present = false;
  bwp_s                               generic_params;
  setup_release_c<rach_cfg_common_s>  rach_cfg_common;
  setup_release_c<pusch_cfg_common_s> pusch_cfg_common;
  setup_release_c<pucch_cfg_common_s> pucch_cfg_common;
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
    typedef enumerated<types_opts> types;

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
  typedef enumerated<ns_opts> ns_e_;
  struct first_pdcch_monitoring_occasion_of_po_c_ {
    using scs15_kh_zone_t_l_                                                              = bounded_array<uint8_t, 4>;
    using scs30_kh_zone_t_scs15_kh_zhalf_t_l_                                             = bounded_array<uint16_t, 4>;
    using scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_                         = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_ = bounded_array<uint16_t, 4>;
    using scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_ =
        bounded_array<uint16_t, 4>;
    using scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_ = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_                     = bounded_array<uint16_t, 4>;
    using scs120_kh_zone_sixteenth_t_l_                                            = bounded_array<uint16_t, 4>;
    struct types_opts {
      enum options {
        scs15_kh_zone_t,
        scs30_kh_zone_t_scs15_kh_zhalf_t,
        scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t,
        scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t,
        scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t,
        scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
        scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t,
        scs120_kh_zone_sixteenth_t,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t()
    {
      assert_choice_type(types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_& scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t()
    {
      assert_choice_type(
          types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
    }
    scs120_kh_zone_sixteenth_t_l_& scs120_kh_zone_sixteenth_t()
    {
      assert_choice_type(types::scs120_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_sixteenth_t_l_>();
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
    const scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t() const
    {
      assert_choice_type(types::scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t,
                         type_,
                         "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
    }
    const scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t() const
    {
      assert_choice_type(
          types::scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
    }
    const scs120_kh_zone_sixteenth_t_l_& scs120_kh_zone_sixteenth_t() const
    {
      assert_choice_type(types::scs120_kh_zone_sixteenth_t, type_, "firstPDCCH-MonitoringOccasionOfPO");
      return c.get<scs120_kh_zone_sixteenth_t_l_>();
    }
    scs15_kh_zone_t_l_&                                      set_scs15_kh_zone_t();
    scs30_kh_zone_t_scs15_kh_zhalf_t_l_&                     set_scs30_kh_zone_t_scs15_kh_zhalf_t();
    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_& set_scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t();
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    set_scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t();
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    set_scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t();
    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
                                                          set_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t();
    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_& set_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t();
    scs120_kh_zone_sixteenth_t_l_&                        set_scs120_kh_zone_sixteenth_t();

  private:
    types type_;
    choice_buffer_t<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_sixteenth_t_l_,
                    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_,
                    scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_,
                    scs15_kh_zone_t_l_,
                    scs30_kh_zone_t_scs15_kh_zhalf_t_l_,
                    scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>
        c;

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
  typedef enumerated<dl_ul_tx_periodicity_opts> dl_ul_tx_periodicity_e_;
  struct dl_ul_tx_periodicity_v1530_opts {
    enum options { ms3, ms4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dl_ul_tx_periodicity_v1530_opts> dl_ul_tx_periodicity_v1530_e_;

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
typedef enumerated<time_align_timer_opts> time_align_timer_e;

// DownlinkConfigCommonSIB ::= SEQUENCE
struct dl_cfg_common_sib_s {
  bool               ext = false;
  freq_info_dl_sib_s freq_info_dl;
  bwp_dl_common_s    init_dl_bwp;
  bcch_cfg_s         bcch_cfg;
  pcch_cfg_s         pcch_cfg;
  // ...

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

// ServingCellConfigCommonSIB ::= SEQUENCE
struct serving_cell_cfg_common_sib_s {
  struct n_timing_advance_offset_opts {
    enum options { n0, n25600, n39936, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<n_timing_advance_offset_opts> n_timing_advance_offset_e_;
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
  typedef enumerated<ssb_periodicity_serving_cell_opts> ssb_periodicity_serving_cell_e_;

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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BFR-CSIRS-Resource ::= SEQUENCE
struct bfr_csirs_res_s {
  using ra_occasion_list_l_ = dyn_array<uint16_t>;

  // member variables
  bool                ext                   = false;
  bool                ra_preamb_idx_present = false;
  uint8_t             csi_rs                = 0;
  ra_occasion_list_l_ ra_occasion_list;
  uint8_t             ra_preamb_idx = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BFR-SSB-Resource ::= SEQUENCE
struct bfr_ssb_res_s {
  bool    ext           = false;
  uint8_t ssb           = 0;
  uint8_t ra_preamb_idx = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-FrequencyOccupation ::= SEQUENCE
struct csi_freq_occupation_s {
  bool     ext      = false;
  uint16_t start_rb = 0;
  uint16_t nrof_rbs = 24;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-RS-ResourceMapping ::= SEQUENCE
struct csi_rs_res_map_s {
  struct freq_domain_alloc_c_ {
    struct types_opts {
      enum options { row1, row2, row4, other, nulltype } value;
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
    fixed_bitstring<3>& row4()
    {
      assert_choice_type(types::row4, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<3> >();
    }
    fixed_bitstring<6>& other()
    {
      assert_choice_type(types::other, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<6> >();
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
    const fixed_bitstring<3>& row4() const
    {
      assert_choice_type(types::row4, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<3> >();
    }
    const fixed_bitstring<6>& other() const
    {
      assert_choice_type(types::other, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<6> >();
    }
    fixed_bitstring<4>&  set_row1();
    fixed_bitstring<12>& set_row2();
    fixed_bitstring<3>&  set_row4();
    fixed_bitstring<6>&  set_other();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<12> > c;

    void destroy_();
  };
  struct nrof_ports_opts {
    enum options { p1, p2, p4, p8, p12, p16, p24, p32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nrof_ports_opts> nrof_ports_e_;
  struct cdm_type_opts {
    enum options { no_cdm, fd_cdm2, cdm4_fd2_td2, cdm8_fd2_td4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<cdm_type_opts> cdm_type_e_;
  struct density_c_ {
    struct dot5_opts {
      enum options { even_prbs, odd_prbs, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<dot5_opts> dot5_e_;
    struct types_opts {
      enum options { dot5, one, three, spare, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    density_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dot5_e_& dot5()
    {
      assert_choice_type(types::dot5, type_, "density");
      return c;
    }
    const dot5_e_& dot5() const
    {
      assert_choice_type(types::dot5, type_, "density");
      return c;
    }
    dot5_e_& set_dot5();
    void     set_one();
    void     set_three();
    void     set_spare();

  private:
    types   type_;
    dot5_e_ c;
  };

  // member variables
  bool                  ext                                       = false;
  bool                  first_ofdm_symbol_in_time_domain2_present = false;
  freq_domain_alloc_c_  freq_domain_alloc;
  nrof_ports_e_         nrof_ports;
  uint8_t               first_ofdm_symbol_in_time_domain  = 0;
  uint8_t               first_ofdm_symbol_in_time_domain2 = 2;
  cdm_type_e_           cdm_type;
  density_c_            density;
  csi_freq_occupation_s freq_band;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-ResourcePeriodicityAndOffset ::= CHOICE
struct csi_res_periodicity_and_offset_c {
  struct types_opts {
    enum options {
      slots4,
      slots5,
      slots8,
      slots10,
      slots16,
      slots20,
      slots32,
      slots40,
      slots64,
      slots80,
      slots160,
      slots320,
      slots640,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  csi_res_periodicity_and_offset_c() = default;
  csi_res_periodicity_and_offset_c(const csi_res_periodicity_and_offset_c& other);
  csi_res_periodicity_and_offset_c& operator=(const csi_res_periodicity_and_offset_c& other);
  ~csi_res_periodicity_and_offset_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& slots4()
  {
    assert_choice_type(types::slots4, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots5()
  {
    assert_choice_type(types::slots5, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots8()
  {
    assert_choice_type(types::slots8, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots10()
  {
    assert_choice_type(types::slots10, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots16()
  {
    assert_choice_type(types::slots16, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots20()
  {
    assert_choice_type(types::slots20, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots32()
  {
    assert_choice_type(types::slots32, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots40()
  {
    assert_choice_type(types::slots40, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots64()
  {
    assert_choice_type(types::slots64, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots80()
  {
    assert_choice_type(types::slots80, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& slots160()
  {
    assert_choice_type(types::slots160, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint16_t& slots320()
  {
    assert_choice_type(types::slots320, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  uint16_t& slots640()
  {
    assert_choice_type(types::slots640, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint8_t& slots4() const
  {
    assert_choice_type(types::slots4, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots5() const
  {
    assert_choice_type(types::slots5, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots8() const
  {
    assert_choice_type(types::slots8, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots10() const
  {
    assert_choice_type(types::slots10, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots16() const
  {
    assert_choice_type(types::slots16, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots20() const
  {
    assert_choice_type(types::slots20, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots32() const
  {
    assert_choice_type(types::slots32, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots40() const
  {
    assert_choice_type(types::slots40, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots64() const
  {
    assert_choice_type(types::slots64, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots80() const
  {
    assert_choice_type(types::slots80, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& slots160() const
  {
    assert_choice_type(types::slots160, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint16_t& slots320() const
  {
    assert_choice_type(types::slots320, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint16_t& slots640() const
  {
    assert_choice_type(types::slots640, type_, "CSI-ResourcePeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  uint8_t&  set_slots4();
  uint8_t&  set_slots5();
  uint8_t&  set_slots8();
  uint8_t&  set_slots10();
  uint8_t&  set_slots16();
  uint8_t&  set_slots20();
  uint8_t&  set_slots32();
  uint8_t&  set_slots40();
  uint8_t&  set_slots64();
  uint8_t&  set_slots80();
  uint8_t&  set_slots160();
  uint16_t& set_slots320();
  uint16_t& set_slots640();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// INT-ConfigurationPerServingCell ::= SEQUENCE
struct int_cfg_per_serving_cell_s {
  uint8_t serving_cell_id = 0;
  uint8_t position_in_dci = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PTRS-DownlinkConfig ::= SEQUENCE
struct ptrs_dl_cfg_s {
  using freq_density_l_ = std::array<uint16_t, 2>;
  using time_density_l_ = std::array<uint8_t, 3>;
  struct res_elem_offset_opts {
    enum options { offset01, offset10, offset11, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<res_elem_offset_opts> res_elem_offset_e_;

  // member variables
  bool               ext                     = false;
  bool               freq_density_present    = false;
  bool               time_density_present    = false;
  bool               epre_ratio_present      = false;
  bool               res_elem_offset_present = false;
  freq_density_l_    freq_density;
  time_density_l_    time_density;
  uint8_t            epre_ratio = 0;
  res_elem_offset_e_ res_elem_offset;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QCL-Info ::= SEQUENCE
struct qcl_info_s {
  struct ref_sig_c_ {
    struct types_opts {
      enum options { csi_rs, ssb, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ref_sig_c_() = default;
    ref_sig_c_(const ref_sig_c_& other);
    ref_sig_c_& operator=(const ref_sig_c_& other);
    ~ref_sig_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& csi_rs()
    {
      assert_choice_type(types::csi_rs, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& ssb()
    {
      assert_choice_type(types::ssb, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs() const
    {
      assert_choice_type(types::csi_rs, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb() const
    {
      assert_choice_type(types::ssb, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& set_csi_rs();
    uint8_t& set_ssb();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct qcl_type_opts {
    enum options { type_a, type_b, type_c, type_d, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<qcl_type_opts> qcl_type_e_;

  // member variables
  bool        ext            = false;
  bool        cell_present   = false;
  bool        bwp_id_present = false;
  uint8_t     cell           = 0;
  uint8_t     bwp_id         = 0;
  ref_sig_c_  ref_sig;
  qcl_type_e_ qcl_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DMRS-DownlinkConfig ::= SEQUENCE
struct dmrs_dl_cfg_s {
  struct dmrs_add_position_opts {
    enum options { pos0, pos1, pos3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dmrs_add_position_opts> dmrs_add_position_e_;

  // member variables
  bool                           ext                       = false;
  bool                           dmrs_type_present         = false;
  bool                           dmrs_add_position_present = false;
  bool                           max_len_present           = false;
  bool                           scrambling_id0_present    = false;
  bool                           scrambling_id1_present    = false;
  bool                           phase_tracking_rs_present = false;
  dmrs_add_position_e_           dmrs_add_position;
  uint32_t                       scrambling_id0 = 0;
  uint32_t                       scrambling_id1 = 0;
  setup_release_c<ptrs_dl_cfg_s> phase_tracking_rs;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkPreemption ::= SEQUENCE
struct dl_preemption_s {
  struct time_freq_set_opts {
    enum options { set0, set1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<time_freq_set_opts> time_freq_set_e_;
  using int_cfg_per_serving_cell_l_ = dyn_array<int_cfg_per_serving_cell_s>;

  // member variables
  bool                        ext      = false;
  uint32_t                    int_rnti = 0;
  time_freq_set_e_            time_freq_set;
  uint8_t                     dci_payload_size = 0;
  int_cfg_per_serving_cell_l_ int_cfg_per_serving_cell;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-TPC-CommandConfig ::= SEQUENCE
struct pucch_tpc_cmd_cfg_s {
  bool    ext                         = false;
  bool    tpc_idx_pcell_present       = false;
  bool    tpc_idx_pucch_scell_present = false;
  uint8_t tpc_idx_pcell               = 1;
  uint8_t tpc_idx_pucch_scell         = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-TPC-CommandConfig ::= SEQUENCE
struct pusch_tpc_cmd_cfg_s {
  bool    ext                 = false;
  bool    tpc_idx_present     = false;
  bool    tpc_idx_sul_present = false;
  bool    target_cell_present = false;
  uint8_t tpc_idx             = 1;
  uint8_t tpc_idx_sul         = 1;
  uint8_t target_cell         = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioLinkMonitoringRS ::= SEQUENCE
struct radio_link_monitoring_rs_s {
  struct purpose_opts {
    enum options { beam_fail, rlf, both, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<purpose_opts> purpose_e_;
  struct detection_res_c_ {
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    detection_res_c_() = default;
    detection_res_c_(const detection_res_c_& other);
    detection_res_c_& operator=(const detection_res_c_& other);
    ~detection_res_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type(types::ssb_idx, type_, "detectionResource");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type(types::csi_rs_idx, type_, "detectionResource");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type(types::ssb_idx, type_, "detectionResource");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type(types::csi_rs_idx, type_, "detectionResource");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_idx();
    uint8_t& set_csi_rs_idx();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool             ext                         = false;
  uint8_t          radio_link_monitoring_rs_id = 0;
  purpose_e_       purpose;
  detection_res_c_ detection_res;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RateMatchPattern ::= SEQUENCE
struct rate_match_pattern_s {
  struct pattern_type_c_ {
    struct bitmaps_s_ {
      struct symbols_in_res_block_c_ {
        struct types_opts {
          enum options { one_slot, two_slots, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<types_opts> types;

        // choice methods
        symbols_in_res_block_c_() = default;
        symbols_in_res_block_c_(const symbols_in_res_block_c_& other);
        symbols_in_res_block_c_& operator=(const symbols_in_res_block_c_& other);
        ~symbols_in_res_block_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        fixed_bitstring<14>& one_slot()
        {
          assert_choice_type(types::one_slot, type_, "symbolsInResourceBlock");
          return c.get<fixed_bitstring<14> >();
        }
        fixed_bitstring<28>& two_slots()
        {
          assert_choice_type(types::two_slots, type_, "symbolsInResourceBlock");
          return c.get<fixed_bitstring<28> >();
        }
        const fixed_bitstring<14>& one_slot() const
        {
          assert_choice_type(types::one_slot, type_, "symbolsInResourceBlock");
          return c.get<fixed_bitstring<14> >();
        }
        const fixed_bitstring<28>& two_slots() const
        {
          assert_choice_type(types::two_slots, type_, "symbolsInResourceBlock");
          return c.get<fixed_bitstring<28> >();
        }
        fixed_bitstring<14>& set_one_slot();
        fixed_bitstring<28>& set_two_slots();

      private:
        types                                 type_;
        choice_buffer_t<fixed_bitstring<28> > c;

        void destroy_();
      };
      struct periodicity_and_pattern_c_ {
        struct types_opts {
          enum options { n2, n4, n5, n8, n10, n20, n40, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<types_opts> types;

        // choice methods
        periodicity_and_pattern_c_() = default;
        periodicity_and_pattern_c_(const periodicity_and_pattern_c_& other);
        periodicity_and_pattern_c_& operator=(const periodicity_and_pattern_c_& other);
        ~periodicity_and_pattern_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        fixed_bitstring<2>& n2()
        {
          assert_choice_type(types::n2, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<2> >();
        }
        fixed_bitstring<4>& n4()
        {
          assert_choice_type(types::n4, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<4> >();
        }
        fixed_bitstring<5>& n5()
        {
          assert_choice_type(types::n5, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<5> >();
        }
        fixed_bitstring<8>& n8()
        {
          assert_choice_type(types::n8, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<8> >();
        }
        fixed_bitstring<10>& n10()
        {
          assert_choice_type(types::n10, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<10> >();
        }
        fixed_bitstring<20>& n20()
        {
          assert_choice_type(types::n20, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<20> >();
        }
        fixed_bitstring<40>& n40()
        {
          assert_choice_type(types::n40, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<40> >();
        }
        const fixed_bitstring<2>& n2() const
        {
          assert_choice_type(types::n2, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<2> >();
        }
        const fixed_bitstring<4>& n4() const
        {
          assert_choice_type(types::n4, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<4> >();
        }
        const fixed_bitstring<5>& n5() const
        {
          assert_choice_type(types::n5, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<5> >();
        }
        const fixed_bitstring<8>& n8() const
        {
          assert_choice_type(types::n8, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<8> >();
        }
        const fixed_bitstring<10>& n10() const
        {
          assert_choice_type(types::n10, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<10> >();
        }
        const fixed_bitstring<20>& n20() const
        {
          assert_choice_type(types::n20, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<20> >();
        }
        const fixed_bitstring<40>& n40() const
        {
          assert_choice_type(types::n40, type_, "periodicityAndPattern");
          return c.get<fixed_bitstring<40> >();
        }
        fixed_bitstring<2>&  set_n2();
        fixed_bitstring<4>&  set_n4();
        fixed_bitstring<5>&  set_n5();
        fixed_bitstring<8>&  set_n8();
        fixed_bitstring<10>& set_n10();
        fixed_bitstring<20>& set_n20();
        fixed_bitstring<40>& set_n40();

      private:
        types                                 type_;
        choice_buffer_t<fixed_bitstring<40> > c;

        void destroy_();
      };

      // member variables
      bool                       ext                             = false;
      bool                       periodicity_and_pattern_present = false;
      fixed_bitstring<275>       res_blocks;
      symbols_in_res_block_c_    symbols_in_res_block;
      periodicity_and_pattern_c_ periodicity_and_pattern;
      // ...
    };
    struct types_opts {
      enum options { bitmaps, coreset, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    pattern_type_c_() = default;
    pattern_type_c_(const pattern_type_c_& other);
    pattern_type_c_& operator=(const pattern_type_c_& other);
    ~pattern_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bitmaps_s_& bitmaps()
    {
      assert_choice_type(types::bitmaps, type_, "patternType");
      return c.get<bitmaps_s_>();
    }
    uint8_t& coreset()
    {
      assert_choice_type(types::coreset, type_, "patternType");
      return c.get<uint8_t>();
    }
    const bitmaps_s_& bitmaps() const
    {
      assert_choice_type(types::bitmaps, type_, "patternType");
      return c.get<bitmaps_s_>();
    }
    const uint8_t& coreset() const
    {
      assert_choice_type(types::coreset, type_, "patternType");
      return c.get<uint8_t>();
    }
    bitmaps_s_& set_bitmaps();
    uint8_t&    set_coreset();

  private:
    types                       type_;
    choice_buffer_t<bitmaps_s_> c;

    void destroy_();
  };
  struct dummy_opts {
    enum options { dynamic_value, semi_static, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<dummy_opts> dummy_e_;

  // member variables
  bool                 ext                        = false;
  bool                 subcarrier_spacing_present = false;
  uint8_t              rate_match_pattern_id      = 0;
  pattern_type_c_      pattern_type;
  subcarrier_spacing_e subcarrier_spacing;
  dummy_e_             dummy;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct rate_match_pattern_group_item_c_ {
  struct types_opts {
    enum options { cell_level, bwp_level, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  rate_match_pattern_group_item_c_() = default;
  rate_match_pattern_group_item_c_(const rate_match_pattern_group_item_c_& other);
  rate_match_pattern_group_item_c_& operator=(const rate_match_pattern_group_item_c_& other);
  ~rate_match_pattern_group_item_c_() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& cell_level()
  {
    assert_choice_type(types::cell_level, type_, "RateMatchPatternGroup-item");
    return c.get<uint8_t>();
  }
  uint8_t& bwp_level()
  {
    assert_choice_type(types::bwp_level, type_, "RateMatchPatternGroup-item");
    return c.get<uint8_t>();
  }
  const uint8_t& cell_level() const
  {
    assert_choice_type(types::cell_level, type_, "RateMatchPatternGroup-item");
    return c.get<uint8_t>();
  }
  const uint8_t& bwp_level() const
  {
    assert_choice_type(types::bwp_level, type_, "RateMatchPatternGroup-item");
    return c.get<uint8_t>();
  }
  uint8_t& set_cell_level();
  uint8_t& set_bwp_level();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// RateMatchPatternGroup ::= SEQUENCE (SIZE (1..8)) OF RateMatchPatternGroup-item
using rate_match_pattern_group_l = dyn_array<rate_match_pattern_group_item_c_>;

// SRS-TPC-CommandConfig ::= SEQUENCE
struct srs_tpc_cmd_cfg_s {
  bool    ext                                 = false;
  bool    start_bit_of_format2_minus3_present = false;
  bool    field_type_format2_minus3_present   = false;
  uint8_t start_bit_of_format2_minus3         = 1;
  uint8_t field_type_format2_minus3           = 0;
  // ...
  // group 0
  bool    start_bit_of_format2_minus3_sul_present = false;
  uint8_t start_bit_of_format2_minus3_sul         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TCI-State ::= SEQUENCE
struct tci_state_s {
  bool       ext               = false;
  bool       qcl_type2_present = false;
  uint8_t    tci_state_id      = 0;
  qcl_info_s qcl_type1;
  qcl_info_s qcl_type2;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ZP-CSI-RS-Resource ::= SEQUENCE
struct zp_csi_rs_res_s {
  bool                             ext                            = false;
  bool                             periodicity_and_offset_present = false;
  uint8_t                          zp_csi_rs_res_id               = 0;
  csi_rs_res_map_s                 res_map;
  csi_res_periodicity_and_offset_c periodicity_and_offset;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ZP-CSI-RS-ResourceSet ::= SEQUENCE
struct zp_csi_rs_res_set_s {
  using zp_csi_rs_res_id_list_l_ = bounded_array<uint8_t, 16>;

  // member variables
  bool                     ext                  = false;
  uint8_t                  zp_csi_rs_res_set_id = 0;
  zp_csi_rs_res_id_list_l_ zp_csi_rs_res_id_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-Config ::= SEQUENCE
struct pdcch_cfg_s {
  using coreset_to_add_mod_list_l_       = dyn_array<coreset_s>;
  using coreset_to_release_list_l_       = bounded_array<uint8_t, 3>;
  using search_spaces_to_add_mod_list_l_ = dyn_array<search_space_s>;
  using search_spaces_to_release_list_l_ = bounded_array<uint8_t, 10>;

  // member variables
  bool                                 ext                   = false;
  bool                                 dl_preemption_present = false;
  bool                                 tpc_pusch_present     = false;
  bool                                 tpc_pucch_present     = false;
  bool                                 tpc_srs_present       = false;
  coreset_to_add_mod_list_l_           coreset_to_add_mod_list;
  coreset_to_release_list_l_           coreset_to_release_list;
  search_spaces_to_add_mod_list_l_     search_spaces_to_add_mod_list;
  search_spaces_to_release_list_l_     search_spaces_to_release_list;
  setup_release_c<dl_preemption_s>     dl_preemption;
  setup_release_c<pusch_tpc_cmd_cfg_s> tpc_pusch;
  setup_release_c<pucch_tpc_cmd_cfg_s> tpc_pucch;
  setup_release_c<srs_tpc_cmd_cfg_s>   tpc_srs;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-Config ::= SEQUENCE
struct pdsch_cfg_s {
  using tci_states_to_add_mod_list_l_ = dyn_array<tci_state_s>;
  using tci_states_to_release_list_l_ = dyn_array<uint8_t>;
  struct vrb_to_prb_interleaver_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<vrb_to_prb_interleaver_opts> vrb_to_prb_interleaver_e_;
  struct res_alloc_opts {
    enum options { res_alloc_type0, res_alloc_type1, dynamic_switch, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<res_alloc_opts> res_alloc_e_;
  struct pdsch_aggregation_factor_opts {
    enum options { n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pdsch_aggregation_factor_opts> pdsch_aggregation_factor_e_;
  using rate_match_pattern_to_add_mod_list_l_ = dyn_array<rate_match_pattern_s>;
  using rate_match_pattern_to_release_list_l_ = bounded_array<uint8_t, 4>;
  struct rbg_size_opts {
    enum options { cfg1, cfg2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<rbg_size_opts> rbg_size_e_;
  struct mcs_table_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mcs_table_opts> mcs_table_e_;
  struct max_nrof_code_words_sched_by_dci_opts {
    enum options { n1, n2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_nrof_code_words_sched_by_dci_opts> max_nrof_code_words_sched_by_dci_e_;
  struct prb_bundling_type_c_ {
    struct static_bundling_s_ {
      struct bundle_size_opts {
        enum options { n4, wideband, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<bundle_size_opts> bundle_size_e_;

      // member variables
      bool           bundle_size_present = false;
      bundle_size_e_ bundle_size;
    };
    struct dynamic_bundling_s_ {
      struct bundle_size_set1_opts {
        enum options { n4, wideband, n2_wideband, n4_wideband, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<bundle_size_set1_opts> bundle_size_set1_e_;
      struct bundle_size_set2_opts {
        enum options { n4, wideband, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<bundle_size_set2_opts> bundle_size_set2_e_;

      // member variables
      bool                bundle_size_set1_present = false;
      bool                bundle_size_set2_present = false;
      bundle_size_set1_e_ bundle_size_set1;
      bundle_size_set2_e_ bundle_size_set2;
    };
    struct types_opts {
      enum options { static_bundling, dynamic_bundling, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    prb_bundling_type_c_() = default;
    prb_bundling_type_c_(const prb_bundling_type_c_& other);
    prb_bundling_type_c_& operator=(const prb_bundling_type_c_& other);
    ~prb_bundling_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    static_bundling_s_& static_bundling()
    {
      assert_choice_type(types::static_bundling, type_, "prb-BundlingType");
      return c.get<static_bundling_s_>();
    }
    dynamic_bundling_s_& dynamic_bundling()
    {
      assert_choice_type(types::dynamic_bundling, type_, "prb-BundlingType");
      return c.get<dynamic_bundling_s_>();
    }
    const static_bundling_s_& static_bundling() const
    {
      assert_choice_type(types::static_bundling, type_, "prb-BundlingType");
      return c.get<static_bundling_s_>();
    }
    const dynamic_bundling_s_& dynamic_bundling() const
    {
      assert_choice_type(types::dynamic_bundling, type_, "prb-BundlingType");
      return c.get<dynamic_bundling_s_>();
    }
    static_bundling_s_&  set_static_bundling();
    dynamic_bundling_s_& set_dynamic_bundling();

  private:
    types                                                    type_;
    choice_buffer_t<dynamic_bundling_s_, static_bundling_s_> c;

    void destroy_();
  };
  using zp_csi_rs_res_to_add_mod_list_l_                = dyn_array<zp_csi_rs_res_s>;
  using zp_csi_rs_res_to_release_list_l_                = bounded_array<uint8_t, 32>;
  using aperiodic_zp_csi_rs_res_sets_to_add_mod_list_l_ = dyn_array<zp_csi_rs_res_set_s>;
  using aperiodic_zp_csi_rs_res_sets_to_release_list_l_ = bounded_array<uint8_t, 16>;
  using sp_zp_csi_rs_res_sets_to_add_mod_list_l_        = dyn_array<zp_csi_rs_res_set_s>;
  using sp_zp_csi_rs_res_sets_to_release_list_l_        = bounded_array<uint8_t, 16>;

  // member variables
  bool                                                               ext                                      = false;
  bool                                                               data_scrambling_id_pdsch_present         = false;
  bool                                                               dmrs_dl_for_pdsch_map_type_a_present     = false;
  bool                                                               dmrs_dl_for_pdsch_map_type_b_present     = false;
  bool                                                               vrb_to_prb_interleaver_present           = false;
  bool                                                               pdsch_time_domain_alloc_list_present     = false;
  bool                                                               pdsch_aggregation_factor_present         = false;
  bool                                                               mcs_table_present                        = false;
  bool                                                               max_nrof_code_words_sched_by_dci_present = false;
  bool                                                               p_zp_csi_rs_res_set_present              = false;
  uint16_t                                                           data_scrambling_id_pdsch                 = 0;
  setup_release_c<dmrs_dl_cfg_s>                                     dmrs_dl_for_pdsch_map_type_a;
  setup_release_c<dmrs_dl_cfg_s>                                     dmrs_dl_for_pdsch_map_type_b;
  tci_states_to_add_mod_list_l_                                      tci_states_to_add_mod_list;
  tci_states_to_release_list_l_                                      tci_states_to_release_list;
  vrb_to_prb_interleaver_e_                                          vrb_to_prb_interleaver;
  res_alloc_e_                                                       res_alloc;
  setup_release_c<dyn_seq_of<pdsch_time_domain_res_alloc_s, 1, 16> > pdsch_time_domain_alloc_list;
  pdsch_aggregation_factor_e_                                        pdsch_aggregation_factor;
  rate_match_pattern_to_add_mod_list_l_                              rate_match_pattern_to_add_mod_list;
  rate_match_pattern_to_release_list_l_                              rate_match_pattern_to_release_list;
  rate_match_pattern_group_l                                         rate_match_pattern_group1;
  rate_match_pattern_group_l                                         rate_match_pattern_group2;
  rbg_size_e_                                                        rbg_size;
  mcs_table_e_                                                       mcs_table;
  max_nrof_code_words_sched_by_dci_e_                                max_nrof_code_words_sched_by_dci;
  prb_bundling_type_c_                                               prb_bundling_type;
  zp_csi_rs_res_to_add_mod_list_l_                                   zp_csi_rs_res_to_add_mod_list;
  zp_csi_rs_res_to_release_list_l_                                   zp_csi_rs_res_to_release_list;
  aperiodic_zp_csi_rs_res_sets_to_add_mod_list_l_                    aperiodic_zp_csi_rs_res_sets_to_add_mod_list;
  aperiodic_zp_csi_rs_res_sets_to_release_list_l_                    aperiodic_zp_csi_rs_res_sets_to_release_list;
  sp_zp_csi_rs_res_sets_to_add_mod_list_l_                           sp_zp_csi_rs_res_sets_to_add_mod_list;
  sp_zp_csi_rs_res_sets_to_release_list_l_                           sp_zp_csi_rs_res_sets_to_release_list;
  setup_release_c<zp_csi_rs_res_set_s>                               p_zp_csi_rs_res_set;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioLinkMonitoringConfig ::= SEQUENCE
struct radio_link_monitoring_cfg_s {
  using fail_detection_res_to_add_mod_list_l_ = dyn_array<radio_link_monitoring_rs_s>;
  using fail_detection_res_to_release_list_l_ = bounded_array<uint8_t, 10>;
  struct beam_fail_instance_max_count_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<beam_fail_instance_max_count_opts> beam_fail_instance_max_count_e_;
  struct beam_fail_detection_timer_opts {
    enum options { pbfd1, pbfd2, pbfd3, pbfd4, pbfd5, pbfd6, pbfd8, pbfd10, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<beam_fail_detection_timer_opts> beam_fail_detection_timer_e_;

  // member variables
  bool                                  ext                                  = false;
  bool                                  beam_fail_instance_max_count_present = false;
  bool                                  beam_fail_detection_timer_present    = false;
  fail_detection_res_to_add_mod_list_l_ fail_detection_res_to_add_mod_list;
  fail_detection_res_to_release_list_l_ fail_detection_res_to_release_list;
  beam_fail_instance_max_count_e_       beam_fail_instance_max_count;
  beam_fail_detection_timer_e_          beam_fail_detection_timer;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SPS-Config ::= SEQUENCE
struct sps_cfg_s {
  struct periodicity_opts {
    enum options {
      ms10,
      ms20,
      ms32,
      ms40,
      ms64,
      ms80,
      ms128,
      ms160,
      ms320,
      ms640,
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
  typedef enumerated<periodicity_opts> periodicity_e_;

  // member variables
  bool           ext                 = false;
  bool           n1_pucch_an_present = false;
  bool           mcs_table_present   = false;
  periodicity_e_ periodicity;
  uint8_t        nrof_harq_processes = 1;
  uint8_t        n1_pucch_an         = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-DownlinkDedicated ::= SEQUENCE
struct bwp_dl_ded_s {
  bool                                         ext                               = false;
  bool                                         pdcch_cfg_present                 = false;
  bool                                         pdsch_cfg_present                 = false;
  bool                                         sps_cfg_present                   = false;
  bool                                         radio_link_monitoring_cfg_present = false;
  setup_release_c<pdcch_cfg_s>                 pdcch_cfg;
  setup_release_c<pdsch_cfg_s>                 pdsch_cfg;
  setup_release_c<sps_cfg_s>                   sps_cfg;
  setup_release_c<radio_link_monitoring_cfg_s> radio_link_monitoring_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-Downlink ::= SEQUENCE
struct bwp_dl_s {
  bool            ext                = false;
  bool            bwp_common_present = false;
  bool            bwp_ded_present    = false;
  uint8_t         bwp_id             = 0;
  bwp_dl_common_s bwp_common;
  bwp_dl_ded_s    bwp_ded;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Alpha ::= ENUMERATED
struct alpha_opts {
  enum options { alpha0, alpha04, alpha05, alpha06, alpha07, alpha08, alpha09, alpha1, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
typedef enumerated<alpha_opts> alpha_e;

// BetaOffsets ::= SEQUENCE
struct beta_offsets_s {
  bool    beta_offset_ack_idx1_present       = false;
  bool    beta_offset_ack_idx2_present       = false;
  bool    beta_offset_ack_idx3_present       = false;
  bool    beta_offset_csi_part1_idx1_present = false;
  bool    beta_offset_csi_part1_idx2_present = false;
  bool    beta_offset_csi_part2_idx1_present = false;
  bool    beta_offset_csi_part2_idx2_present = false;
  uint8_t beta_offset_ack_idx1               = 0;
  uint8_t beta_offset_ack_idx2               = 0;
  uint8_t beta_offset_ack_idx3               = 0;
  uint8_t beta_offset_csi_part1_idx1         = 0;
  uint8_t beta_offset_csi_part1_idx2         = 0;
  uint8_t beta_offset_csi_part2_idx1         = 0;
  uint8_t beta_offset_csi_part2_idx2         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// P0-PUCCH ::= SEQUENCE
struct p0_pucch_s {
  uint8_t p0_pucch_id    = 1;
  int8_t  p0_pucch_value = -16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// P0-PUSCH-AlphaSet ::= SEQUENCE
struct p0_pusch_alpha_set_s {
  bool    p0_present            = false;
  bool    alpha_present         = false;
  uint8_t p0_pusch_alpha_set_id = 0;
  int8_t  p0                    = -16;
  alpha_e alpha;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PTRS-UplinkConfig ::= SEQUENCE
struct ptrs_ul_cfg_s {
  struct transform_precoder_disabled_s_ {
    using freq_density_l_ = std::array<uint16_t, 2>;
    using time_density_l_ = std::array<uint8_t, 3>;
    struct max_nrof_ports_opts {
      enum options { n1, n2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<max_nrof_ports_opts> max_nrof_ports_e_;
    struct res_elem_offset_opts {
      enum options { offset01, offset10, offset11, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<res_elem_offset_opts> res_elem_offset_e_;
    struct ptrs_pwr_opts {
      enum options { p00, p01, p10, p11, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<ptrs_pwr_opts> ptrs_pwr_e_;

    // member variables
    bool               freq_density_present    = false;
    bool               time_density_present    = false;
    bool               res_elem_offset_present = false;
    freq_density_l_    freq_density;
    time_density_l_    time_density;
    max_nrof_ports_e_  max_nrof_ports;
    res_elem_offset_e_ res_elem_offset;
    ptrs_pwr_e_        ptrs_pwr;
  };
  struct transform_precoder_enabled_s_ {
    using sample_density_l_ = std::array<uint16_t, 5>;

    // member variables
    bool              time_density_transform_precoding_present = false;
    sample_density_l_ sample_density;
  };

  // member variables
  bool                           ext                                 = false;
  bool                           transform_precoder_disabled_present = false;
  bool                           transform_precoder_enabled_present  = false;
  transform_precoder_disabled_s_ transform_precoder_disabled;
  transform_precoder_enabled_s_  transform_precoder_enabled;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-MaxCodeRate ::= ENUMERATED
struct pucch_max_code_rate_opts {
  enum options { zero_dot08, zero_dot15, zero_dot25, zero_dot35, zero_dot45, zero_dot60, zero_dot80, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
typedef enumerated<pucch_max_code_rate_opts> pucch_max_code_rate_e;

// PUCCH-PathlossReferenceRS ::= SEQUENCE
struct pucch_pathloss_ref_rs_s {
  struct ref_sig_c_ {
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ref_sig_c_() = default;
    ref_sig_c_(const ref_sig_c_& other);
    ref_sig_c_& operator=(const ref_sig_c_& other);
    ~ref_sig_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_idx();
    uint8_t& set_csi_rs_idx();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  uint8_t    pucch_pathloss_ref_rs_id = 0;
  ref_sig_c_ ref_sig;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-format0 ::= SEQUENCE
struct pucch_format0_s {
  uint8_t init_cyclic_shift = 0;
  uint8_t nrof_symbols      = 1;
  uint8_t start_symbol_idx  = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-format1 ::= SEQUENCE
struct pucch_format1_s {
  uint8_t init_cyclic_shift = 0;
  uint8_t nrof_symbols      = 4;
  uint8_t start_symbol_idx  = 0;
  uint8_t time_domain_occ   = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-format2 ::= SEQUENCE
struct pucch_format2_s {
  uint8_t nrof_prbs        = 1;
  uint8_t nrof_symbols     = 1;
  uint8_t start_symbol_idx = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-format3 ::= SEQUENCE
struct pucch_format3_s {
  uint8_t nrof_prbs        = 1;
  uint8_t nrof_symbols     = 4;
  uint8_t start_symbol_idx = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-format4 ::= SEQUENCE
struct pucch_format4_s {
  struct occ_len_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<occ_len_opts> occ_len_e_;
  struct occ_idx_opts {
    enum options { n0, n1, n2, n3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<occ_idx_opts> occ_idx_e_;

  // member variables
  uint8_t    nrof_symbols = 4;
  occ_len_e_ occ_len;
  occ_idx_e_ occ_idx;
  uint8_t    start_symbol_idx = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-PathlossReferenceRS ::= SEQUENCE
struct pusch_pathloss_ref_rs_s {
  struct ref_sig_c_ {
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ref_sig_c_() = default;
    ref_sig_c_(const ref_sig_c_& other);
    ref_sig_c_& operator=(const ref_sig_c_& other);
    ~ref_sig_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_idx();
    uint8_t& set_csi_rs_idx();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  uint8_t    pusch_pathloss_ref_rs_id = 0;
  ref_sig_c_ ref_sig;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRI-PUSCH-PowerControl ::= SEQUENCE
struct sri_pusch_pwr_ctrl_s {
  struct sri_pusch_closed_loop_idx_opts {
    enum options { i0, i1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sri_pusch_closed_loop_idx_opts> sri_pusch_closed_loop_idx_e_;

  // member variables
  uint8_t                      sri_pusch_pwr_ctrl_id        = 0;
  uint8_t                      sri_pusch_pathloss_ref_rs_id = 0;
  uint8_t                      sri_p0_pusch_alpha_set_id    = 0;
  sri_pusch_closed_loop_idx_e_ sri_pusch_closed_loop_idx;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CG-UCI-OnPUSCH ::= CHOICE
struct cg_uci_on_pusch_c {
  using dynamic_l_ = dyn_array<beta_offsets_s>;
  struct types_opts {
    enum options { dynamic_type, semi_static, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  cg_uci_on_pusch_c() = default;
  cg_uci_on_pusch_c(const cg_uci_on_pusch_c& other);
  cg_uci_on_pusch_c& operator=(const cg_uci_on_pusch_c& other);
  ~cg_uci_on_pusch_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  dynamic_l_& dynamic_type()
  {
    assert_choice_type(types::dynamic_type, type_, "CG-UCI-OnPUSCH");
    return c.get<dynamic_l_>();
  }
  beta_offsets_s& semi_static()
  {
    assert_choice_type(types::semi_static, type_, "CG-UCI-OnPUSCH");
    return c.get<beta_offsets_s>();
  }
  const dynamic_l_& dynamic_type() const
  {
    assert_choice_type(types::dynamic_type, type_, "CG-UCI-OnPUSCH");
    return c.get<dynamic_l_>();
  }
  const beta_offsets_s& semi_static() const
  {
    assert_choice_type(types::semi_static, type_, "CG-UCI-OnPUSCH");
    return c.get<beta_offsets_s>();
  }
  dynamic_l_&     set_dynamic_type();
  beta_offsets_s& set_semi_static();

private:
  types                                       type_;
  choice_buffer_t<beta_offsets_s, dynamic_l_> c;

  void destroy_();
};

// DMRS-UplinkConfig ::= SEQUENCE
struct dmrs_ul_cfg_s {
  struct dmrs_add_position_opts {
    enum options { pos0, pos1, pos3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dmrs_add_position_opts> dmrs_add_position_e_;
  struct transform_precoding_disabled_s_ {
    bool     ext                    = false;
    bool     scrambling_id0_present = false;
    bool     scrambling_id1_present = false;
    uint32_t scrambling_id0         = 0;
    uint32_t scrambling_id1         = 0;
    // ...
  };
  struct transform_precoding_enabled_s_ {
    bool     ext                   = false;
    bool     npusch_id_present     = false;
    bool     seq_group_hop_present = false;
    bool     seq_hop_present       = false;
    uint16_t npusch_id             = 0;
    // ...
  };

  // member variables
  bool                            ext                                  = false;
  bool                            dmrs_type_present                    = false;
  bool                            dmrs_add_position_present            = false;
  bool                            phase_tracking_rs_present            = false;
  bool                            max_len_present                      = false;
  bool                            transform_precoding_disabled_present = false;
  bool                            transform_precoding_enabled_present  = false;
  dmrs_add_position_e_            dmrs_add_position;
  setup_release_c<ptrs_ul_cfg_s>  phase_tracking_rs;
  transform_precoding_disabled_s_ transform_precoding_disabled;
  transform_precoding_enabled_s_  transform_precoding_enabled;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRACH-ResourceDedicatedBFR ::= CHOICE
struct prach_res_ded_bfr_c {
  struct types_opts {
    enum options { ssb, csi_rs, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  prach_res_ded_bfr_c() = default;
  prach_res_ded_bfr_c(const prach_res_ded_bfr_c& other);
  prach_res_ded_bfr_c& operator=(const prach_res_ded_bfr_c& other);
  ~prach_res_ded_bfr_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  bfr_ssb_res_s& ssb()
  {
    assert_choice_type(types::ssb, type_, "PRACH-ResourceDedicatedBFR");
    return c.get<bfr_ssb_res_s>();
  }
  bfr_csirs_res_s& csi_rs()
  {
    assert_choice_type(types::csi_rs, type_, "PRACH-ResourceDedicatedBFR");
    return c.get<bfr_csirs_res_s>();
  }
  const bfr_ssb_res_s& ssb() const
  {
    assert_choice_type(types::ssb, type_, "PRACH-ResourceDedicatedBFR");
    return c.get<bfr_ssb_res_s>();
  }
  const bfr_csirs_res_s& csi_rs() const
  {
    assert_choice_type(types::csi_rs, type_, "PRACH-ResourceDedicatedBFR");
    return c.get<bfr_csirs_res_s>();
  }
  bfr_ssb_res_s&   set_ssb();
  bfr_csirs_res_s& set_csi_rs();

private:
  types                                           type_;
  choice_buffer_t<bfr_csirs_res_s, bfr_ssb_res_s> c;

  void destroy_();
};

// PUCCH-FormatConfig ::= SEQUENCE
struct pucch_format_cfg_s {
  struct nrof_slots_opts {
    enum options { n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nrof_slots_opts> nrof_slots_e_;

  // member variables
  bool                  interslot_freq_hop_present = false;
  bool                  add_dmrs_present           = false;
  bool                  max_code_rate_present      = false;
  bool                  nrof_slots_present         = false;
  bool                  pi2_bpsk_present           = false;
  bool                  simul_harq_ack_csi_present = false;
  pucch_max_code_rate_e max_code_rate;
  nrof_slots_e_         nrof_slots;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-PowerControl ::= SEQUENCE
struct pucch_pwr_ctrl_s {
  using p0_set_l_           = dyn_array<p0_pucch_s>;
  using pathloss_ref_rss_l_ = dyn_array<pucch_pathloss_ref_rs_s>;

  // member variables
  bool                ext                                    = false;
  bool                delta_f_pucch_f0_present               = false;
  bool                delta_f_pucch_f1_present               = false;
  bool                delta_f_pucch_f2_present               = false;
  bool                delta_f_pucch_f3_present               = false;
  bool                delta_f_pucch_f4_present               = false;
  bool                two_pucch_pc_adjustment_states_present = false;
  int8_t              delta_f_pucch_f0                       = -16;
  int8_t              delta_f_pucch_f1                       = -16;
  int8_t              delta_f_pucch_f2                       = -16;
  int8_t              delta_f_pucch_f3                       = -16;
  int8_t              delta_f_pucch_f4                       = -16;
  p0_set_l_           p0_set;
  pathloss_ref_rss_l_ pathloss_ref_rss;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-Resource ::= SEQUENCE
struct pucch_res_s {
  struct format_c_ {
    struct types_opts {
      enum options { format0, format1, format2, format3, format4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    format_c_() = default;
    format_c_(const format_c_& other);
    format_c_& operator=(const format_c_& other);
    ~format_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pucch_format0_s& format0()
    {
      assert_choice_type(types::format0, type_, "format");
      return c.get<pucch_format0_s>();
    }
    pucch_format1_s& format1()
    {
      assert_choice_type(types::format1, type_, "format");
      return c.get<pucch_format1_s>();
    }
    pucch_format2_s& format2()
    {
      assert_choice_type(types::format2, type_, "format");
      return c.get<pucch_format2_s>();
    }
    pucch_format3_s& format3()
    {
      assert_choice_type(types::format3, type_, "format");
      return c.get<pucch_format3_s>();
    }
    pucch_format4_s& format4()
    {
      assert_choice_type(types::format4, type_, "format");
      return c.get<pucch_format4_s>();
    }
    const pucch_format0_s& format0() const
    {
      assert_choice_type(types::format0, type_, "format");
      return c.get<pucch_format0_s>();
    }
    const pucch_format1_s& format1() const
    {
      assert_choice_type(types::format1, type_, "format");
      return c.get<pucch_format1_s>();
    }
    const pucch_format2_s& format2() const
    {
      assert_choice_type(types::format2, type_, "format");
      return c.get<pucch_format2_s>();
    }
    const pucch_format3_s& format3() const
    {
      assert_choice_type(types::format3, type_, "format");
      return c.get<pucch_format3_s>();
    }
    const pucch_format4_s& format4() const
    {
      assert_choice_type(types::format4, type_, "format");
      return c.get<pucch_format4_s>();
    }
    pucch_format0_s& set_format0();
    pucch_format1_s& set_format1();
    pucch_format2_s& set_format2();
    pucch_format3_s& set_format3();
    pucch_format4_s& set_format4();

  private:
    types                                                                                                type_;
    choice_buffer_t<pucch_format0_s, pucch_format1_s, pucch_format2_s, pucch_format3_s, pucch_format4_s> c;

    void destroy_();
  };

  // member variables
  bool      intra_slot_freq_hop_present = false;
  bool      second_hop_prb_present      = false;
  uint8_t   pucch_res_id                = 0;
  uint16_t  start_prb                   = 0;
  uint16_t  second_hop_prb              = 0;
  format_c_ format;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-ResourceSet ::= SEQUENCE
struct pucch_res_set_s {
  using res_list_l_ = bounded_array<uint8_t, 32>;

  // member variables
  bool        max_payload_size_present = false;
  uint8_t     pucch_res_set_id         = 0;
  res_list_l_ res_list;
  uint16_t    max_payload_size = 4;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-SpatialRelationInfo ::= SEQUENCE
struct pucch_spatial_relation_info_s {
  struct ref_sig_c_ {
    struct srs_s_ {
      uint8_t res    = 0;
      uint8_t ul_bwp = 0;
    };
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, srs, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ref_sig_c_() = default;
    ref_sig_c_(const ref_sig_c_& other);
    ref_sig_c_& operator=(const ref_sig_c_& other);
    ~ref_sig_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    srs_s_& srs()
    {
      assert_choice_type(types::srs, type_, "referenceSignal");
      return c.get<srs_s_>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const srs_s_& srs() const
    {
      assert_choice_type(types::srs, type_, "referenceSignal");
      return c.get<srs_s_>();
    }
    uint8_t& set_ssb_idx();
    uint8_t& set_csi_rs_idx();
    srs_s_&  set_srs();

  private:
    types                   type_;
    choice_buffer_t<srs_s_> c;

    void destroy_();
  };
  struct closed_loop_idx_opts {
    enum options { i0, i1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<closed_loop_idx_opts> closed_loop_idx_e_;

  // member variables
  bool               serving_cell_id_present        = false;
  uint8_t            pucch_spatial_relation_info_id = 1;
  uint8_t            serving_cell_id                = 0;
  ref_sig_c_         ref_sig;
  uint8_t            pucch_pathloss_ref_rs_id = 0;
  uint8_t            p0_pucch_id              = 1;
  closed_loop_idx_e_ closed_loop_idx;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-PowerControl ::= SEQUENCE
struct pusch_pwr_ctrl_s {
  using p0_alpha_sets_l_                   = dyn_array<p0_pusch_alpha_set_s>;
  using pathloss_ref_rs_to_add_mod_list_l_ = dyn_array<pusch_pathloss_ref_rs_s>;
  using pathloss_ref_rs_to_release_list_l_ = bounded_array<uint8_t, 4>;
  using sri_pusch_map_to_add_mod_list_l_   = dyn_array<sri_pusch_pwr_ctrl_s>;
  using sri_pusch_map_to_release_list_l_   = bounded_array<uint8_t, 16>;

  // member variables
  bool                               tpc_accumulation_present               = false;
  bool                               msg3_alpha_present                     = false;
  bool                               p0_nominal_without_grant_present       = false;
  bool                               two_pusch_pc_adjustment_states_present = false;
  bool                               delta_mcs_present                      = false;
  alpha_e                            msg3_alpha;
  int16_t                            p0_nominal_without_grant = -202;
  p0_alpha_sets_l_                   p0_alpha_sets;
  pathloss_ref_rs_to_add_mod_list_l_ pathloss_ref_rs_to_add_mod_list;
  pathloss_ref_rs_to_release_list_l_ pathloss_ref_rs_to_release_list;
  sri_pusch_map_to_add_mod_list_l_   sri_pusch_map_to_add_mod_list;
  sri_pusch_map_to_release_list_l_   sri_pusch_map_to_release_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RA-Prioritization ::= SEQUENCE
struct ra_prioritization_s {
  struct pwr_ramp_step_high_prio_opts {
    enum options { db0, db2, db4, db6, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pwr_ramp_step_high_prio_opts> pwr_ramp_step_high_prio_e_;
  struct scaling_factor_bi_opts {
    enum options { zero, dot25, dot5, dot75, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<scaling_factor_bi_opts> scaling_factor_bi_e_;

  // member variables
  bool                       ext                       = false;
  bool                       scaling_factor_bi_present = false;
  pwr_ramp_step_high_prio_e_ pwr_ramp_step_high_prio;
  scaling_factor_bi_e_       scaling_factor_bi;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-ResourceSet ::= SEQUENCE
struct srs_res_set_s {
  using srs_res_id_list_l_ = bounded_array<uint8_t, 16>;
  struct res_type_c_ {
    struct aperiodic_s_ {
      using aperiodic_srs_res_trigger_list_l_ = bounded_array<uint8_t, 2>;

      // member variables
      bool    ext                       = false;
      bool    csi_rs_present            = false;
      bool    slot_offset_present       = false;
      uint8_t aperiodic_srs_res_trigger = 1;
      uint8_t csi_rs                    = 0;
      uint8_t slot_offset               = 1;
      // ...
      // group 0
      copy_ptr<aperiodic_srs_res_trigger_list_l_> aperiodic_srs_res_trigger_list;

      // sequence methods
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
    };
    struct semi_persistent_s_ {
      bool    ext                       = false;
      bool    associated_csi_rs_present = false;
      uint8_t associated_csi_rs         = 0;
      // ...
    };
    struct periodic_s_ {
      bool    ext                       = false;
      bool    associated_csi_rs_present = false;
      uint8_t associated_csi_rs         = 0;
      // ...
    };
    struct types_opts {
      enum options { aperiodic, semi_persistent, periodic, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    res_type_c_() = default;
    res_type_c_(const res_type_c_& other);
    res_type_c_& operator=(const res_type_c_& other);
    ~res_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    aperiodic_s_& aperiodic()
    {
      assert_choice_type(types::aperiodic, type_, "resourceType");
      return c.get<aperiodic_s_>();
    }
    semi_persistent_s_& semi_persistent()
    {
      assert_choice_type(types::semi_persistent, type_, "resourceType");
      return c.get<semi_persistent_s_>();
    }
    periodic_s_& periodic()
    {
      assert_choice_type(types::periodic, type_, "resourceType");
      return c.get<periodic_s_>();
    }
    const aperiodic_s_& aperiodic() const
    {
      assert_choice_type(types::aperiodic, type_, "resourceType");
      return c.get<aperiodic_s_>();
    }
    const semi_persistent_s_& semi_persistent() const
    {
      assert_choice_type(types::semi_persistent, type_, "resourceType");
      return c.get<semi_persistent_s_>();
    }
    const periodic_s_& periodic() const
    {
      assert_choice_type(types::periodic, type_, "resourceType");
      return c.get<periodic_s_>();
    }
    aperiodic_s_&       set_aperiodic();
    semi_persistent_s_& set_semi_persistent();
    periodic_s_&        set_periodic();

  private:
    types                                                          type_;
    choice_buffer_t<aperiodic_s_, periodic_s_, semi_persistent_s_> c;

    void destroy_();
  };
  struct usage_opts {
    enum options { beam_management, codebook, non_codebook, ant_switching, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<usage_opts> usage_e_;
  struct pathloss_ref_rs_c_ {
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    pathloss_ref_rs_c_() = default;
    pathloss_ref_rs_c_(const pathloss_ref_rs_c_& other);
    pathloss_ref_rs_c_& operator=(const pathloss_ref_rs_c_& other);
    ~pathloss_ref_rs_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type(types::ssb_idx, type_, "pathlossReferenceRS");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type(types::csi_rs_idx, type_, "pathlossReferenceRS");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type(types::ssb_idx, type_, "pathlossReferenceRS");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type(types::csi_rs_idx, type_, "pathlossReferenceRS");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_idx();
    uint8_t& set_csi_rs_idx();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct srs_pwr_ctrl_adjustment_states_opts {
    enum options { same_as_fci2, separate_closed_loop, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<srs_pwr_ctrl_adjustment_states_opts> srs_pwr_ctrl_adjustment_states_e_;

  // member variables
  bool                              ext                                    = false;
  bool                              alpha_present                          = false;
  bool                              p0_present                             = false;
  bool                              pathloss_ref_rs_present                = false;
  bool                              srs_pwr_ctrl_adjustment_states_present = false;
  uint8_t                           srs_res_set_id                         = 0;
  srs_res_id_list_l_                srs_res_id_list;
  res_type_c_                       res_type;
  usage_e_                          usage;
  alpha_e                           alpha;
  int16_t                           p0 = -202;
  pathloss_ref_rs_c_                pathloss_ref_rs;
  srs_pwr_ctrl_adjustment_states_e_ srs_pwr_ctrl_adjustment_states;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingRequestResourceConfig ::= SEQUENCE
struct sched_request_res_cfg_s {
  struct periodicity_and_offset_c_ {
    struct types_opts {
      enum options {
        sym2,
        sym6or7,
        sl1,
        sl2,
        sl4,
        sl5,
        sl8,
        sl10,
        sl16,
        sl20,
        sl40,
        sl80,
        sl160,
        sl320,
        sl640,
        nulltype
      } value;

      const char* to_string() const;
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
    uint8_t& sl2()
    {
      assert_choice_type(types::sl2, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl4()
    {
      assert_choice_type(types::sl4, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl5()
    {
      assert_choice_type(types::sl5, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl8()
    {
      assert_choice_type(types::sl8, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl10()
    {
      assert_choice_type(types::sl10, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl16()
    {
      assert_choice_type(types::sl16, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl20()
    {
      assert_choice_type(types::sl20, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl40()
    {
      assert_choice_type(types::sl40, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl80()
    {
      assert_choice_type(types::sl80, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint8_t& sl160()
    {
      assert_choice_type(types::sl160, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    uint16_t& sl320()
    {
      assert_choice_type(types::sl320, type_, "periodicityAndOffset");
      return c.get<uint16_t>();
    }
    uint16_t& sl640()
    {
      assert_choice_type(types::sl640, type_, "periodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint8_t& sl2() const
    {
      assert_choice_type(types::sl2, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl4() const
    {
      assert_choice_type(types::sl4, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl5() const
    {
      assert_choice_type(types::sl5, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl8() const
    {
      assert_choice_type(types::sl8, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl10() const
    {
      assert_choice_type(types::sl10, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl16() const
    {
      assert_choice_type(types::sl16, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl20() const
    {
      assert_choice_type(types::sl20, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl40() const
    {
      assert_choice_type(types::sl40, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl80() const
    {
      assert_choice_type(types::sl80, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& sl160() const
    {
      assert_choice_type(types::sl160, type_, "periodicityAndOffset");
      return c.get<uint8_t>();
    }
    const uint16_t& sl320() const
    {
      assert_choice_type(types::sl320, type_, "periodicityAndOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& sl640() const
    {
      assert_choice_type(types::sl640, type_, "periodicityAndOffset");
      return c.get<uint16_t>();
    }
    void      set_sym2();
    void      set_sym6or7();
    void      set_sl1();
    uint8_t&  set_sl2();
    uint8_t&  set_sl4();
    uint8_t&  set_sl5();
    uint8_t&  set_sl8();
    uint8_t&  set_sl10();
    uint8_t&  set_sl16();
    uint8_t&  set_sl20();
    uint8_t&  set_sl40();
    uint8_t&  set_sl80();
    uint8_t&  set_sl160();
    uint16_t& set_sl320();
    uint16_t& set_sl640();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                      periodicity_and_offset_present = false;
  bool                      res_present                    = false;
  uint8_t                   sched_request_res_id           = 1;
  uint8_t                   sched_request_id               = 0;
  periodicity_and_offset_c_ periodicity_and_offset;
  uint8_t                   res = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UCI-OnPUSCH ::= SEQUENCE
struct uci_on_pusch_s {
  struct beta_offsets_c_ {
    using dynamic_l_ = std::array<beta_offsets_s, 4>;
    struct types_opts {
      enum options { dynamic_type, semi_static, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    beta_offsets_c_() = default;
    beta_offsets_c_(const beta_offsets_c_& other);
    beta_offsets_c_& operator=(const beta_offsets_c_& other);
    ~beta_offsets_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dynamic_l_& dynamic_type()
    {
      assert_choice_type(types::dynamic_type, type_, "betaOffsets");
      return c.get<dynamic_l_>();
    }
    beta_offsets_s& semi_static()
    {
      assert_choice_type(types::semi_static, type_, "betaOffsets");
      return c.get<beta_offsets_s>();
    }
    const dynamic_l_& dynamic_type() const
    {
      assert_choice_type(types::dynamic_type, type_, "betaOffsets");
      return c.get<dynamic_l_>();
    }
    const beta_offsets_s& semi_static() const
    {
      assert_choice_type(types::semi_static, type_, "betaOffsets");
      return c.get<beta_offsets_s>();
    }
    dynamic_l_&     set_dynamic_type();
    beta_offsets_s& set_semi_static();

  private:
    types                                       type_;
    choice_buffer_t<beta_offsets_s, dynamic_l_> c;

    void destroy_();
  };
  struct scaling_opts {
    enum options { f0p5, f0p65, f0p8, f1, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<scaling_opts> scaling_e_;

  // member variables
  bool            beta_offsets_present = false;
  beta_offsets_c_ beta_offsets;
  scaling_e_      scaling;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BeamFailureRecoveryConfig ::= SEQUENCE
struct beam_fail_recovery_cfg_s {
  using candidate_beam_rs_list_l_ = dyn_array<prach_res_ded_bfr_c>;
  struct ssb_per_rach_occasion_opts {
    enum options { one_eighth, one_fourth, one_half, one, two, four, eight, sixteen, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<ssb_per_rach_occasion_opts> ssb_per_rach_occasion_e_;
  struct beam_fail_recovery_timer_opts {
    enum options { ms10, ms20, ms40, ms60, ms80, ms100, ms150, ms200, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<beam_fail_recovery_timer_opts> beam_fail_recovery_timer_e_;

  // member variables
  bool                        ext                              = false;
  bool                        root_seq_idx_bfr_present         = false;
  bool                        rach_cfg_bfr_present             = false;
  bool                        rsrp_thres_ssb_present           = false;
  bool                        ssb_per_rach_occasion_present    = false;
  bool                        ra_ssb_occasion_mask_idx_present = false;
  bool                        recovery_search_space_id_present = false;
  bool                        ra_prioritization_present        = false;
  bool                        beam_fail_recovery_timer_present = false;
  uint8_t                     root_seq_idx_bfr                 = 0;
  rach_cfg_generic_s          rach_cfg_bfr;
  uint8_t                     rsrp_thres_ssb = 0;
  candidate_beam_rs_list_l_   candidate_beam_rs_list;
  ssb_per_rach_occasion_e_    ssb_per_rach_occasion;
  uint8_t                     ra_ssb_occasion_mask_idx = 0;
  uint8_t                     recovery_search_space_id = 0;
  ra_prioritization_s         ra_prioritization;
  beam_fail_recovery_timer_e_ beam_fail_recovery_timer;
  // ...
  // group 0
  bool                 msg1_subcarrier_spacing_present = false;
  subcarrier_spacing_e msg1_subcarrier_spacing;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ConfiguredGrantConfig ::= SEQUENCE
struct cfgured_grant_cfg_s {
  struct freq_hop_opts {
    enum options { intra_slot, inter_slot, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<freq_hop_opts> freq_hop_e_;
  struct mcs_table_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mcs_table_opts> mcs_table_e_;
  struct mcs_table_transform_precoder_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mcs_table_transform_precoder_opts> mcs_table_transform_precoder_e_;
  struct res_alloc_opts {
    enum options { res_alloc_type0, res_alloc_type1, dynamic_switch, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<res_alloc_opts> res_alloc_e_;
  struct pwr_ctrl_loop_to_use_opts {
    enum options { n0, n1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pwr_ctrl_loop_to_use_opts> pwr_ctrl_loop_to_use_e_;
  struct transform_precoder_opts {
    enum options { enabled, disabled, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<transform_precoder_opts> transform_precoder_e_;
  struct rep_k_opts {
    enum options { n1, n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<rep_k_opts> rep_k_e_;
  struct rep_k_rv_opts {
    enum options { s1_minus0231, s2_minus0303, s3_minus0000, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<rep_k_rv_opts> rep_k_rv_e_;
  struct periodicity_opts {
    enum options {
      sym2,
      sym7,
      sym1x14,
      sym2x14,
      sym4x14,
      sym5x14,
      sym8x14,
      sym10x14,
      sym16x14,
      sym20x14,
      sym32x14,
      sym40x14,
      sym64x14,
      sym80x14,
      sym128x14,
      sym160x14,
      sym256x14,
      sym320x14,
      sym512x14,
      sym640x14,
      sym1024x14,
      sym1280x14,
      sym2560x14,
      sym5120x14,
      sym6,
      sym1x12,
      sym2x12,
      sym4x12,
      sym5x12,
      sym8x12,
      sym10x12,
      sym16x12,
      sym20x12,
      sym32x12,
      sym40x12,
      sym64x12,
      sym80x12,
      sym128x12,
      sym160x12,
      sym256x12,
      sym320x12,
      sym512x12,
      sym640x12,
      sym1280x12,
      sym2560x12,
      nulltype
    } value;

    const char* to_string() const;
  };
  typedef enumerated<periodicity_opts> periodicity_e_;
  struct rrc_cfgured_ul_grant_s_ {
    bool                ext                          = false;
    bool                dmrs_seq_initization_present = false;
    bool                srs_res_ind_present          = false;
    bool                freq_hop_offset_present      = false;
    uint16_t            time_domain_offset           = 0;
    uint8_t             time_domain_alloc            = 0;
    fixed_bitstring<18> freq_domain_alloc;
    uint8_t             ant_port                 = 0;
    uint8_t             dmrs_seq_initization     = 0;
    uint8_t             precoding_and_nof_layers = 0;
    uint8_t             srs_res_ind              = 0;
    uint8_t             mcs_and_tbs              = 0;
    uint16_t            freq_hop_offset          = 1;
    uint8_t             pathloss_ref_idx         = 0;
    // ...
  };

  // member variables
  bool                               ext                                  = false;
  bool                               freq_hop_present                     = false;
  bool                               mcs_table_present                    = false;
  bool                               mcs_table_transform_precoder_present = false;
  bool                               uci_on_pusch_present                 = false;
  bool                               rbg_size_present                     = false;
  bool                               transform_precoder_present           = false;
  bool                               rep_k_rv_present                     = false;
  bool                               cfgured_grant_timer_present          = false;
  bool                               rrc_cfgured_ul_grant_present         = false;
  freq_hop_e_                        freq_hop;
  dmrs_ul_cfg_s                      cg_dmrs_cfg;
  mcs_table_e_                       mcs_table;
  mcs_table_transform_precoder_e_    mcs_table_transform_precoder;
  setup_release_c<cg_uci_on_pusch_c> uci_on_pusch;
  res_alloc_e_                       res_alloc;
  pwr_ctrl_loop_to_use_e_            pwr_ctrl_loop_to_use;
  uint8_t                            p0_pusch_alpha = 0;
  transform_precoder_e_              transform_precoder;
  uint8_t                            nrof_harq_processes = 1;
  rep_k_e_                           rep_k;
  rep_k_rv_e_                        rep_k_rv;
  periodicity_e_                     periodicity;
  uint8_t                            cfgured_grant_timer = 1;
  rrc_cfgured_ul_grant_s_            rrc_cfgured_ul_grant;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-Config ::= SEQUENCE
struct pucch_cfg_s {
  using res_set_to_add_mod_list_l_               = dyn_array<pucch_res_set_s>;
  using res_set_to_release_list_l_               = bounded_array<uint8_t, 4>;
  using res_to_add_mod_list_l_                   = dyn_array<pucch_res_s>;
  using res_to_release_list_l_                   = dyn_array<uint8_t>;
  using sched_request_res_to_add_mod_list_l_     = dyn_array<sched_request_res_cfg_s>;
  using sched_request_res_to_release_list_l_     = bounded_array<uint8_t, 8>;
  using multi_csi_pucch_res_list_l_              = bounded_array<uint8_t, 2>;
  using dl_data_to_ul_ack_l_                     = bounded_array<uint8_t, 8>;
  using spatial_relation_info_to_add_mod_list_l_ = dyn_array<pucch_spatial_relation_info_s>;
  using spatial_relation_info_to_release_list_l_ = bounded_array<uint8_t, 8>;

  // member variables
  bool                                     ext                    = false;
  bool                                     format1_present        = false;
  bool                                     format2_present        = false;
  bool                                     format3_present        = false;
  bool                                     format4_present        = false;
  bool                                     pucch_pwr_ctrl_present = false;
  res_set_to_add_mod_list_l_               res_set_to_add_mod_list;
  res_set_to_release_list_l_               res_set_to_release_list;
  res_to_add_mod_list_l_                   res_to_add_mod_list;
  res_to_release_list_l_                   res_to_release_list;
  setup_release_c<pucch_format_cfg_s>      format1;
  setup_release_c<pucch_format_cfg_s>      format2;
  setup_release_c<pucch_format_cfg_s>      format3;
  setup_release_c<pucch_format_cfg_s>      format4;
  sched_request_res_to_add_mod_list_l_     sched_request_res_to_add_mod_list;
  sched_request_res_to_release_list_l_     sched_request_res_to_release_list;
  multi_csi_pucch_res_list_l_              multi_csi_pucch_res_list;
  dl_data_to_ul_ack_l_                     dl_data_to_ul_ack;
  spatial_relation_info_to_add_mod_list_l_ spatial_relation_info_to_add_mod_list;
  spatial_relation_info_to_release_list_l_ spatial_relation_info_to_release_list;
  pucch_pwr_ctrl_s                         pucch_pwr_ctrl;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-Config ::= SEQUENCE
struct pusch_cfg_s {
  struct tx_cfg_opts {
    enum options { codebook, non_codebook, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<tx_cfg_opts> tx_cfg_e_;
  struct freq_hop_opts {
    enum options { intra_slot, inter_slot, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<freq_hop_opts> freq_hop_e_;
  using freq_hop_offset_lists_l_ = bounded_array<uint16_t, 4>;
  struct res_alloc_opts {
    enum options { res_alloc_type0, res_alloc_type1, dynamic_switch, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<res_alloc_opts> res_alloc_e_;
  struct pusch_aggregation_factor_opts {
    enum options { n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pusch_aggregation_factor_opts> pusch_aggregation_factor_e_;
  struct mcs_table_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mcs_table_opts> mcs_table_e_;
  struct mcs_table_transform_precoder_opts {
    enum options { qam256, qam64_low_se, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mcs_table_transform_precoder_opts> mcs_table_transform_precoder_e_;
  struct transform_precoder_opts {
    enum options { enabled, disabled, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<transform_precoder_opts> transform_precoder_e_;
  struct codebook_subset_opts {
    enum options { fully_and_partial_and_non_coherent, partial_and_non_coherent, non_coherent, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<codebook_subset_opts> codebook_subset_e_;

  // member variables
  bool                                                               ext                                  = false;
  bool                                                               data_scrambling_id_pusch_present     = false;
  bool                                                               tx_cfg_present                       = false;
  bool                                                               dmrs_ul_for_pusch_map_type_a_present = false;
  bool                                                               dmrs_ul_for_pusch_map_type_b_present = false;
  bool                                                               pusch_pwr_ctrl_present               = false;
  bool                                                               freq_hop_present                     = false;
  bool                                                               pusch_time_domain_alloc_list_present = false;
  bool                                                               pusch_aggregation_factor_present     = false;
  bool                                                               mcs_table_present                    = false;
  bool                                                               mcs_table_transform_precoder_present = false;
  bool                                                               transform_precoder_present           = false;
  bool                                                               codebook_subset_present              = false;
  bool                                                               max_rank_present                     = false;
  bool                                                               rbg_size_present                     = false;
  bool                                                               uci_on_pusch_present                 = false;
  bool                                                               tp_pi2_bpsk_present                  = false;
  uint16_t                                                           data_scrambling_id_pusch             = 0;
  tx_cfg_e_                                                          tx_cfg;
  setup_release_c<dmrs_ul_cfg_s>                                     dmrs_ul_for_pusch_map_type_a;
  setup_release_c<dmrs_ul_cfg_s>                                     dmrs_ul_for_pusch_map_type_b;
  pusch_pwr_ctrl_s                                                   pusch_pwr_ctrl;
  freq_hop_e_                                                        freq_hop;
  freq_hop_offset_lists_l_                                           freq_hop_offset_lists;
  res_alloc_e_                                                       res_alloc;
  setup_release_c<dyn_seq_of<pusch_time_domain_res_alloc_s, 1, 16> > pusch_time_domain_alloc_list;
  pusch_aggregation_factor_e_                                        pusch_aggregation_factor;
  mcs_table_e_                                                       mcs_table;
  mcs_table_transform_precoder_e_                                    mcs_table_transform_precoder;
  transform_precoder_e_                                              transform_precoder;
  codebook_subset_e_                                                 codebook_subset;
  uint8_t                                                            max_rank = 1;
  setup_release_c<uci_on_pusch_s>                                    uci_on_pusch;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-Config ::= SEQUENCE
struct srs_cfg_s {
  using srs_res_set_to_release_list_l_ = bounded_array<uint8_t, 16>;
  using srs_res_set_to_add_mod_list_l_ = dyn_array<srs_res_set_s>;
  using srs_res_to_release_list_l_     = dyn_array<uint8_t>;
  using srs_res_to_add_mod_list_l_     = dyn_array<srs_res_s>;

  // member variables
  bool                           ext                      = false;
  bool                           tpc_accumulation_present = false;
  srs_res_set_to_release_list_l_ srs_res_set_to_release_list;
  srs_res_set_to_add_mod_list_l_ srs_res_set_to_add_mod_list;
  srs_res_to_release_list_l_     srs_res_to_release_list;
  srs_res_to_add_mod_list_l_     srs_res_to_add_mod_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-UplinkDedicated ::= SEQUENCE
struct bwp_ul_ded_s {
  bool                                      ext                            = false;
  bool                                      pucch_cfg_present              = false;
  bool                                      pusch_cfg_present              = false;
  bool                                      cfgured_grant_cfg_present      = false;
  bool                                      srs_cfg_present                = false;
  bool                                      beam_fail_recovery_cfg_present = false;
  setup_release_c<pucch_cfg_s>              pucch_cfg;
  setup_release_c<pusch_cfg_s>              pusch_cfg;
  setup_release_c<cfgured_grant_cfg_s>      cfgured_grant_cfg;
  setup_release_c<srs_cfg_s>                srs_cfg;
  setup_release_c<beam_fail_recovery_cfg_s> beam_fail_recovery_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BWP-Uplink ::= SEQUENCE
struct bwp_ul_s {
  bool            ext                = false;
  bool            bwp_common_present = false;
  bool            bwp_ded_present    = false;
  uint8_t         bwp_id             = 0;
  bwp_ul_common_s bwp_common;
  bwp_ul_ded_s    bwp_ded;
  // ...

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
    typedef enumerated<types_opts> types;

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

  // member variables
  bool          ext                                     = false;
  bool          csi_im_res_for_interference_present     = false;
  bool          nzp_csi_rs_res_for_interference_present = false;
  uint8_t       report_cfg_id                           = 0;
  res_for_ch_c_ res_for_ch;
  uint8_t       csi_im_res_for_interference     = 1;
  uint8_t       nzp_csi_rs_res_for_interference = 1;
  // ...

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
      typedef enumerated<subcarrier_location_p0_opts> subcarrier_location_p0_e_;

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
      typedef enumerated<subcarrier_location_p1_opts> subcarrier_location_p1_e_;

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
    typedef enumerated<types_opts> types;

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
  typedef enumerated<types_opts> types;

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
                typedef enumerated<types_opts> types;

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
                  return c.get<fixed_bitstring<8> >();
                }
                fixed_bitstring<64>& two_two_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::two_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<64> >();
                }
                fixed_bitstring<16>& four_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::four_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<16> >();
                }
                fixed_bitstring<96>& three_two_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::three_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<96> >();
                }
                fixed_bitstring<24>& six_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::six_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<24> >();
                }
                fixed_bitstring<128>& four_two_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::four_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<128> >();
                }
                fixed_bitstring<32>& eight_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::eight_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<32> >();
                }
                fixed_bitstring<192>& four_three_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::four_three_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<192> >();
                }
                fixed_bitstring<192>& six_two_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::six_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<192> >();
                }
                fixed_bitstring<48>& twelve_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::twelve_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<48> >();
                }
                fixed_bitstring<256>& four_four_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::four_four_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<256> >();
                }
                fixed_bitstring<256>& eight_two_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::eight_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<256> >();
                }
                fixed_bitstring<64>& sixteen_one_type_i_single_panel_restrict()
                {
                  assert_choice_type(types::sixteen_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<64> >();
                }
                const fixed_bitstring<8>& two_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::two_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<8> >();
                }
                const fixed_bitstring<64>& two_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::two_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<64> >();
                }
                const fixed_bitstring<16>& four_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::four_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<16> >();
                }
                const fixed_bitstring<96>& three_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::three_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<96> >();
                }
                const fixed_bitstring<24>& six_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::six_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<24> >();
                }
                const fixed_bitstring<128>& four_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::four_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<128> >();
                }
                const fixed_bitstring<32>& eight_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::eight_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<32> >();
                }
                const fixed_bitstring<192>& four_three_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::four_three_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<192> >();
                }
                const fixed_bitstring<192>& six_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::six_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<192> >();
                }
                const fixed_bitstring<48>& twelve_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::twelve_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<48> >();
                }
                const fixed_bitstring<256>& four_four_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::four_four_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<256> >();
                }
                const fixed_bitstring<256>& eight_two_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::eight_two_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<256> >();
                }
                const fixed_bitstring<64>& sixteen_one_type_i_single_panel_restrict() const
                {
                  assert_choice_type(types::sixteen_one_type_i_single_panel_restrict, type_, "n1-n2");
                  return c.get<fixed_bitstring<64> >();
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
                types                                  type_;
                choice_buffer_t<fixed_bitstring<256> > c;

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
            typedef enumerated<types_opts> types;

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
            typedef enumerated<types_opts> types;

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
              return c.get<fixed_bitstring<8> >();
            }
            fixed_bitstring<16>& two_four_one_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::two_four_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<16> >();
            }
            fixed_bitstring<8>& four_two_one_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::four_two_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<8> >();
            }
            fixed_bitstring<64>& two_two_two_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::two_two_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<64> >();
            }
            fixed_bitstring<32>& two_eight_one_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::two_eight_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<32> >();
            }
            fixed_bitstring<16>& four_four_one_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::four_four_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<16> >();
            }
            fixed_bitstring<128>& two_four_two_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::two_four_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<128> >();
            }
            fixed_bitstring<64>& four_two_two_type_i_multi_panel_restrict()
            {
              assert_choice_type(types::four_two_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<64> >();
            }
            const fixed_bitstring<8>& two_two_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::two_two_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<8> >();
            }
            const fixed_bitstring<16>& two_four_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::two_four_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<16> >();
            }
            const fixed_bitstring<8>& four_two_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::four_two_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<8> >();
            }
            const fixed_bitstring<64>& two_two_two_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::two_two_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<64> >();
            }
            const fixed_bitstring<32>& two_eight_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::two_eight_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<32> >();
            }
            const fixed_bitstring<16>& four_four_one_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::four_four_one_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<16> >();
            }
            const fixed_bitstring<128>& two_four_two_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::two_four_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<128> >();
            }
            const fixed_bitstring<64>& four_two_two_type_i_multi_panel_restrict() const
            {
              assert_choice_type(types::four_two_two_type_i_multi_panel_restrict, type_, "ng-n1-n2");
              return c.get<fixed_bitstring<64> >();
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
            types                                  type_;
            choice_buffer_t<fixed_bitstring<128> > c;

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
        typedef enumerated<types_opts> types;

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
            typedef enumerated<types_opts> types;

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
              return c.get<fixed_bitstring<16> >();
            }
            fixed_bitstring<43>& two_two()
            {
              assert_choice_type(types::two_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<43> >();
            }
            fixed_bitstring<32>& four_one()
            {
              assert_choice_type(types::four_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<32> >();
            }
            fixed_bitstring<59>& three_two()
            {
              assert_choice_type(types::three_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<59> >();
            }
            fixed_bitstring<48>& six_one()
            {
              assert_choice_type(types::six_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<48> >();
            }
            fixed_bitstring<75>& four_two()
            {
              assert_choice_type(types::four_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<75> >();
            }
            fixed_bitstring<64>& eight_one()
            {
              assert_choice_type(types::eight_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<64> >();
            }
            fixed_bitstring<107>& four_three()
            {
              assert_choice_type(types::four_three, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<107> >();
            }
            fixed_bitstring<107>& six_two()
            {
              assert_choice_type(types::six_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<107> >();
            }
            fixed_bitstring<96>& twelve_one()
            {
              assert_choice_type(types::twelve_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<96> >();
            }
            fixed_bitstring<139>& four_four()
            {
              assert_choice_type(types::four_four, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<139> >();
            }
            fixed_bitstring<139>& eight_two()
            {
              assert_choice_type(types::eight_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<139> >();
            }
            fixed_bitstring<128>& sixteen_one()
            {
              assert_choice_type(types::sixteen_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<128> >();
            }
            const fixed_bitstring<16>& two_one() const
            {
              assert_choice_type(types::two_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<16> >();
            }
            const fixed_bitstring<43>& two_two() const
            {
              assert_choice_type(types::two_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<43> >();
            }
            const fixed_bitstring<32>& four_one() const
            {
              assert_choice_type(types::four_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<32> >();
            }
            const fixed_bitstring<59>& three_two() const
            {
              assert_choice_type(types::three_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<59> >();
            }
            const fixed_bitstring<48>& six_one() const
            {
              assert_choice_type(types::six_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<48> >();
            }
            const fixed_bitstring<75>& four_two() const
            {
              assert_choice_type(types::four_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<75> >();
            }
            const fixed_bitstring<64>& eight_one() const
            {
              assert_choice_type(types::eight_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<64> >();
            }
            const fixed_bitstring<107>& four_three() const
            {
              assert_choice_type(types::four_three, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<107> >();
            }
            const fixed_bitstring<107>& six_two() const
            {
              assert_choice_type(types::six_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<107> >();
            }
            const fixed_bitstring<96>& twelve_one() const
            {
              assert_choice_type(types::twelve_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<96> >();
            }
            const fixed_bitstring<139>& four_four() const
            {
              assert_choice_type(types::four_four, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<139> >();
            }
            const fixed_bitstring<139>& eight_two() const
            {
              assert_choice_type(types::eight_two, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<139> >();
            }
            const fixed_bitstring<128>& sixteen_one() const
            {
              assert_choice_type(types::sixteen_one, type_, "n1-n2-codebookSubsetRestriction");
              return c.get<fixed_bitstring<128> >();
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
            types                                  type_;
            choice_buffer_t<fixed_bitstring<139> > c;

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
          typedef enumerated<port_sel_sampling_size_opts> port_sel_sampling_size_e_;

          // member variables
          bool                      port_sel_sampling_size_present = false;
          port_sel_sampling_size_e_ port_sel_sampling_size;
          fixed_bitstring<2>        type_ii_port_sel_ri_restrict;
        };
        struct types_opts {
          enum options { type_ii, type_ii_port_sel, nulltype } value;

          const char* to_string() const;
        };
        typedef enumerated<types_opts> types;

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
      typedef enumerated<phase_alphabet_size_opts> phase_alphabet_size_e_;
      struct nof_beams_opts {
        enum options { two, three, four, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<nof_beams_opts> nof_beams_e_;

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
    typedef enumerated<types_opts> types;

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
    using rank2_minus8_l_ = std::array<uint8_t, 2>;
    using rank3_minus8_l_ = std::array<uint8_t, 3>;
    using rank4_minus8_l_ = std::array<uint8_t, 4>;
    using rank5_minus8_l_ = std::array<uint8_t, 5>;
    using rank6_minus8_l_ = std::array<uint8_t, 6>;
    using rank7_minus8_l_ = std::array<uint8_t, 7>;
    using rank8_minus8_l_ = std::array<uint8_t, 8>;

    // member variables
    bool            rank1_minus8_present = false;
    bool            rank2_minus8_present = false;
    bool            rank3_minus8_present = false;
    bool            rank4_minus8_present = false;
    bool            rank5_minus8_present = false;
    bool            rank6_minus8_present = false;
    bool            rank7_minus8_present = false;
    bool            rank8_minus8_present = false;
    uint8_t         rank1_minus8         = 0;
    rank2_minus8_l_ rank2_minus8;
    rank3_minus8_l_ rank3_minus8;
    rank4_minus8_l_ rank4_minus8;
    rank5_minus8_l_ rank5_minus8;
    rank6_minus8_l_ rank6_minus8;
    rank7_minus8_l_ rank7_minus8;
    rank8_minus8_l_ rank8_minus8;
  };
  struct port_idx4_s_ {
    using rank2_minus4_l_ = std::array<uint8_t, 2>;
    using rank3_minus4_l_ = std::array<uint8_t, 3>;
    using rank4_minus4_l_ = std::array<uint8_t, 4>;

    // member variables
    bool            rank1_minus4_present = false;
    bool            rank2_minus4_present = false;
    bool            rank3_minus4_present = false;
    bool            rank4_minus4_present = false;
    uint8_t         rank1_minus4         = 0;
    rank2_minus4_l_ rank2_minus4;
    rank3_minus4_l_ rank3_minus4;
    rank4_minus4_l_ rank4_minus4;
  };
  struct port_idx2_s_ {
    using rank2_minus2_l_ = std::array<uint8_t, 2>;

    // member variables
    bool            rank1_minus2_present = false;
    bool            rank2_minus2_present = false;
    uint8_t         rank1_minus2         = 0;
    rank2_minus2_l_ rank2_minus2;
  };
  struct types_opts {
    enum options { port_idx8, port_idx4, port_idx2, port_idx1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

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
      typedef enumerated<report_slot_cfg_opts> report_slot_cfg_e_;
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
    typedef enumerated<types_opts> types;

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
      typedef enumerated<pdsch_bundle_size_for_csi_opts> pdsch_bundle_size_for_csi_e_;

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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<cqi_format_ind_opts> cqi_format_ind_e_;
    struct pmi_format_ind_opts {
      enum options { wideband_pmi, subband_pmi, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<pmi_format_ind_opts> pmi_format_ind_e_;
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
      typedef enumerated<types_opts, true, 1> types;

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
        return c.get<fixed_bitstring<3> >();
      }
      fixed_bitstring<4>& subbands4()
      {
        assert_choice_type(types::subbands4, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<4> >();
      }
      fixed_bitstring<5>& subbands5()
      {
        assert_choice_type(types::subbands5, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<5> >();
      }
      fixed_bitstring<6>& subbands6()
      {
        assert_choice_type(types::subbands6, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<6> >();
      }
      fixed_bitstring<7>& subbands7()
      {
        assert_choice_type(types::subbands7, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<7> >();
      }
      fixed_bitstring<8>& subbands8()
      {
        assert_choice_type(types::subbands8, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<8> >();
      }
      fixed_bitstring<9>& subbands9()
      {
        assert_choice_type(types::subbands9, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<9> >();
      }
      fixed_bitstring<10>& subbands10()
      {
        assert_choice_type(types::subbands10, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<10> >();
      }
      fixed_bitstring<11>& subbands11()
      {
        assert_choice_type(types::subbands11, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<11> >();
      }
      fixed_bitstring<12>& subbands12()
      {
        assert_choice_type(types::subbands12, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<12> >();
      }
      fixed_bitstring<13>& subbands13()
      {
        assert_choice_type(types::subbands13, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<13> >();
      }
      fixed_bitstring<14>& subbands14()
      {
        assert_choice_type(types::subbands14, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<14> >();
      }
      fixed_bitstring<15>& subbands15()
      {
        assert_choice_type(types::subbands15, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<15> >();
      }
      fixed_bitstring<16>& subbands16()
      {
        assert_choice_type(types::subbands16, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<16> >();
      }
      fixed_bitstring<17>& subbands17()
      {
        assert_choice_type(types::subbands17, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<17> >();
      }
      fixed_bitstring<18>& subbands18()
      {
        assert_choice_type(types::subbands18, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<18> >();
      }
      fixed_bitstring<19>& subbands19_v1530()
      {
        assert_choice_type(types::subbands19_v1530, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<19> >();
      }
      const fixed_bitstring<3>& subbands3() const
      {
        assert_choice_type(types::subbands3, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<3> >();
      }
      const fixed_bitstring<4>& subbands4() const
      {
        assert_choice_type(types::subbands4, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<4> >();
      }
      const fixed_bitstring<5>& subbands5() const
      {
        assert_choice_type(types::subbands5, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<5> >();
      }
      const fixed_bitstring<6>& subbands6() const
      {
        assert_choice_type(types::subbands6, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<6> >();
      }
      const fixed_bitstring<7>& subbands7() const
      {
        assert_choice_type(types::subbands7, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<7> >();
      }
      const fixed_bitstring<8>& subbands8() const
      {
        assert_choice_type(types::subbands8, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<8> >();
      }
      const fixed_bitstring<9>& subbands9() const
      {
        assert_choice_type(types::subbands9, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<9> >();
      }
      const fixed_bitstring<10>& subbands10() const
      {
        assert_choice_type(types::subbands10, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<10> >();
      }
      const fixed_bitstring<11>& subbands11() const
      {
        assert_choice_type(types::subbands11, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<11> >();
      }
      const fixed_bitstring<12>& subbands12() const
      {
        assert_choice_type(types::subbands12, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<12> >();
      }
      const fixed_bitstring<13>& subbands13() const
      {
        assert_choice_type(types::subbands13, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<13> >();
      }
      const fixed_bitstring<14>& subbands14() const
      {
        assert_choice_type(types::subbands14, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<14> >();
      }
      const fixed_bitstring<15>& subbands15() const
      {
        assert_choice_type(types::subbands15, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<15> >();
      }
      const fixed_bitstring<16>& subbands16() const
      {
        assert_choice_type(types::subbands16, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<16> >();
      }
      const fixed_bitstring<17>& subbands17() const
      {
        assert_choice_type(types::subbands17, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<17> >();
      }
      const fixed_bitstring<18>& subbands18() const
      {
        assert_choice_type(types::subbands18, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<18> >();
      }
      const fixed_bitstring<19>& subbands19_v1530() const
      {
        assert_choice_type(types::subbands19_v1530, type_, "csi-ReportingBand");
        return c.get<fixed_bitstring<19> >();
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
      types                                 type_;
      choice_buffer_t<fixed_bitstring<19> > c;

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
    enum options { cfgured, not_cfgured, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<time_restrict_for_ch_meass_opts> time_restrict_for_ch_meass_e_;
  struct time_restrict_for_interference_meass_opts {
    enum options { cfgured, not_cfgured, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<time_restrict_for_interference_meass_opts> time_restrict_for_interference_meass_e_;
  struct dummy_opts {
    enum options { n1, n2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dummy_opts> dummy_e_;
  struct group_based_beam_report_c_ {
    struct disabled_s_ {
      struct nrof_reported_rs_opts {
        enum options { n1, n2, n3, n4, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<nrof_reported_rs_opts> nrof_reported_rs_e_;

      // member variables
      bool                nrof_reported_rs_present = false;
      nrof_reported_rs_e_ nrof_reported_rs;
    };
    struct types_opts {
      enum options { enabled, disabled, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    enum options { table1, table2, table3, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<cqi_table_opts> cqi_table_e_;
  struct subband_size_opts {
    enum options { value1, value2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<subband_size_opts> subband_size_e_;
  using non_pmi_port_ind_l_ = dyn_array<port_idx_for8_ranks_c>;
  struct semi_persistent_on_pusch_v1530_s_ {
    struct report_slot_cfg_v1530_opts {
      enum options { sl4, sl8, sl16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<report_slot_cfg_v1530_opts> report_slot_cfg_v1530_e_;

    // member variables
    report_slot_cfg_v1530_e_ report_slot_cfg_v1530;
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
    typedef enumerated<types_opts> types;

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
  typedef enumerated<res_type_opts> res_type_e_;

  // member variables
  bool                   ext            = false;
  uint8_t                csi_res_cfg_id = 0;
  csi_rs_res_set_list_c_ csi_rs_res_set_list;
  uint8_t                bwp_id = 0;
  res_type_e_            res_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-SSB-ResourceSet ::= SEQUENCE
struct csi_ssb_res_set_s {
  using csi_ssb_res_list_l_ = dyn_array<uint8_t>;

  // member variables
  bool                ext                = false;
  uint8_t             csi_ssb_res_set_id = 0;
  csi_ssb_res_list_l_ csi_ssb_res_list;
  // ...

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
    enum options { db_minus3, db0, db3, db6, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<pwr_ctrl_offset_ss_opts> pwr_ctrl_offset_ss_e_;

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
  typedef enumerated<repeat_opts> repeat_e_;

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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-MeasConfig ::= SEQUENCE
struct csi_meas_cfg_s {
  using nzp_csi_rs_res_to_add_mod_list_l_     = dyn_array<nzp_csi_rs_res_s>;
  using nzp_csi_rs_res_to_release_list_l_     = dyn_array<uint8_t>;
  using nzp_csi_rs_res_set_to_add_mod_list_l_ = dyn_array<nzp_csi_rs_res_set_s>;
  using nzp_csi_rs_res_set_to_release_list_l_ = dyn_array<uint8_t>;
  using csi_im_res_to_add_mod_list_l_         = dyn_array<csi_im_res_s>;
  using csi_im_res_to_release_list_l_         = bounded_array<uint8_t, 32>;
  using csi_im_res_set_to_add_mod_list_l_     = dyn_array<csi_im_res_set_s>;
  using csi_im_res_set_to_release_list_l_     = dyn_array<uint8_t>;
  using csi_ssb_res_set_to_add_mod_list_l_    = dyn_array<csi_ssb_res_set_s>;
  using csi_ssb_res_set_to_release_list_l_    = dyn_array<uint8_t>;
  using csi_res_cfg_to_add_mod_list_l_        = dyn_array<csi_res_cfg_s>;
  using csi_res_cfg_to_release_list_l_        = dyn_array<uint8_t>;
  using csi_report_cfg_to_add_mod_list_l_     = dyn_array<csi_report_cfg_s>;
  using csi_report_cfg_to_release_list_l_     = dyn_array<uint8_t>;

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
  setup_release_c<dyn_seq_of<csi_aperiodic_trigger_state_s, 1, 128> > aperiodic_trigger_state_list;
  setup_release_c<dyn_seq_of<csi_semi_persistent_on_pusch_trigger_state_s, 1, 64> >
      semi_persistent_on_pusch_trigger_state_list;
  // ...

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
  typedef enumerated<radioframe_alloc_period_opts> radioframe_alloc_period_e_;
  struct sf_alloc1_c_ {
    struct types_opts {
      enum options { one_frame, four_frames, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
      return c.get<fixed_bitstring<6> >();
    }
    fixed_bitstring<24>& four_frames()
    {
      assert_choice_type(types::four_frames, type_, "subframeAllocation1");
      return c.get<fixed_bitstring<24> >();
    }
    const fixed_bitstring<6>& one_frame() const
    {
      assert_choice_type(types::one_frame, type_, "subframeAllocation1");
      return c.get<fixed_bitstring<6> >();
    }
    const fixed_bitstring<24>& four_frames() const
    {
      assert_choice_type(types::four_frames, type_, "subframeAllocation1");
      return c.get<fixed_bitstring<24> >();
    }
    fixed_bitstring<6>&  set_one_frame();
    fixed_bitstring<24>& set_four_frames();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<24> > c;

    void destroy_();
  };
  struct sf_alloc2_c_ {
    struct types_opts {
      enum options { one_frame, four_frames, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
      return c.get<fixed_bitstring<2> >();
    }
    fixed_bitstring<8>& four_frames()
    {
      assert_choice_type(types::four_frames, type_, "subframeAllocation2");
      return c.get<fixed_bitstring<8> >();
    }
    const fixed_bitstring<2>& one_frame() const
    {
      assert_choice_type(types::one_frame, type_, "subframeAllocation2");
      return c.get<fixed_bitstring<2> >();
    }
    const fixed_bitstring<8>& four_frames() const
    {
      assert_choice_type(types::four_frames, type_, "subframeAllocation2");
      return c.get<fixed_bitstring<8> >();
    }
    fixed_bitstring<2>& set_one_frame();
    fixed_bitstring<8>& set_four_frames();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<8> > c;

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

// EUTRA-MBSFN-SubframeConfigList ::= SEQUENCE (SIZE (1..8)) OF EUTRA-MBSFN-SubframeConfig
using eutra_mbsfn_sf_cfg_list_l = dyn_array<eutra_mbsfn_sf_cfg_s>;

// PUSCH-CodeBlockGroupTransmission ::= SEQUENCE
struct pusch_code_block_group_tx_s {
  struct max_code_block_groups_per_transport_block_opts {
    enum options { n2, n4, n6, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_code_block_groups_per_transport_block_opts> max_code_block_groups_per_transport_block_e_;

  // member variables
  bool                                         ext = false;
  max_code_block_groups_per_transport_block_e_ max_code_block_groups_per_transport_block;
  // ...

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
  typedef enumerated<max_code_block_groups_per_transport_block_opts> max_code_block_groups_per_transport_block_e_;

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

// PUSCH-ServingCellConfig ::= SEQUENCE
struct pusch_serving_cell_cfg_s {
  struct xoverhead_opts {
    enum options { xoh6, xoh12, xoh18, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<xoverhead_opts> xoverhead_e_;

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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RateMatchPatternLTE-CRS ::= SEQUENCE
struct rate_match_pattern_lte_crs_s {
  struct carrier_bw_dl_opts {
    enum options { n6, n15, n25, n50, n75, n100, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<carrier_bw_dl_opts> carrier_bw_dl_e_;
  struct nrof_crs_ports_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nrof_crs_ports_opts> nrof_crs_ports_e_;
  struct v_shift_opts {
    enum options { n0, n1, n2, n3, n4, n5, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<v_shift_opts> v_shift_e_;

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

// SRS-CarrierSwitching ::= SEQUENCE
struct srs_carrier_switching_s {
  struct srs_switch_from_carrier_opts {
    enum options { sul, nul, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<srs_switch_from_carrier_opts> srs_switch_from_carrier_e_;
  struct srs_tpc_pdcch_group_c_ {
    using type_a_l_ = dyn_array<srs_tpc_pdcch_cfg_s>;
    struct types_opts {
      enum options { type_a, type_b, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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

// SlotFormatIndicator ::= SEQUENCE
struct slot_format_ind_s {
  using slot_format_comb_to_add_mod_list_l_ = dyn_array<slot_format_combinations_per_cell_s>;
  using slot_format_comb_to_release_list_l_ = bounded_array<uint8_t, 16>;

  // member variables
  bool                                ext              = false;
  uint32_t                            sfi_rnti         = 0;
  uint8_t                             dci_payload_size = 1;
  slot_format_comb_to_add_mod_list_l_ slot_format_comb_to_add_mod_list;
  slot_format_comb_to_release_list_l_ slot_format_comb_to_release_list;
  // ...

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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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

  // member variables
  bool               ext = false;
  sched_cell_info_c_ sched_cell_info;
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
  typedef enumerated<xoverhead_opts> xoverhead_e_;
  struct nrof_harq_processes_for_pdsch_opts {
    enum options { n2, n4, n6, n10, n12, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nrof_harq_processes_for_pdsch_opts> nrof_harq_processes_for_pdsch_e_;

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
  typedef enumerated<bwp_inactivity_timer_opts> bwp_inactivity_timer_e_;
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
  typedef enumerated<scell_deactivation_timer_opts> scell_deactivation_timer_e_;
  struct pathloss_ref_linking_opts {
    enum options { sp_cell, scell, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<pathloss_ref_linking_opts> pathloss_ref_linking_e_;
  using rate_match_pattern_to_add_mod_list_l_ = dyn_array<rate_match_pattern_s>;
  using rate_match_pattern_to_release_list_l_ = bounded_array<uint8_t, 4>;
  using dl_ch_bw_per_scs_list_l_              = dyn_array<scs_specific_carrier_s>;

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
  bool                                      dummy_present                    = false;
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
  copy_ptr<setup_release_c<rate_match_pattern_lte_crs_s> > lte_crs_to_match_around;
  copy_ptr<rate_match_pattern_to_add_mod_list_l_>          rate_match_pattern_to_add_mod_list;
  copy_ptr<rate_match_pattern_to_release_list_l_>          rate_match_pattern_to_release_list;
  copy_ptr<dl_ch_bw_per_scs_list_l_>                       dl_ch_bw_per_scs_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1

#endif // SRSASN1_RRC_NR_SERVING_CELL_H

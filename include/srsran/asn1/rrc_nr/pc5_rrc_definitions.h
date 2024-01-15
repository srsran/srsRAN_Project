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

#include "ul_dcch_msg.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

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

// BandCombinationParametersSidelinkNR-r16 ::= SEQUENCE (SIZE (1..32)) OF BandParametersSidelink-r16
using band_combination_params_sidelink_nr_r16_l = dyn_array<band_params_sidelink_r16_s>;

// BandCombinationListSidelinkNR-r16 ::= SEQUENCE (SIZE (1..65536)) OF BandCombinationParametersSidelinkNR-r16
using band_combination_list_sidelink_nr_r16_l = dyn_array<band_combination_params_sidelink_nr_r16_l>;

// BandParametersSidelink-v1710 ::= SEQUENCE
struct band_params_sidelink_v1710_s {
  struct tx_iuc_scheme2_mode2_sidelink_r17_opts {
    enum options { n4, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using tx_iuc_scheme2_mode2_sidelink_r17_e_ = enumerated<tx_iuc_scheme2_mode2_sidelink_r17_opts>;

  // member variables
  bool                                 tx_iuc_scheme1_mode2_sidelink_r17_present = false;
  bool                                 tx_iuc_scheme2_mode2_sidelink_r17_present = false;
  tx_iuc_scheme2_mode2_sidelink_r17_e_ tx_iuc_scheme2_mode2_sidelink_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParametersSidelinkNR-v1710 ::= SEQUENCE (SIZE (1..32)) OF BandParametersSidelink-v1710
using band_combination_params_sidelink_nr_v1710_l = dyn_array<band_params_sidelink_v1710_s>;

// BandCombinationListSidelinkNR-v1710 ::= SEQUENCE (SIZE (1..65536)) OF BandCombinationParametersSidelinkNR-v1710
using band_combination_list_sidelink_nr_v1710_l = dyn_array<band_combination_params_sidelink_nr_v1710_l>;

// BandSidelinkPC5-r16 ::= SEQUENCE
struct band_sidelink_pc5_r16_s {
  struct sl_reception_r16_s_ {
    struct harq_rx_process_sidelink_r16_opts {
      enum options { n16, n24, n32, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using harq_rx_process_sidelink_r16_e_ = enumerated<harq_rx_process_sidelink_r16_opts>;
    struct pscch_rx_sidelink_r16_opts {
      enum options { value1, value2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using pscch_rx_sidelink_r16_e_ = enumerated<pscch_rx_sidelink_r16_opts>;
    struct scs_cp_pattern_rx_sidelink_r16_c_ {
      struct fr1_r16_s_ {
        bool                scs_15k_hz_r16_present = false;
        bool                scs_30k_hz_r16_present = false;
        bool                scs_60k_hz_r16_present = false;
        fixed_bitstring<16> scs_15k_hz_r16;
        fixed_bitstring<16> scs_30k_hz_r16;
        fixed_bitstring<16> scs_60k_hz_r16;
      };
      struct fr2_r16_s_ {
        bool                scs_60k_hz_r16_present  = false;
        bool                scs_120k_hz_r16_present = false;
        fixed_bitstring<16> scs_60k_hz_r16;
        fixed_bitstring<16> scs_120k_hz_r16;
      };
      struct types_opts {
        enum options { fr1_r16, fr2_r16, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      scs_cp_pattern_rx_sidelink_r16_c_() = default;
      scs_cp_pattern_rx_sidelink_r16_c_(const scs_cp_pattern_rx_sidelink_r16_c_& other);
      scs_cp_pattern_rx_sidelink_r16_c_& operator=(const scs_cp_pattern_rx_sidelink_r16_c_& other);
      ~scs_cp_pattern_rx_sidelink_r16_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      fr1_r16_s_& fr1_r16()
      {
        assert_choice_type(types::fr1_r16, type_, "scs-CP-PatternRxSidelink-r16");
        return c.get<fr1_r16_s_>();
      }
      fr2_r16_s_& fr2_r16()
      {
        assert_choice_type(types::fr2_r16, type_, "scs-CP-PatternRxSidelink-r16");
        return c.get<fr2_r16_s_>();
      }
      const fr1_r16_s_& fr1_r16() const
      {
        assert_choice_type(types::fr1_r16, type_, "scs-CP-PatternRxSidelink-r16");
        return c.get<fr1_r16_s_>();
      }
      const fr2_r16_s_& fr2_r16() const
      {
        assert_choice_type(types::fr2_r16, type_, "scs-CP-PatternRxSidelink-r16");
        return c.get<fr2_r16_s_>();
      }
      fr1_r16_s_& set_fr1_r16();
      fr2_r16_s_& set_fr2_r16();

    private:
      types                                   type_;
      choice_buffer_t<fr1_r16_s_, fr2_r16_s_> c;

      void destroy_();
    };

    // member variables
    bool                              scs_cp_pattern_rx_sidelink_r16_present = false;
    bool                              extended_cp_rx_sidelink_r16_present    = false;
    harq_rx_process_sidelink_r16_e_   harq_rx_process_sidelink_r16;
    pscch_rx_sidelink_r16_e_          pscch_rx_sidelink_r16;
    scs_cp_pattern_rx_sidelink_r16_c_ scs_cp_pattern_rx_sidelink_r16;
  };
  struct csi_report_sidelink_r16_s_ {
    struct csi_rs_ports_sidelink_r16_opts {
      enum options { p1, p2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using csi_rs_ports_sidelink_r16_e_ = enumerated<csi_rs_ports_sidelink_r16_opts>;

    // member variables
    csi_rs_ports_sidelink_r16_e_ csi_rs_ports_sidelink_r16;
  };
  struct rx_iuc_scheme2_mode2_sidelink_r17_opts {
    enum options { n5, n15, n25, n32, n35, n45, n50, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using rx_iuc_scheme2_mode2_sidelink_r17_e_ = enumerated<rx_iuc_scheme2_mode2_sidelink_r17_opts>;

  // member variables
  bool                ext                                          = false;
  bool                sl_reception_r16_present                     = false;
  bool                sl_tx_256_qam_r16_present                    = false;
  bool                low_se_64_qam_mcs_table_sidelink_r16_present = false;
  uint16_t            freq_band_sidelink_r16                       = 1;
  sl_reception_r16_s_ sl_reception_r16;
  // ...
  // group 0
  bool                                 rank_two_reception_r16_present                   = false;
  bool                                 sl_open_loop_pc_rsrp_report_sidelink_r16_present = false;
  bool                                 sl_rx_256_qam_r16_present                        = false;
  copy_ptr<csi_report_sidelink_r16_s_> csi_report_sidelink_r16;
  // group 1
  bool                                 rx_iuc_scheme1_preferred_mode2_sidelink_r17_present     = false;
  bool                                 rx_iuc_scheme1_non_preferred_mode2_sidelink_r17_present = false;
  bool                                 rx_iuc_scheme2_mode2_sidelink_r17_present               = false;
  bool                                 rx_iuc_scheme1_sci_r17_present                          = false;
  bool                                 rx_iuc_scheme1_sci_explicit_req_r17_present             = false;
  bool                                 scheme2_conflict_determination_rsrp_r17_present         = false;
  rx_iuc_scheme2_mode2_sidelink_r17_e_ rx_iuc_scheme2_mode2_sidelink_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-MeasQuantityResult-r16 ::= SEQUENCE
struct sl_meas_quant_result_r16_s {
  bool    ext                 = false;
  bool    sl_rsrp_r16_present = false;
  uint8_t sl_rsrp_r16         = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-MeasResult-r16 ::= SEQUENCE
struct sl_meas_result_r16_s {
  bool                       ext                        = false;
  bool                       sl_result_dmrs_r16_present = false;
  sl_meas_quant_result_r16_s sl_result_dmrs_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-MeasResults-r16 ::= SEQUENCE
struct sl_meas_results_r16_s {
  bool                 ext            = false;
  uint8_t              sl_meas_id_r16 = 1;
  sl_meas_result_r16_s sl_meas_result_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementReportSidelink-r16-IEs ::= SEQUENCE
struct meas_report_sidelink_r16_ies_s {
  bool                  non_crit_ext_present = false;
  sl_meas_results_r16_s sl_meas_results_r16;
  dyn_octstring         late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementReportSidelink ::= SEQUENCE
struct meas_report_sidelink_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { meas_report_sidelink_r16, crit_exts_future, nulltype } value;

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
    meas_report_sidelink_r16_ies_s& meas_report_sidelink_r16()
    {
      assert_choice_type(types::meas_report_sidelink_r16, type_, "criticalExtensions");
      return c;
    }
    const meas_report_sidelink_r16_ies_s& meas_report_sidelink_r16() const
    {
      assert_choice_type(types::meas_report_sidelink_r16, type_, "criticalExtensions");
      return c;
    }
    meas_report_sidelink_r16_ies_s& set_meas_report_sidelink_r16();
    void                            set_crit_exts_future();

  private:
    types                          type_;
    meas_report_sidelink_r16_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NotificationMessageSidelink-r17-IEs ::= SEQUENCE
struct notif_msg_sidelink_r17_ies_s {
  struct ind_type_r17_opts {
    enum options { relay_ue_uu_rlf, relay_ue_ho, relay_ue_cell_resel, relay_ue_uu_rrc_fail, nulltype } value;

    const char* to_string() const;
  };
  using ind_type_r17_e_ = enumerated<ind_type_r17_opts>;

  // member variables
  bool            ind_type_r17_present = false;
  bool            non_crit_ext_present = false;
  ind_type_r17_e_ ind_type_r17;
  dyn_octstring   late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NotificationMessageSidelink-r17 ::= SEQUENCE
struct notif_msg_sidelink_r17_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { notif_msg_sidelink_r17, crit_exts_future, nulltype } value;

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
    notif_msg_sidelink_r17_ies_s& notif_msg_sidelink_r17()
    {
      assert_choice_type(types::notif_msg_sidelink_r17, type_, "criticalExtensions");
      return c;
    }
    const notif_msg_sidelink_r17_ies_s& notif_msg_sidelink_r17() const
    {
      assert_choice_type(types::notif_msg_sidelink_r17, type_, "criticalExtensions");
      return c;
    }
    notif_msg_sidelink_r17_ies_s& set_notif_msg_sidelink_r17();
    void                          set_crit_exts_future();

  private:
    types                        type_;
    notif_msg_sidelink_r17_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationCompleteSidelink-v1720-IEs ::= SEQUENCE
struct rrc_recfg_complete_sidelink_v1720_ies_s {
  bool sl_drx_cfg_reject_v1720_present = false;
  bool non_crit_ext_present            = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationCompleteSidelink-v1710-IEs ::= SEQUENCE
struct rrc_recfg_complete_sidelink_v1710_ies_s {
  bool                                    non_crit_ext_present = false;
  rrc_recfg_complete_sidelink_v1720_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationCompleteSidelink-r16-IEs ::= SEQUENCE
struct rrc_recfg_complete_sidelink_r16_ies_s {
  bool                                    non_crit_ext_present = false;
  dyn_octstring                           late_non_crit_ext;
  rrc_recfg_complete_sidelink_v1710_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationCompleteSidelink ::= SEQUENCE
struct rrc_recfg_complete_sidelink_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_recfg_complete_sidelink_r16, crit_exts_future, nulltype } value;

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
    rrc_recfg_complete_sidelink_r16_ies_s& rrc_recfg_complete_sidelink_r16()
    {
      assert_choice_type(types::rrc_recfg_complete_sidelink_r16, type_, "criticalExtensions");
      return c;
    }
    const rrc_recfg_complete_sidelink_r16_ies_s& rrc_recfg_complete_sidelink_r16() const
    {
      assert_choice_type(types::rrc_recfg_complete_sidelink_r16, type_, "criticalExtensions");
      return c;
    }
    rrc_recfg_complete_sidelink_r16_ies_s& set_rrc_recfg_complete_sidelink_r16();
    void                                   set_crit_exts_future();

  private:
    types                                 type_;
    rrc_recfg_complete_sidelink_r16_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id_r16 = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationFailureSidelink-r16-IEs ::= SEQUENCE
struct rrc_recfg_fail_sidelink_r16_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationFailureSidelink ::= SEQUENCE
struct rrc_recfg_fail_sidelink_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_recfg_fail_sidelink_r16, crit_exts_future, nulltype } value;

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
    rrc_recfg_fail_sidelink_r16_ies_s& rrc_recfg_fail_sidelink_r16()
    {
      assert_choice_type(types::rrc_recfg_fail_sidelink_r16, type_, "criticalExtensions");
      return c;
    }
    const rrc_recfg_fail_sidelink_r16_ies_s& rrc_recfg_fail_sidelink_r16() const
    {
      assert_choice_type(types::rrc_recfg_fail_sidelink_r16, type_, "criticalExtensions");
      return c;
    }
    rrc_recfg_fail_sidelink_r16_ies_s& set_rrc_recfg_fail_sidelink_r16();
    void                               set_crit_exts_future();

  private:
    types                             type_;
    rrc_recfg_fail_sidelink_r16_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id_r16 = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-LogicalChannelConfigPC5-r16 ::= SEQUENCE
struct sl_lc_ch_cfg_pc5_r16_s {
  bool    ext             = false;
  uint8_t sl_lc_ch_id_r16 = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-RLC-ConfigPC5-r16 ::= CHOICE
struct sl_rlc_cfg_pc5_r16_c {
  struct sl_am_rlc_r16_s_ {
    bool              ext                            = false;
    bool              sl_sn_field_len_am_r16_present = false;
    sn_field_len_am_e sl_sn_field_len_am_r16;
    // ...
  };
  struct sl_um_bi_dir_rlc_r16_s_ {
    bool              ext                            = false;
    bool              sl_sn_field_len_um_r16_present = false;
    sn_field_len_um_e sl_sn_field_len_um_r16;
    // ...
  };
  struct sl_um_uni_dir_rlc_r16_s_ {
    bool              ext                            = false;
    bool              sl_sn_field_len_um_r16_present = false;
    sn_field_len_um_e sl_sn_field_len_um_r16;
    // ...
  };
  struct types_opts {
    enum options { sl_am_rlc_r16, sl_um_bi_dir_rlc_r16, sl_um_uni_dir_rlc_r16, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  sl_rlc_cfg_pc5_r16_c() = default;
  sl_rlc_cfg_pc5_r16_c(const sl_rlc_cfg_pc5_r16_c& other);
  sl_rlc_cfg_pc5_r16_c& operator=(const sl_rlc_cfg_pc5_r16_c& other);
  ~sl_rlc_cfg_pc5_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  sl_am_rlc_r16_s_& sl_am_rlc_r16()
  {
    assert_choice_type(types::sl_am_rlc_r16, type_, "SL-RLC-ConfigPC5-r16");
    return c.get<sl_am_rlc_r16_s_>();
  }
  sl_um_bi_dir_rlc_r16_s_& sl_um_bi_dir_rlc_r16()
  {
    assert_choice_type(types::sl_um_bi_dir_rlc_r16, type_, "SL-RLC-ConfigPC5-r16");
    return c.get<sl_um_bi_dir_rlc_r16_s_>();
  }
  sl_um_uni_dir_rlc_r16_s_& sl_um_uni_dir_rlc_r16()
  {
    assert_choice_type(types::sl_um_uni_dir_rlc_r16, type_, "SL-RLC-ConfigPC5-r16");
    return c.get<sl_um_uni_dir_rlc_r16_s_>();
  }
  const sl_am_rlc_r16_s_& sl_am_rlc_r16() const
  {
    assert_choice_type(types::sl_am_rlc_r16, type_, "SL-RLC-ConfigPC5-r16");
    return c.get<sl_am_rlc_r16_s_>();
  }
  const sl_um_bi_dir_rlc_r16_s_& sl_um_bi_dir_rlc_r16() const
  {
    assert_choice_type(types::sl_um_bi_dir_rlc_r16, type_, "SL-RLC-ConfigPC5-r16");
    return c.get<sl_um_bi_dir_rlc_r16_s_>();
  }
  const sl_um_uni_dir_rlc_r16_s_& sl_um_uni_dir_rlc_r16() const
  {
    assert_choice_type(types::sl_um_uni_dir_rlc_r16, type_, "SL-RLC-ConfigPC5-r16");
    return c.get<sl_um_uni_dir_rlc_r16_s_>();
  }
  sl_am_rlc_r16_s_&         set_sl_am_rlc_r16();
  sl_um_bi_dir_rlc_r16_s_&  set_sl_um_bi_dir_rlc_r16();
  sl_um_uni_dir_rlc_r16_s_& set_sl_um_uni_dir_rlc_r16();

private:
  types                                                                                type_;
  choice_buffer_t<sl_am_rlc_r16_s_, sl_um_bi_dir_rlc_r16_s_, sl_um_uni_dir_rlc_r16_s_> c;

  void destroy_();
};

// SL-PDCP-ConfigPC5-r16 ::= SEQUENCE
struct sl_pdcp_cfg_pc5_r16_s {
  struct sl_pdcp_sn_size_r16_opts {
    enum options { len12bits, len18bits, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using sl_pdcp_sn_size_r16_e_ = enumerated<sl_pdcp_sn_size_r16_opts>;

  // member variables
  bool                   ext                                  = false;
  bool                   sl_pdcp_sn_size_r16_present          = false;
  bool                   sl_out_of_order_delivery_r16_present = false;
  sl_pdcp_sn_size_r16_e_ sl_pdcp_sn_size_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-RLC-ChannelConfigPC5-r17 ::= SEQUENCE
struct sl_rlc_ch_cfg_pc5_r17_s {
  bool                   ext                              = false;
  bool                   sl_rlc_cfg_pc5_r17_present       = false;
  bool                   sl_mac_lc_ch_cfg_pc5_r17_present = false;
  uint16_t               sl_rlc_ch_id_pc5_r17             = 1;
  sl_rlc_cfg_pc5_r16_c   sl_rlc_cfg_pc5_r17;
  sl_lc_ch_cfg_pc5_r16_s sl_mac_lc_ch_cfg_pc5_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-SDAP-ConfigPC5-r16 ::= SEQUENCE
struct sl_sdap_cfg_pc5_r16_s {
  using sl_mapped_qos_flows_to_add_list_r16_l_     = dyn_array<uint8_t>;
  using sl_mapped_qos_flows_to_release_list_r16_l_ = dyn_array<uint8_t>;
  struct sl_sdap_hdr_r16_opts {
    enum options { present, absent, nulltype } value;

    const char* to_string() const;
  };
  using sl_sdap_hdr_r16_e_ = enumerated<sl_sdap_hdr_r16_opts>;

  // member variables
  bool                                       ext = false;
  sl_mapped_qos_flows_to_add_list_r16_l_     sl_mapped_qos_flows_to_add_list_r16;
  sl_mapped_qos_flows_to_release_list_r16_l_ sl_mapped_qos_flows_to_release_list_r16;
  sl_sdap_hdr_r16_e_                         sl_sdap_hdr_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationSidelink-v1700-IEs ::= SEQUENCE
struct rrc_recfg_sidelink_v1700_ies_s {
  using sl_rlc_ch_to_release_list_pc5_r17_l_ = dyn_array<uint16_t>;
  using sl_rlc_ch_to_add_mod_list_pc5_r17_l_ = dyn_array<sl_rlc_ch_cfg_pc5_r17_s>;

  // member variables
  bool                                      sl_drx_cfg_uc_pc5_r17_present           = false;
  bool                                      sl_latency_bound_iuc_report_r17_present = false;
  bool                                      non_crit_ext_present                    = false;
  setup_release_c<sl_drx_cfg_uc_r17_s>      sl_drx_cfg_uc_pc5_r17;
  setup_release_c<integer<uint8_t, 3, 160>> sl_latency_bound_iuc_report_r17;
  sl_rlc_ch_to_release_list_pc5_r17_l_      sl_rlc_ch_to_release_list_pc5_r17;
  sl_rlc_ch_to_add_mod_list_pc5_r17_l_      sl_rlc_ch_to_add_mod_list_pc5_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-CSI-RS-Config-r16 ::= SEQUENCE
struct sl_csi_rs_cfg_r16_s {
  struct sl_csi_rs_freq_alloc_r16_c_ {
    struct types_opts {
      enum options { sl_one_ant_port_r16, sl_two_ant_port_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    sl_csi_rs_freq_alloc_r16_c_() = default;
    sl_csi_rs_freq_alloc_r16_c_(const sl_csi_rs_freq_alloc_r16_c_& other);
    sl_csi_rs_freq_alloc_r16_c_& operator=(const sl_csi_rs_freq_alloc_r16_c_& other);
    ~sl_csi_rs_freq_alloc_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<12>& sl_one_ant_port_r16()
    {
      assert_choice_type(types::sl_one_ant_port_r16, type_, "sl-CSI-RS-FreqAllocation-r16");
      return c.get<fixed_bitstring<12>>();
    }
    fixed_bitstring<6>& sl_two_ant_port_r16()
    {
      assert_choice_type(types::sl_two_ant_port_r16, type_, "sl-CSI-RS-FreqAllocation-r16");
      return c.get<fixed_bitstring<6>>();
    }
    const fixed_bitstring<12>& sl_one_ant_port_r16() const
    {
      assert_choice_type(types::sl_one_ant_port_r16, type_, "sl-CSI-RS-FreqAllocation-r16");
      return c.get<fixed_bitstring<12>>();
    }
    const fixed_bitstring<6>& sl_two_ant_port_r16() const
    {
      assert_choice_type(types::sl_two_ant_port_r16, type_, "sl-CSI-RS-FreqAllocation-r16");
      return c.get<fixed_bitstring<6>>();
    }
    fixed_bitstring<12>& set_sl_one_ant_port_r16();
    fixed_bitstring<6>&  set_sl_two_ant_port_r16();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<12>> c;

    void destroy_();
  };

  // member variables
  bool                        ext                                = false;
  bool                        sl_csi_rs_freq_alloc_r16_present   = false;
  bool                        sl_csi_rs_first_symbol_r16_present = false;
  sl_csi_rs_freq_alloc_r16_c_ sl_csi_rs_freq_alloc_r16;
  uint8_t                     sl_csi_rs_first_symbol_r16 = 3;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SLRB-Config-r16 ::= SEQUENCE
struct slrb_cfg_r16_s {
  bool                   ext                              = false;
  bool                   sl_sdap_cfg_pc5_r16_present      = false;
  bool                   sl_pdcp_cfg_pc5_r16_present      = false;
  bool                   sl_rlc_cfg_pc5_r16_present       = false;
  bool                   sl_mac_lc_ch_cfg_pc5_r16_present = false;
  uint16_t               slrb_pc5_cfg_idx_r16             = 1;
  sl_sdap_cfg_pc5_r16_s  sl_sdap_cfg_pc5_r16;
  sl_pdcp_cfg_pc5_r16_s  sl_pdcp_cfg_pc5_r16;
  sl_rlc_cfg_pc5_r16_c   sl_rlc_cfg_pc5_r16;
  sl_lc_ch_cfg_pc5_r16_s sl_mac_lc_ch_cfg_pc5_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationSidelink-r16-IEs ::= SEQUENCE
struct rrc_recfg_sidelink_r16_ies_s {
  using slrb_cfg_to_add_mod_list_r16_l_ = dyn_array<slrb_cfg_r16_s>;
  using slrb_cfg_to_release_list_r16_l_ = dyn_array<uint16_t>;

  // member variables
  bool                                 sl_meas_cfg_r16_present                 = false;
  bool                                 sl_csi_rs_cfg_r16_present               = false;
  bool                                 sl_reset_cfg_r16_present                = false;
  bool                                 sl_latency_bound_csi_report_r16_present = false;
  bool                                 non_crit_ext_present                    = false;
  slrb_cfg_to_add_mod_list_r16_l_      slrb_cfg_to_add_mod_list_r16;
  slrb_cfg_to_release_list_r16_l_      slrb_cfg_to_release_list_r16;
  setup_release_c<sl_meas_cfg_r16_s>   sl_meas_cfg_r16;
  setup_release_c<sl_csi_rs_cfg_r16_s> sl_csi_rs_cfg_r16;
  uint8_t                              sl_latency_bound_csi_report_r16 = 3;
  dyn_octstring                        late_non_crit_ext;
  rrc_recfg_sidelink_v1700_ies_s       non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationSidelink ::= SEQUENCE
struct rrc_recfg_sidelink_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_recfg_sidelink_r16, crit_exts_future, nulltype } value;

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
    rrc_recfg_sidelink_r16_ies_s& rrc_recfg_sidelink_r16()
    {
      assert_choice_type(types::rrc_recfg_sidelink_r16, type_, "criticalExtensions");
      return c;
    }
    const rrc_recfg_sidelink_r16_ies_s& rrc_recfg_sidelink_r16() const
    {
      assert_choice_type(types::rrc_recfg_sidelink_r16, type_, "criticalExtensions");
      return c;
    }
    rrc_recfg_sidelink_r16_ies_s& set_rrc_recfg_sidelink_r16();
    void                          set_crit_exts_future();

  private:
    types                        type_;
    rrc_recfg_sidelink_r16_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id_r16 = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-SIB-ReqInfo-r17 ::= ENUMERATED
struct sl_sib_req_info_r17_opts {
  enum options {
    sib1,
    sib2,
    sib3,
    sib4,
    sib5,
    sib6,
    sib7,
    sib8,
    sib9,
    sib10,
    sib11,
    sib12,
    sib13,
    sib14,
    sib15,
    sib16,
    sib17,
    sib18,
    sib19,
    sib20,
    sib21,
    sib_not_req11,
    sib_not_req10,
    sib_not_req9,
    sib_not_req8,
    sib_not_req7,
    sib_not_req6,
    sib_not_req5,
    sib_not_req4,
    sib_not_req3,
    sib_not_req2,
    sib_not_req1,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
using sl_sib_req_info_r17_e = enumerated<sl_sib_req_info_r17_opts, true>;

// SL-PagingInfo-RemoteUE-r17 ::= SEQUENCE
struct sl_paging_info_remote_ue_r17_s {
  bool                         sl_paging_cycle_remote_ue_r17_present = false;
  sl_paging_id_remote_ue_r17_s sl_paging_id_remote_ue_r17;
  paging_cycle_e               sl_paging_cycle_remote_ue_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-RequestedSIB-List-r17 ::= SEQUENCE (SIZE (33)) OF SL-SIB-ReqInfo-r17
using sl_requested_sib_list_r17_l = std::array<sl_sib_req_info_r17_e, 33>;

// RemoteUEInformationSidelink-r17-IEs ::= SEQUENCE
struct remote_ue_info_sidelink_r17_ies_s {
  bool                                            sl_requested_sib_list_r17_present    = false;
  bool                                            sl_paging_info_remote_ue_r17_present = false;
  bool                                            non_crit_ext_present                 = false;
  setup_release_c<sl_requested_sib_list_r17_l>    sl_requested_sib_list_r17;
  setup_release_c<sl_paging_info_remote_ue_r17_s> sl_paging_info_remote_ue_r17;
  dyn_octstring                                   late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RemoteUEInformationSidelink-r17 ::= SEQUENCE
struct remote_ue_info_sidelink_r17_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { remote_ue_info_sidelink_r17, crit_exts_future, nulltype } value;

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
    remote_ue_info_sidelink_r17_ies_s& remote_ue_info_sidelink_r17()
    {
      assert_choice_type(types::remote_ue_info_sidelink_r17, type_, "criticalExtensions");
      return c;
    }
    const remote_ue_info_sidelink_r17_ies_s& remote_ue_info_sidelink_r17() const
    {
      assert_choice_type(types::remote_ue_info_sidelink_r17, type_, "criticalExtensions");
      return c;
    }
    remote_ue_info_sidelink_r17_ies_s& set_remote_ue_info_sidelink_r17();
    void                               set_crit_exts_future();

  private:
    types                             type_;
    remote_ue_info_sidelink_r17_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MasterInformationBlockSidelink ::= SEQUENCE
struct mib_sidelink_s {
  fixed_bitstring<12> sl_tdd_cfg_r16;
  bool                in_coverage_r16 = false;
  fixed_bitstring<10> direct_frame_num_r16;
  fixed_bitstring<7>  slot_idx_r16;
  fixed_bitstring<2>  reserved_bits_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SBCCH-SL-BCH-MessageType ::= CHOICE
struct sbcch_sl_bch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { mib_sidelink, spare1, nulltype } value;

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
    mib_sidelink_s& mib_sidelink()
    {
      assert_choice_type(types::mib_sidelink, type_, "c1");
      return c;
    }
    const mib_sidelink_s& mib_sidelink() const
    {
      assert_choice_type(types::mib_sidelink, type_, "c1");
      return c;
    }
    mib_sidelink_s& set_mib_sidelink();
    void            set_spare1();

  private:
    types          type_;
    mib_sidelink_s c;
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  sbcch_sl_bch_msg_type_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "SBCCH-SL-BCH-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "SBCCH-SL-BCH-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// SBCCH-SL-BCH-Message ::= SEQUENCE
struct sbcch_sl_bch_msg_s {
  sbcch_sl_bch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-ParametersSidelink-r17 ::= SEQUENCE
struct mac_params_sidelink_r17_s {
  bool ext                         = false;
  bool drx_on_sidelink_r17_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AccessStratumReleaseSidelink-r16 ::= ENUMERATED
struct access_stratum_release_sidelink_r16_opts {
  enum options { rel16, rel17, spare6, spare5, spare4, spare3, spare2, spare1, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using access_stratum_release_sidelink_r16_e = enumerated<access_stratum_release_sidelink_r16_opts, true>;

// PDCP-ParametersSidelink-r16 ::= SEQUENCE
struct pdcp_params_sidelink_r16_s {
  bool ext                                        = false;
  bool out_of_order_delivery_sidelink_r16_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityInformationSidelink-v1700-IEs ::= SEQUENCE
struct ue_cap_info_sidelink_v1700_ies_s {
  bool                                      mac_params_sidelink_r17_present = false;
  bool                                      non_crit_ext_present            = false;
  mac_params_sidelink_r17_s                 mac_params_sidelink_r17;
  band_combination_list_sidelink_nr_v1710_l supported_band_combination_list_sidelink_nr_v1710;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEAssistanceInformationSidelink-r17-IEs ::= SEQUENCE
struct ue_assist_info_sidelink_r17_ies_s {
  using sl_preferred_drx_cfg_list_r17_l_ = dyn_array<sl_drx_cfg_uc_semi_static_r17_s>;

  // member variables
  bool                             non_crit_ext_present = false;
  sl_preferred_drx_cfg_list_r17_l_ sl_preferred_drx_cfg_list_r17;
  dyn_octstring                    late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityEnquirySidelink-r16-IEs ::= SEQUENCE
struct ue_cap_enquiry_sidelink_r16_ies_s {
  bool             non_crit_ext_present = false;
  freq_band_list_l freq_band_list_filt_sidelink_r16;
  dyn_octstring    ue_cap_info_sidelink_r16;
  dyn_octstring    late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityInformationSidelink-r16-IEs ::= SEQUENCE
struct ue_cap_info_sidelink_r16_ies_s {
  using supported_band_list_sidelink_r16_l_ = dyn_array<band_sidelink_pc5_r16_s>;

  // member variables
  bool                                    pdcp_params_sidelink_r16_present = false;
  bool                                    rlc_params_sidelink_r16_present  = false;
  bool                                    non_crit_ext_present             = false;
  access_stratum_release_sidelink_r16_e   access_stratum_release_sidelink_r16;
  pdcp_params_sidelink_r16_s              pdcp_params_sidelink_r16;
  rlc_params_sidelink_r16_s               rlc_params_sidelink_r16;
  band_combination_list_sidelink_nr_r16_l supported_band_combination_list_sidelink_nr_r16;
  supported_band_list_sidelink_r16_l_     supported_band_list_sidelink_r16;
  freq_band_list_l                        applied_freq_band_list_filt_r16;
  dyn_octstring                           late_non_crit_ext;
  ue_cap_info_sidelink_v1700_ies_s        non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UuMessageTransferSidelink-r17-IEs ::= SEQUENCE
struct uu_msg_transfer_sidelink_r17_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring sl_paging_delivery_r17;
  dyn_octstring sl_sib1_delivery_r17;
  dyn_octstring sl_sys_info_delivery_r17;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEAssistanceInformationSidelink-r17 ::= SEQUENCE
struct ue_assist_info_sidelink_r17_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_assist_info_sidelink_r17, crit_exts_future, nulltype } value;

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
    ue_assist_info_sidelink_r17_ies_s& ue_assist_info_sidelink_r17()
    {
      assert_choice_type(types::ue_assist_info_sidelink_r17, type_, "criticalExtensions");
      return c;
    }
    const ue_assist_info_sidelink_r17_ies_s& ue_assist_info_sidelink_r17() const
    {
      assert_choice_type(types::ue_assist_info_sidelink_r17, type_, "criticalExtensions");
      return c;
    }
    ue_assist_info_sidelink_r17_ies_s& set_ue_assist_info_sidelink_r17();
    void                               set_crit_exts_future();

  private:
    types                             type_;
    ue_assist_info_sidelink_r17_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityEnquirySidelink ::= SEQUENCE
struct ue_cap_enquiry_sidelink_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_cap_enquiry_sidelink_r16, crit_exts_future, nulltype } value;

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
    ue_cap_enquiry_sidelink_r16_ies_s& ue_cap_enquiry_sidelink_r16()
    {
      assert_choice_type(types::ue_cap_enquiry_sidelink_r16, type_, "criticalExtensions");
      return c;
    }
    const ue_cap_enquiry_sidelink_r16_ies_s& ue_cap_enquiry_sidelink_r16() const
    {
      assert_choice_type(types::ue_cap_enquiry_sidelink_r16, type_, "criticalExtensions");
      return c;
    }
    ue_cap_enquiry_sidelink_r16_ies_s& set_ue_cap_enquiry_sidelink_r16();
    void                               set_crit_exts_future();

  private:
    types                             type_;
    ue_cap_enquiry_sidelink_r16_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id_r16 = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityInformationSidelink ::= SEQUENCE
struct ue_cap_info_sidelink_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_cap_info_sidelink_r16, crit_exts_future, nulltype } value;

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
    ue_cap_info_sidelink_r16_ies_s& ue_cap_info_sidelink_r16()
    {
      assert_choice_type(types::ue_cap_info_sidelink_r16, type_, "criticalExtensions");
      return c;
    }
    const ue_cap_info_sidelink_r16_ies_s& ue_cap_info_sidelink_r16() const
    {
      assert_choice_type(types::ue_cap_info_sidelink_r16, type_, "criticalExtensions");
      return c;
    }
    ue_cap_info_sidelink_r16_ies_s& set_ue_cap_info_sidelink_r16();
    void                            set_crit_exts_future();

  private:
    types                          type_;
    ue_cap_info_sidelink_r16_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id_r16 = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UuMessageTransferSidelink-r17 ::= SEQUENCE
struct uu_msg_transfer_sidelink_r17_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { uu_msg_transfer_sidelink_r17, crit_exts_future, nulltype } value;

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
    uu_msg_transfer_sidelink_r17_ies_s& uu_msg_transfer_sidelink_r17()
    {
      assert_choice_type(types::uu_msg_transfer_sidelink_r17, type_, "criticalExtensions");
      return c;
    }
    const uu_msg_transfer_sidelink_r17_ies_s& uu_msg_transfer_sidelink_r17() const
    {
      assert_choice_type(types::uu_msg_transfer_sidelink_r17, type_, "criticalExtensions");
      return c;
    }
    uu_msg_transfer_sidelink_r17_ies_s& set_uu_msg_transfer_sidelink_r17();
    void                                set_crit_exts_future();

  private:
    types                              type_;
    uu_msg_transfer_sidelink_r17_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCCH-MessageType ::= CHOICE
struct s_cch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options {
        meas_report_sidelink,
        rrc_recfg_sidelink,
        rrc_recfg_complete_sidelink,
        rrc_recfg_fail_sidelink,
        ue_cap_enquiry_sidelink,
        ue_cap_info_sidelink,
        uu_msg_transfer_sidelink_r17,
        remote_ue_info_sidelink_r17,
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
    meas_report_sidelink_s& meas_report_sidelink()
    {
      assert_choice_type(types::meas_report_sidelink, type_, "c1");
      return c.get<meas_report_sidelink_s>();
    }
    rrc_recfg_sidelink_s& rrc_recfg_sidelink()
    {
      assert_choice_type(types::rrc_recfg_sidelink, type_, "c1");
      return c.get<rrc_recfg_sidelink_s>();
    }
    rrc_recfg_complete_sidelink_s& rrc_recfg_complete_sidelink()
    {
      assert_choice_type(types::rrc_recfg_complete_sidelink, type_, "c1");
      return c.get<rrc_recfg_complete_sidelink_s>();
    }
    rrc_recfg_fail_sidelink_s& rrc_recfg_fail_sidelink()
    {
      assert_choice_type(types::rrc_recfg_fail_sidelink, type_, "c1");
      return c.get<rrc_recfg_fail_sidelink_s>();
    }
    ue_cap_enquiry_sidelink_s& ue_cap_enquiry_sidelink()
    {
      assert_choice_type(types::ue_cap_enquiry_sidelink, type_, "c1");
      return c.get<ue_cap_enquiry_sidelink_s>();
    }
    ue_cap_info_sidelink_s& ue_cap_info_sidelink()
    {
      assert_choice_type(types::ue_cap_info_sidelink, type_, "c1");
      return c.get<ue_cap_info_sidelink_s>();
    }
    uu_msg_transfer_sidelink_r17_s& uu_msg_transfer_sidelink_r17()
    {
      assert_choice_type(types::uu_msg_transfer_sidelink_r17, type_, "c1");
      return c.get<uu_msg_transfer_sidelink_r17_s>();
    }
    remote_ue_info_sidelink_r17_s& remote_ue_info_sidelink_r17()
    {
      assert_choice_type(types::remote_ue_info_sidelink_r17, type_, "c1");
      return c.get<remote_ue_info_sidelink_r17_s>();
    }
    const meas_report_sidelink_s& meas_report_sidelink() const
    {
      assert_choice_type(types::meas_report_sidelink, type_, "c1");
      return c.get<meas_report_sidelink_s>();
    }
    const rrc_recfg_sidelink_s& rrc_recfg_sidelink() const
    {
      assert_choice_type(types::rrc_recfg_sidelink, type_, "c1");
      return c.get<rrc_recfg_sidelink_s>();
    }
    const rrc_recfg_complete_sidelink_s& rrc_recfg_complete_sidelink() const
    {
      assert_choice_type(types::rrc_recfg_complete_sidelink, type_, "c1");
      return c.get<rrc_recfg_complete_sidelink_s>();
    }
    const rrc_recfg_fail_sidelink_s& rrc_recfg_fail_sidelink() const
    {
      assert_choice_type(types::rrc_recfg_fail_sidelink, type_, "c1");
      return c.get<rrc_recfg_fail_sidelink_s>();
    }
    const ue_cap_enquiry_sidelink_s& ue_cap_enquiry_sidelink() const
    {
      assert_choice_type(types::ue_cap_enquiry_sidelink, type_, "c1");
      return c.get<ue_cap_enquiry_sidelink_s>();
    }
    const ue_cap_info_sidelink_s& ue_cap_info_sidelink() const
    {
      assert_choice_type(types::ue_cap_info_sidelink, type_, "c1");
      return c.get<ue_cap_info_sidelink_s>();
    }
    const uu_msg_transfer_sidelink_r17_s& uu_msg_transfer_sidelink_r17() const
    {
      assert_choice_type(types::uu_msg_transfer_sidelink_r17, type_, "c1");
      return c.get<uu_msg_transfer_sidelink_r17_s>();
    }
    const remote_ue_info_sidelink_r17_s& remote_ue_info_sidelink_r17() const
    {
      assert_choice_type(types::remote_ue_info_sidelink_r17, type_, "c1");
      return c.get<remote_ue_info_sidelink_r17_s>();
    }
    meas_report_sidelink_s&         set_meas_report_sidelink();
    rrc_recfg_sidelink_s&           set_rrc_recfg_sidelink();
    rrc_recfg_complete_sidelink_s&  set_rrc_recfg_complete_sidelink();
    rrc_recfg_fail_sidelink_s&      set_rrc_recfg_fail_sidelink();
    ue_cap_enquiry_sidelink_s&      set_ue_cap_enquiry_sidelink();
    ue_cap_info_sidelink_s&         set_ue_cap_info_sidelink();
    uu_msg_transfer_sidelink_r17_s& set_uu_msg_transfer_sidelink_r17();
    remote_ue_info_sidelink_r17_s&  set_remote_ue_info_sidelink_r17();

  private:
    types type_;
    choice_buffer_t<meas_report_sidelink_s,
                    remote_ue_info_sidelink_r17_s,
                    rrc_recfg_complete_sidelink_s,
                    rrc_recfg_fail_sidelink_s,
                    rrc_recfg_sidelink_s,
                    ue_cap_enquiry_sidelink_s,
                    ue_cap_info_sidelink_s,
                    uu_msg_transfer_sidelink_r17_s>
        c;

    void destroy_();
  };
  struct msg_class_ext_c_ {
    struct c2_c_ {
      struct types_opts {
        enum options {
          notif_msg_sidelink_r17,
          ue_assist_info_sidelink_r17,
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
      c2_c_() = default;
      c2_c_(const c2_c_& other);
      c2_c_& operator=(const c2_c_& other);
      ~c2_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      notif_msg_sidelink_r17_s& notif_msg_sidelink_r17()
      {
        assert_choice_type(types::notif_msg_sidelink_r17, type_, "c2");
        return c.get<notif_msg_sidelink_r17_s>();
      }
      ue_assist_info_sidelink_r17_s& ue_assist_info_sidelink_r17()
      {
        assert_choice_type(types::ue_assist_info_sidelink_r17, type_, "c2");
        return c.get<ue_assist_info_sidelink_r17_s>();
      }
      const notif_msg_sidelink_r17_s& notif_msg_sidelink_r17() const
      {
        assert_choice_type(types::notif_msg_sidelink_r17, type_, "c2");
        return c.get<notif_msg_sidelink_r17_s>();
      }
      const ue_assist_info_sidelink_r17_s& ue_assist_info_sidelink_r17() const
      {
        assert_choice_type(types::ue_assist_info_sidelink_r17, type_, "c2");
        return c.get<ue_assist_info_sidelink_r17_s>();
      }
      notif_msg_sidelink_r17_s&      set_notif_msg_sidelink_r17();
      ue_assist_info_sidelink_r17_s& set_ue_assist_info_sidelink_r17();
      void                           set_spare6();
      void                           set_spare5();
      void                           set_spare4();
      void                           set_spare3();
      void                           set_spare2();
      void                           set_spare1();

    private:
      types                                                                    type_;
      choice_buffer_t<notif_msg_sidelink_r17_s, ue_assist_info_sidelink_r17_s> c;

      void destroy_();
    };
    struct types_opts {
      enum options { c2, msg_class_ext_future_r17, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    msg_class_ext_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c2_c_& c2()
    {
      assert_choice_type(types::c2, type_, "messageClassExtension");
      return c;
    }
    const c2_c_& c2() const
    {
      assert_choice_type(types::c2, type_, "messageClassExtension");
      return c;
    }
    c2_c_& set_c2();
    void   set_msg_class_ext_future_r17();

  private:
    types type_;
    c2_c_ c;
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  s_cch_msg_type_c() = default;
  s_cch_msg_type_c(const s_cch_msg_type_c& other);
  s_cch_msg_type_c& operator=(const s_cch_msg_type_c& other);
  ~s_cch_msg_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "SCCH-MessageType");
    return c.get<c1_c_>();
  }
  msg_class_ext_c_& msg_class_ext()
  {
    assert_choice_type(types::msg_class_ext, type_, "SCCH-MessageType");
    return c.get<msg_class_ext_c_>();
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "SCCH-MessageType");
    return c.get<c1_c_>();
  }
  const msg_class_ext_c_& msg_class_ext() const
  {
    assert_choice_type(types::msg_class_ext, type_, "SCCH-MessageType");
    return c.get<msg_class_ext_c_>();
  }
  c1_c_&            set_c1();
  msg_class_ext_c_& set_msg_class_ext();

private:
  types                                    type_;
  choice_buffer_t<c1_c_, msg_class_ext_c_> c;

  void destroy_();
};

// SCCH-Message ::= SEQUENCE
struct s_cch_msg_s {
  s_cch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-MeasResultRelay-r17 ::= SEQUENCE
struct sl_meas_result_relay_r17_s {
  bool                       ext = false;
  cell_access_related_info_s cell_id_r17;
  fixed_bitstring<24>        sl_relay_ue_id_r17;
  sl_meas_result_r16_s       sl_meas_result_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-MeasResultListRelay-r17 ::= SEQUENCE (SIZE (1..32)) OF SL-MeasResultRelay-r17
using sl_meas_result_list_relay_r17_l = dyn_array<sl_meas_result_relay_r17_s>;

} // namespace rrc_nr
} // namespace asn1

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

#include "dl_dcch_msg.h"
#include "ul_dcch_msg.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// CLI-RSSI-TriggeredList-r16 ::= SEQUENCE (SIZE (1..64)) OF INTEGER (0..63)
using cli_rssi_triggered_list_r16_l = dyn_array<uint8_t>;

// SRS-RSRP-TriggeredList-r16 ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..63)
using srs_rsrp_triggered_list_r16_l = bounded_array<uint8_t, 32>;

// CLI-TriggeredList-r16 ::= CHOICE
struct cli_triggered_list_r16_c {
  struct types_opts {
    enum options { srs_rsrp_triggered_list_r16, cli_rssi_triggered_list_r16, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  cli_triggered_list_r16_c() = default;
  cli_triggered_list_r16_c(const cli_triggered_list_r16_c& other);
  cli_triggered_list_r16_c& operator=(const cli_triggered_list_r16_c& other);
  ~cli_triggered_list_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  srs_rsrp_triggered_list_r16_l& srs_rsrp_triggered_list_r16()
  {
    assert_choice_type(types::srs_rsrp_triggered_list_r16, type_, "CLI-TriggeredList-r16");
    return c.get<srs_rsrp_triggered_list_r16_l>();
  }
  cli_rssi_triggered_list_r16_l& cli_rssi_triggered_list_r16()
  {
    assert_choice_type(types::cli_rssi_triggered_list_r16, type_, "CLI-TriggeredList-r16");
    return c.get<cli_rssi_triggered_list_r16_l>();
  }
  const srs_rsrp_triggered_list_r16_l& srs_rsrp_triggered_list_r16() const
  {
    assert_choice_type(types::srs_rsrp_triggered_list_r16, type_, "CLI-TriggeredList-r16");
    return c.get<srs_rsrp_triggered_list_r16_l>();
  }
  const cli_rssi_triggered_list_r16_l& cli_rssi_triggered_list_r16() const
  {
    assert_choice_type(types::cli_rssi_triggered_list_r16, type_, "CLI-TriggeredList-r16");
    return c.get<cli_rssi_triggered_list_r16_l>();
  }
  srs_rsrp_triggered_list_r16_l& set_srs_rsrp_triggered_list_r16();
  cli_rssi_triggered_list_r16_l& set_cli_rssi_triggered_list_r16();

private:
  types                                                                         type_;
  choice_buffer_t<cli_rssi_triggered_list_r16_l, srs_rsrp_triggered_list_r16_l> c;

  void destroy_();
};

struct cells_triggered_list_item_c_ {
  struct types_opts {
    enum options { pci, pci_eutra, pci_utra_fdd_r16, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

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
  uint16_t& pci_utra_fdd_r16()
  {
    assert_choice_type(types::pci_utra_fdd_r16, type_, "CellsTriggeredList-item");
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
  const uint16_t& pci_utra_fdd_r16() const
  {
    assert_choice_type(types::pci_utra_fdd_r16, type_, "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  uint16_t& set_pci();
  uint16_t& set_pci_eutra();
  uint16_t& set_pci_utra_fdd_r16();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// CellsTriggeredList ::= SEQUENCE (SIZE (1..32)) OF CellsTriggeredList-item
using cells_triggered_list_l = dyn_array<cells_triggered_list_item_c_>;

// PLMN-IdentityList-r16 ::= SEQUENCE (SIZE (1..12)) OF PLMN-Identity
using plmn_id_list_r16_l = dyn_array<plmn_id_s>;

// RelaysTriggeredList-r17 ::= SEQUENCE (SIZE (1..32)) OF BIT STRING (SIZE (24))
using relays_triggered_list_r17_l = bounded_array<fixed_bitstring<24>, 32>;

// VarConditionalReconfig ::= SEQUENCE
struct var_conditional_recfg_s {
  cond_recfg_to_add_mod_list_r16_l cond_recfg_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarConnEstFailReport-r16 ::= SEQUENCE
struct var_conn_est_fail_report_r16_s {
  conn_est_fail_report_r16_s conn_est_fail_report_r16;
  plmn_id_s                  plmn_id_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarConnEstFailReportList-r17 ::= SEQUENCE
struct var_conn_est_fail_report_list_r17_s {
  using conn_est_fail_report_list_r17_l_ = dyn_array<var_conn_est_fail_report_r16_s>;

  // member variables
  conn_est_fail_report_list_r17_l_ conn_est_fail_report_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarLogMeasConfig-r16-IEs ::= SEQUENCE
struct var_log_meas_cfg_r16_ies_s {
  struct report_type_c_ {
    struct types_opts {
      enum options { periodical, event_triggered, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

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
  bool                   area_cfg_r16_present         = false;
  bool                   sensor_name_list_r16_present = false;
  bool                   early_meas_ind_r17_present   = false;
  bool                   area_cfg_v1700_present       = false;
  area_cfg_r16_s         area_cfg_r16;
  bt_name_list_r16_l     bt_name_list_r16;
  wlan_name_list_r16_l   wlan_name_list_r16;
  sensor_name_list_r16_s sensor_name_list_r16;
  logging_dur_r16_e      logging_dur_r16;
  report_type_c_         report_type;
  area_cfg_v1700_s       area_cfg_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarLogMeasReport-r16 ::= SEQUENCE
struct var_log_meas_report_r16_s {
  fixed_bitstring<48>      absolute_time_info_r16;
  trace_ref_r16_s          trace_ref_r16;
  fixed_octstring<2>       trace_recording_session_ref_r16;
  fixed_octstring<1>       tce_id_r16;
  log_meas_info_list_r16_l log_meas_info_list_r16;
  plmn_id_list2_r16_l      plmn_id_list_r16;

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

// VarMeasConfigSL-r16 ::= SEQUENCE
struct var_meas_cfg_sl_r16_s {
  bool                     sl_quant_cfg_r16_present = false;
  sl_meas_id_list_r16_l    sl_meas_id_list_r16;
  sl_meas_obj_list_r16_l   sl_meas_obj_list_r16;
  sl_report_cfg_list_r16_l sl_report_cfg_list_r16;
  sl_quant_cfg_r16_s       sl_quant_cfg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasIdleConfig-r16 ::= SEQUENCE
struct var_meas_idle_cfg_r16_s {
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
  meas_idle_carrier_list_nr_r16_l_    meas_idle_carrier_list_nr_r16;
  meas_idle_carrier_list_eutra_r16_l_ meas_idle_carrier_list_eutra_r16;
  meas_idle_dur_r16_e_                meas_idle_dur_r16;
  validity_area_list_r16_l            validity_area_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasIdleReport-r16 ::= SEQUENCE
struct var_meas_idle_report_r16_s {
  bool                         meas_report_idle_nr_r16_present    = false;
  bool                         meas_report_idle_eutra_r16_present = false;
  meas_result_idle_nr_r16_s    meas_report_idle_nr_r16;
  meas_result_idle_eutra_r16_s meas_report_idle_eutra_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasReport ::= SEQUENCE
struct var_meas_report_s {
  bool                        cli_triggered_list_r16_present = false;
  uint8_t                     meas_id                        = 1;
  cells_triggered_list_l      cells_triggered_list;
  int64_t                     nof_reports_sent = 0;
  cli_triggered_list_r16_c    cli_triggered_list_r16;
  tx_pool_meas_list_r16_l     tx_pool_meas_to_add_mod_list_nr_r16;
  relays_triggered_list_r17_l relays_triggered_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasReportList ::= SEQUENCE (SIZE (1..64)) OF VarMeasReport
using var_meas_report_list_l = dyn_array<var_meas_report_s>;

// VarMeasReportSL-r16 ::= SEQUENCE
struct var_meas_report_sl_r16_s {
  using sl_freq_triggered_list_r16_l_ = bounded_array<uint32_t, 8>;

  // member variables
  uint8_t                       sl_meas_id_r16 = 1;
  sl_freq_triggered_list_r16_l_ sl_freq_triggered_list_r16;
  int64_t                       sl_nof_reports_sent_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasReportListSL-r16 ::= SEQUENCE (SIZE (1..64)) OF VarMeasReportSL-r16
using var_meas_report_list_sl_r16_l = dyn_array<var_meas_report_sl_r16_s>;

// VarMobilityHistoryReport-r17 ::= SEQUENCE
struct var_mob_history_report_r17_s {
  visited_cell_info_list_r16_l   visited_cell_info_list_r16;
  visited_pscell_info_list_r17_l visited_pscell_info_list_report_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarRA-Report-r16 ::= SEQUENCE
struct var_ra_report_r16_s {
  ra_report_list_r16_l ra_report_list_r16;
  plmn_id_list_r16_l   plmn_id_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarRLF-Report-r16 ::= SEQUENCE
struct var_rlf_report_r16_s {
  rlf_report_r16_c    rlf_report_r16;
  plmn_id_list2_r16_l plmn_id_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// VarSuccessHO-Report-r17-IEs ::= SEQUENCE
struct var_success_ho_report_r17_ies_s {
  success_ho_report_r17_s success_ho_report_r17;
  plmn_id_list2_r16_l     plmn_id_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1

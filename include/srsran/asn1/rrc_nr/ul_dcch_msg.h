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

// PDCCH-ConfigSIB1 ::= SEQUENCE
struct pdcch_cfg_sib1_s {
  uint8_t coreset_zero      = 0;
  uint8_t search_space_zero = 0;

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

// CAG-IdentityInfo-r16 ::= SEQUENCE
struct cag_id_info_r16_s {
  bool                manual_ca_gsel_allowed_r16_present = false;
  fixed_bitstring<32> cag_id_r16;

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

// NPN-IdentityInfoList-r16 ::= SEQUENCE (SIZE (1..12)) OF NPN-IdentityInfo-r16
using npn_id_info_list_r16_l = dyn_array<npn_id_info_r16_s>;

// PLMN-IdentityInfoList ::= SEQUENCE (SIZE (1..12)) OF PLMN-IdentityInfo
using plmn_id_info_list_l = dyn_array<plmn_id_info_s>;

// PLMN-Identity-EUTRA-5GC ::= CHOICE
struct plmn_id_eutra_5_gc_c {
  struct types_opts {
    enum options { plmn_id_eutra_5_gc, plmn_idx, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  plmn_id_eutra_5_gc_c() = default;
  plmn_id_eutra_5_gc_c(const plmn_id_eutra_5_gc_c& other);
  plmn_id_eutra_5_gc_c& operator=(const plmn_id_eutra_5_gc_c& other);
  ~plmn_id_eutra_5_gc_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  plmn_id_s& plmn_id_eutra_5_gc()
  {
    assert_choice_type(types::plmn_id_eutra_5_gc, type_, "PLMN-Identity-EUTRA-5GC");
    return c.get<plmn_id_s>();
  }
  uint8_t& plmn_idx()
  {
    assert_choice_type(types::plmn_idx, type_, "PLMN-Identity-EUTRA-5GC");
    return c.get<uint8_t>();
  }
  const plmn_id_s& plmn_id_eutra_5_gc() const
  {
    assert_choice_type(types::plmn_id_eutra_5_gc, type_, "PLMN-Identity-EUTRA-5GC");
    return c.get<plmn_id_s>();
  }
  const uint8_t& plmn_idx() const
  {
    assert_choice_type(types::plmn_idx, type_, "PLMN-Identity-EUTRA-5GC");
    return c.get<uint8_t>();
  }
  plmn_id_s& set_plmn_id_eutra_5_gc();
  uint8_t&   set_plmn_idx();

private:
  types                      type_;
  choice_buffer_t<plmn_id_s> c;

  void destroy_();
};

// CellIdentity-EUTRA-5GC ::= CHOICE
struct cell_id_eutra_5_gc_c {
  struct types_opts {
    enum options { cell_id_eutra, cell_id_idx, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  cell_id_eutra_5_gc_c() = default;
  cell_id_eutra_5_gc_c(const cell_id_eutra_5_gc_c& other);
  cell_id_eutra_5_gc_c& operator=(const cell_id_eutra_5_gc_c& other);
  ~cell_id_eutra_5_gc_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<28>& cell_id_eutra()
  {
    assert_choice_type(types::cell_id_eutra, type_, "CellIdentity-EUTRA-5GC");
    return c.get<fixed_bitstring<28>>();
  }
  uint8_t& cell_id_idx()
  {
    assert_choice_type(types::cell_id_idx, type_, "CellIdentity-EUTRA-5GC");
    return c.get<uint8_t>();
  }
  const fixed_bitstring<28>& cell_id_eutra() const
  {
    assert_choice_type(types::cell_id_eutra, type_, "CellIdentity-EUTRA-5GC");
    return c.get<fixed_bitstring<28>>();
  }
  const uint8_t& cell_id_idx() const
  {
    assert_choice_type(types::cell_id_idx, type_, "CellIdentity-EUTRA-5GC");
    return c.get<uint8_t>();
  }
  fixed_bitstring<28>& set_cell_id_eutra();
  uint8_t&             set_cell_id_idx();

private:
  types                                type_;
  choice_buffer_t<fixed_bitstring<28>> c;

  void destroy_();
};

// FrequencyComponent-r17 ::= ENUMERATED
struct freq_component_r17_opts {
  enum options { active_carrier, cfg_carrier, active_bwp, cfg_bwp, nulltype } value;

  const char* to_string() const;
};
using freq_component_r17_e = enumerated<freq_component_r17_opts>;

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

// PLMN-IdentityList-EUTRA-5GC ::= SEQUENCE (SIZE (1..12)) OF PLMN-Identity-EUTRA-5GC
using plmn_id_list_eutra_5_gc_l = dyn_array<plmn_id_eutra_5_gc_c>;

// PLMN-IdentityList-EUTRA-EPC ::= SEQUENCE (SIZE (1..12)) OF PLMN-Identity
using plmn_id_list_eutra_epc_l = dyn_array<plmn_id_s>;

// CellAccessRelatedInfo-EUTRA-5GC ::= SEQUENCE
struct cell_access_related_info_eutra_5_gc_s {
  bool                      ranac_5gc_present = false;
  plmn_id_list_eutra_5_gc_l plmn_id_list_eutra_5gc;
  fixed_bitstring<24>       tac_eutra_5gc;
  uint16_t                  ranac_5gc = 0;
  cell_id_eutra_5_gc_c      cell_id_eutra_5gc;

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

// DefaultDC-Location-r17 ::= CHOICE
struct default_dc_location_r17_c {
  struct types_opts {
    enum options { ul, dl, ul_and_dl, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  default_dc_location_r17_c() = default;
  default_dc_location_r17_c(const default_dc_location_r17_c& other);
  default_dc_location_r17_c& operator=(const default_dc_location_r17_c& other);
  ~default_dc_location_r17_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  freq_component_r17_e& ul()
  {
    assert_choice_type(types::ul, type_, "DefaultDC-Location-r17");
    return c.get<freq_component_r17_e>();
  }
  freq_component_r17_e& dl()
  {
    assert_choice_type(types::dl, type_, "DefaultDC-Location-r17");
    return c.get<freq_component_r17_e>();
  }
  freq_component_r17_e& ul_and_dl()
  {
    assert_choice_type(types::ul_and_dl, type_, "DefaultDC-Location-r17");
    return c.get<freq_component_r17_e>();
  }
  const freq_component_r17_e& ul() const
  {
    assert_choice_type(types::ul, type_, "DefaultDC-Location-r17");
    return c.get<freq_component_r17_e>();
  }
  const freq_component_r17_e& dl() const
  {
    assert_choice_type(types::dl, type_, "DefaultDC-Location-r17");
    return c.get<freq_component_r17_e>();
  }
  const freq_component_r17_e& ul_and_dl() const
  {
    assert_choice_type(types::ul_and_dl, type_, "DefaultDC-Location-r17");
    return c.get<freq_component_r17_e>();
  }
  freq_component_r17_e& set_ul();
  freq_component_r17_e& set_dl();
  freq_component_r17_e& set_ul_and_dl();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// MultiBandInfoListEUTRA ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..256)
using multi_band_info_list_eutra_l = bounded_array<uint16_t, 8>;

// OffsetValue-r17 ::= SEQUENCE
struct offset_value_r17_s {
  int16_t offset_value_r17   = -20000;
  bool    shift7dot5k_hz_r17 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// CC-Group-r17 ::= SEQUENCE
struct cc_group_r17_s {
  struct offset_to_default_r17_c_ {
    using offsetlist_l_ = dyn_array<offset_value_r17_s>;
    struct types_opts {
      enum options { offset_value, offsetlist, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    offset_to_default_r17_c_() = default;
    offset_to_default_r17_c_(const offset_to_default_r17_c_& other);
    offset_to_default_r17_c_& operator=(const offset_to_default_r17_c_& other);
    ~offset_to_default_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    offset_value_r17_s& offset_value()
    {
      assert_choice_type(types::offset_value, type_, "offsetToDefault-r17");
      return c.get<offset_value_r17_s>();
    }
    offsetlist_l_& offsetlist()
    {
      assert_choice_type(types::offsetlist, type_, "offsetToDefault-r17");
      return c.get<offsetlist_l_>();
    }
    const offset_value_r17_s& offset_value() const
    {
      assert_choice_type(types::offset_value, type_, "offsetToDefault-r17");
      return c.get<offset_value_r17_s>();
    }
    const offsetlist_l_& offsetlist() const
    {
      assert_choice_type(types::offsetlist, type_, "offsetToDefault-r17");
      return c.get<offsetlist_l_>();
    }
    offset_value_r17_s& set_offset_value();
    offsetlist_l_&      set_offsetlist();

  private:
    types                                              type_;
    choice_buffer_t<offset_value_r17_s, offsetlist_l_> c;

    void destroy_();
  };

  // member variables
  bool                      serv_cell_idx_higher_r17_present = false;
  bool                      offset_to_default_r17_present    = false;
  uint8_t                   serv_cell_idx_lower_r17          = 0;
  uint8_t                   serv_cell_idx_higher_r17         = 0;
  default_dc_location_r17_c default_dc_location_r17;
  offset_to_default_r17_c_  offset_to_default_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CGI-InfoEUTRA ::= SEQUENCE
struct cgi_info_eutra_s {
  struct cgi_info_epc_s_ {
    using cgi_info_epc_list_l_ = dyn_array<cell_access_related_info_eutra_epc_s>;

    // member variables
    cell_access_related_info_eutra_epc_s cgi_info_epc_legacy;
    cgi_info_epc_list_l_                 cgi_info_epc_list;
  };
  using cgi_info_5_gc_l_ = dyn_array<cell_access_related_info_eutra_5_gc_s>;

  // member variables
  bool                         cgi_info_epc_present       = false;
  bool                         freq_band_ind_prio_present = false;
  cgi_info_epc_s_              cgi_info_epc;
  cgi_info_5_gc_l_             cgi_info_5_gc;
  uint16_t                     freq_band_ind = 1;
  multi_band_info_list_eutra_l multi_band_info_list;

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
  // group 0
  copy_ptr<npn_id_info_list_r16_l> npn_id_info_list_r16;
  // group 1
  bool cell_reserved_for_other_use_r16_present = false;

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

// NeedForNCSG-IntraFreq-r17 ::= SEQUENCE
struct need_for_ncsg_intra_freq_r17_s {
  struct gap_ind_intra_r17_opts {
    enum options { gap, ncsg, nogap_noncsg, nulltype } value;

    const char* to_string() const;
  };
  using gap_ind_intra_r17_e_ = enumerated<gap_ind_intra_r17_opts>;

  // member variables
  uint8_t              serv_cell_id_r17 = 0;
  gap_ind_intra_r17_e_ gap_ind_intra_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeedForNCSG-NR-r17 ::= SEQUENCE
struct need_for_ncsg_nr_r17_s {
  struct gap_ind_r17_opts {
    enum options { gap, ncsg, nogap_noncsg, nulltype } value;

    const char* to_string() const;
  };
  using gap_ind_r17_e_ = enumerated<gap_ind_r17_opts>;

  // member variables
  uint16_t       band_nr_r17 = 1;
  gap_ind_r17_e_ gap_ind_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PerRAAttemptInfo-r16 ::= SEQUENCE
struct per_ra_attempt_info_r16_s {
  bool ext                             = false;
  bool contention_detected_r16_present = false;
  bool dl_rsrp_above_thres_r16_present = false;
  bool contention_detected_r16         = false;
  bool dl_rsrp_above_thres_r16         = false;
  // ...
  // group 0
  bool fallback_to_four_step_ra_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultsPerCSI-RS-IndexList ::= SEQUENCE (SIZE (1..64)) OF ResultsPerCSI-RS-Index
using results_per_csi_rs_idx_list_l = dyn_array<results_per_csi_rs_idx_s>;

// ResultsPerSSB-IndexList ::= SEQUENCE (SIZE (1..64)) OF ResultsPerSSB-Index
using results_per_ssb_idx_list_l = dyn_array<results_per_ssb_idx_s>;

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

// MeasResultLoggingNR-r16 ::= SEQUENCE
struct meas_result_logging_nr_r16_s {
  bool                 nof_good_ssb_r16_present = false;
  uint16_t             pci_r16                  = 0;
  meas_quant_results_s results_ssb_cell_r16;
  uint8_t              nof_good_ssb_r16 = 1;

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
  using cho_cfg_r17_l_ = dyn_array<cond_trigger_cfg_r16_s>;
  struct triggered_event_r17_s_ {
    struct first_triggered_event_opts {
      enum options { cond_first_event, cond_second_event, nulltype } value;

      const char* to_string() const;
    };
    using first_triggered_event_e_ = enumerated<first_triggered_event_opts>;

    // member variables
    bool                     time_between_events_r17_present = false;
    bool                     first_triggered_event_present   = false;
    uint16_t                 time_between_events_r17         = 0;
    first_triggered_event_e_ first_triggered_event;
  };

  // member variables
  bool           ext         = false;
  bool           pci_present = false;
  uint16_t       pci         = 0;
  meas_result_s_ meas_result;
  // ...
  // group 0
  copy_ptr<cgi_info_nr_s> cgi_info;
  // group 1
  bool                             cho_candidate_r17_present = false;
  copy_ptr<cho_cfg_r17_l_>         cho_cfg_r17;
  copy_ptr<triggered_event_r17_s_> triggered_event_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeedForNCSG-BandListNR-r17 ::= SEQUENCE (SIZE (1..1024)) OF NeedForNCSG-NR-r17
using need_for_ncsg_band_list_nr_r17_l = dyn_array<need_for_ncsg_nr_r17_s>;

// NeedForNCSG-EUTRA-r17 ::= SEQUENCE
struct need_for_ncsg_eutra_r17_s {
  struct gap_ind_r17_opts {
    enum options { gap, ncsg, nogap_noncsg, nulltype } value;

    const char* to_string() const;
  };
  using gap_ind_r17_e_ = enumerated<gap_ind_r17_opts>;

  // member variables
  uint16_t       band_eutra_r17 = 1;
  gap_ind_r17_e_ gap_ind_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeedForNCSG-IntraFreqList-r17 ::= SEQUENCE (SIZE (1..32)) OF NeedForNCSG-IntraFreq-r17
using need_for_ncsg_intra_freq_list_r17_l = dyn_array<need_for_ncsg_intra_freq_r17_s>;

// PerRAAttemptInfoList-r16 ::= SEQUENCE (SIZE (1..200)) OF PerRAAttemptInfo-r16
using per_ra_attempt_info_list_r16_l = dyn_array<per_ra_attempt_info_r16_s>;

// SL-QoS-Info-r16 ::= SEQUENCE
struct sl_qos_info_r16_s {
  bool                 sl_qos_profile_r16_present = false;
  uint16_t             sl_qos_flow_id_r16         = 1;
  sl_qos_profile_r16_s sl_qos_profile_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkTxDirectCurrentCarrierInfo-r16 ::= SEQUENCE
struct ul_tx_direct_current_carrier_info_r16_s {
  struct serv_cell_info_r16_c_ {
    struct types_opts {
      enum options { bwp_id_r16, deactiv_carrier_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    serv_cell_info_r16_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& bwp_id_r16()
    {
      assert_choice_type(types::bwp_id_r16, type_, "servCellInfo-r16");
      return c;
    }
    const uint8_t& bwp_id_r16() const
    {
      assert_choice_type(types::bwp_id_r16, type_, "servCellInfo-r16");
      return c;
    }
    uint8_t& set_bwp_id_r16();
    void     set_deactiv_carrier_r16();

  private:
    types   type_;
    uint8_t c;
  };

  // member variables
  uint8_t               serv_cell_idx_r16 = 0;
  serv_cell_info_r16_c_ serv_cell_info_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkTxDirectCurrentMoreCarrierList-r17 ::= SEQUENCE (SIZE (1..16)) OF CC-Group-r17
using ul_tx_direct_current_more_carrier_list_r17_l = dyn_array<cc_group_r17_s>;

// UplinkTxDirectCurrentTwoCarrierInfo-r16 ::= SEQUENCE
struct ul_tx_direct_current_two_carrier_info_r16_s {
  uint8_t  ref_carrier_idx_r16            = 0;
  bool     shift7dot5k_hz_r16             = false;
  uint16_t tx_direct_current_location_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WLAN-Identifiers-r16 ::= SEQUENCE
struct wlan_ids_r16_s {
  bool                     ext                = false;
  bool                     bssid_r16_present  = false;
  bool                     hessid_r16_present = false;
  bounded_octstring<1, 32> ssid_r16;
  fixed_octstring<6>       bssid_r16;
  fixed_octstring<6>       hessid_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WLAN-RTT-r16 ::= SEQUENCE
struct wlan_rtt_r16_s {
  struct rtt_units_r16_opts {
    enum options {
      microseconds,
      hundredsofnanoseconds,
      tensofnanoseconds,
      nanoseconds,
      tenthsofnanoseconds,
      /*...*/ nulltype
    } value;

    const char* to_string() const;
  };
  using rtt_units_r16_e_ = enumerated<rtt_units_r16_opts, true>;

  // member variables
  bool             ext                      = false;
  bool             rtt_accuracy_r16_present = false;
  uint32_t         rtt_value_r16            = 0;
  rtt_units_r16_e_ rtt_units_r16;
  uint16_t         rtt_accuracy_r16 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CGI-InfoEUTRALogging ::= SEQUENCE
struct cgi_info_eutra_logging_s {
  bool                plmn_id_eutra_5gc_present = false;
  bool                tac_eutra_5gc_present     = false;
  bool                cell_id_eutra_5gc_present = false;
  bool                plmn_id_eutra_epc_present = false;
  bool                tac_eutra_epc_present     = false;
  bool                cell_id_eutra_epc_present = false;
  plmn_id_s           plmn_id_eutra_5gc;
  fixed_bitstring<24> tac_eutra_5gc;
  fixed_bitstring<28> cell_id_eutra_5gc;
  plmn_id_s           plmn_id_eutra_epc;
  fixed_bitstring<16> tac_eutra_epc;
  fixed_bitstring<28> cell_id_eutra_epc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LogMeasResultBT-r16 ::= SEQUENCE
struct log_meas_result_bt_r16_s {
  bool                ext                 = false;
  bool                rssi_bt_r16_present = false;
  fixed_bitstring<48> bt_addr_r16;
  int16_t             rssi_bt_r16 = -128;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LogMeasResultWLAN-r16 ::= SEQUENCE
struct log_meas_result_wlan_r16_s {
  bool           ext                   = false;
  bool           rssi_wlan_r16_present = false;
  bool           rtt_wlan_r16_present  = false;
  wlan_ids_r16_s wlan_ids_r16;
  uint8_t        rssi_wlan_r16 = 0;
  wlan_rtt_r16_s rtt_wlan_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultListEUTRA ::= SEQUENCE (SIZE (1..8)) OF MeasResultEUTRA
using meas_result_list_eutra_l = dyn_array<meas_result_eutra_s>;

// MeasResultListLoggingNR-r16 ::= SEQUENCE (SIZE (1..8)) OF MeasResultLoggingNR-r16
using meas_result_list_logging_nr_r16_l = dyn_array<meas_result_logging_nr_r16_s>;

// MeasResultListNR ::= SEQUENCE (SIZE (1..8)) OF MeasResultNR
using meas_result_list_nr_l = dyn_array<meas_result_nr_s>;

// NeedForGapNCSG-InfoEUTRA-r17 ::= SEQUENCE
struct need_for_gap_ncsg_info_eutra_r17_s {
  using need_for_ncsg_eutra_r17_l_ = dyn_array<need_for_ncsg_eutra_r17_s>;

  // member variables
  need_for_ncsg_eutra_r17_l_ need_for_ncsg_eutra_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeedForGapNCSG-InfoNR-r17 ::= SEQUENCE
struct need_for_gap_ncsg_info_nr_r17_s {
  need_for_ncsg_intra_freq_list_r17_l intra_freq_need_for_ncsg_r17;
  need_for_ncsg_band_list_nr_r17_l    inter_freq_need_for_ncsg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeedForGapsIntraFreq-r16 ::= SEQUENCE
struct need_for_gaps_intra_freq_r16_s {
  struct gap_ind_intra_r16_opts {
    enum options { gap, no_gap, nulltype } value;

    const char* to_string() const;
  };
  using gap_ind_intra_r16_e_ = enumerated<gap_ind_intra_r16_opts>;

  // member variables
  uint8_t              serv_cell_id_r16 = 0;
  gap_ind_intra_r16_e_ gap_ind_intra_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeedForGapsNR-r16 ::= SEQUENCE
struct need_for_gaps_nr_r16_s {
  struct gap_ind_r16_opts {
    enum options { gap, no_gap, nulltype } value;

    const char* to_string() const;
  };
  using gap_ind_r16_e_ = enumerated<gap_ind_r16_opts>;

  // member variables
  uint16_t       band_nr_r16 = 1;
  gap_ind_r16_e_ gap_ind_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCI-ARFCN-EUTRA-r16 ::= SEQUENCE
struct pci_arfcn_eutra_r16_s {
  uint16_t pci_r16          = 0;
  uint32_t carrier_freq_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCI-ARFCN-NR-r16 ::= SEQUENCE
struct pci_arfcn_nr_r16_s {
  uint16_t pci_r16          = 0;
  uint32_t carrier_freq_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PerRACSI-RSInfo-r16 ::= SEQUENCE
struct per_ra_csi_r_si_nfo_r16_s {
  uint8_t csi_rs_idx_r16                 = 0;
  uint8_t nof_preambs_sent_on_csi_rs_r16 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PerRASSBInfo-r16 ::= SEQUENCE
struct per_ra_ssb_info_r16_s {
  uint8_t                        ssb_idx_r16                 = 0;
  uint8_t                        nof_preambs_sent_on_ssb_r16 = 1;
  per_ra_attempt_info_list_r16_l per_ra_attempt_info_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationComplete-v1720-IEs ::= SEQUENCE
struct rrc_recfg_complete_v1720_ies_s {
  bool                                         non_crit_ext_present = false;
  ul_tx_direct_current_more_carrier_list_r17_l ul_tx_direct_current_more_carrier_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultsPerSSB-IndexIdle-r16 ::= SEQUENCE
struct results_per_ssb_idx_idle_r16_s {
  struct ssb_results_r16_s_ {
    bool    ssb_rsrp_result_r16_present = false;
    bool    ssb_rsrq_result_r16_present = false;
    uint8_t ssb_rsrp_result_r16         = 0;
    uint8_t ssb_rsrq_result_r16         = 0;
  };

  // member variables
  bool               ssb_results_r16_present = false;
  uint8_t            ssb_idx_r16             = 0;
  ssb_results_r16_s_ ssb_results_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PagingIdentityRemoteUE-r17 ::= SEQUENCE
struct sl_paging_id_remote_ue_r17_s {
  bool                full_i_rnti_r17_present = false;
  fixed_bitstring<48> ng_5_g_s_tmsi_r17;
  fixed_bitstring<40> full_i_rnti_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-RLC-ModeIndication-r16 ::= SEQUENCE
struct sl_rlc_mode_ind_r16_s {
  struct sl_mode_r16_c_ {
    struct types_opts {
      enum options { sl_am_mode_r16, sl_um_mode_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    sl_mode_r16_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    void set_sl_am_mode_r16();
    void set_sl_um_mode_r16();

  private:
    types type_;
  };
  using sl_qos_info_list_r16_l_ = dyn_array<sl_qos_info_r16_s>;

  // member variables
  sl_mode_r16_c_          sl_mode_r16;
  sl_qos_info_list_r16_l_ sl_qos_info_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TxInterestedFreqList-r16 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..8)
using sl_tx_interested_freq_list_r16_l = bounded_array<uint8_t, 8>;

// UplinkTxDirectCurrentTwoCarrier-r16 ::= SEQUENCE
struct ul_tx_direct_current_two_carrier_r16_s {
  bool                                        second_pa_tx_direct_current_r16_present = false;
  ul_tx_direct_current_carrier_info_r16_s     carrier_one_info_r16;
  ul_tx_direct_current_carrier_info_r16_s     carrier_two_info_r16;
  ul_tx_direct_current_two_carrier_info_r16_s single_pa_tx_direct_current_r16;
  ul_tx_direct_current_two_carrier_info_r16_s second_pa_tx_direct_current_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VictimSystemType-r16 ::= SEQUENCE
struct victim_sys_type_r16_s {
  bool ext                   = false;
  bool gps_r16_present       = false;
  bool glonass_r16_present   = false;
  bool bds_r16_present       = false;
  bool galileo_r16_present   = false;
  bool nav_ic_r16_present    = false;
  bool wlan_r16_present      = false;
  bool bluetooth_r16_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AffectedCarrierFreq-r16 ::= SEQUENCE
struct affected_carrier_freq_r16_s {
  struct interference_direction_r16_opts {
    enum options { nr, other, both, spare, nulltype } value;

    const char* to_string() const;
  };
  using interference_direction_r16_e_ = enumerated<interference_direction_r16_opts>;

  // member variables
  uint32_t                      carrier_freq_r16 = 0;
  interference_direction_r16_e_ interference_direction_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AffectedCarrierFreqComb-r16 ::= SEQUENCE
struct affected_carrier_freq_comb_r16_s {
  using affected_carrier_freq_comb_r16_l_ = bounded_array<uint32_t, 32>;

  // member variables
  affected_carrier_freq_comb_r16_l_ affected_carrier_freq_comb_r16;
  victim_sys_type_r16_s             victim_sys_type_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CommonLocationInfo-r16 ::= SEQUENCE
struct common_location_info_r16_s {
  dyn_octstring gnss_tod_msec_r16;
  dyn_octstring location_timestamp_r16;
  dyn_octstring location_coordinate_r16;
  dyn_octstring location_error_r16;
  dyn_octstring location_source_r16;
  dyn_octstring velocity_estimate_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LogMeasResultListBT-r16 ::= SEQUENCE (SIZE (1..32)) OF LogMeasResultBT-r16
using log_meas_result_list_bt_r16_l = dyn_array<log_meas_result_bt_r16_s>;

// LogMeasResultListWLAN-r16 ::= SEQUENCE (SIZE (1..32)) OF LogMeasResultWLAN-r16
using log_meas_result_list_wlan_r16_l = dyn_array<log_meas_result_wlan_r16_s>;

// MUSIM-GapPreferenceList-r17 ::= SEQUENCE (SIZE (1..4)) OF MUSIM-GapInfo-r17
using mu_si_m_gap_pref_list_r17_l = dyn_array<mu_si_m_gap_info_r17_s>;

// MeasResult2EUTRA-r16 ::= SEQUENCE
struct meas_result2_eutra_r16_s {
  uint32_t                 carrier_freq_r16 = 0;
  meas_result_list_eutra_l meas_result_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResult2NR-r16 ::= SEQUENCE
struct meas_result2_nr_r16_s {
  bool                  ssb_freq_r16_present        = false;
  bool                  ref_freq_csi_rs_r16_present = false;
  uint32_t              ssb_freq_r16                = 0;
  uint32_t              ref_freq_csi_rs_r16         = 0;
  meas_result_list_nr_l meas_result_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultLogging2NR-r16 ::= SEQUENCE
struct meas_result_logging2_nr_r16_s {
  uint32_t                          carrier_freq_r16 = 0;
  meas_result_list_logging_nr_r16_l meas_result_list_logging_nr_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultUTRA-FDD-r16 ::= SEQUENCE
struct meas_result_utra_fdd_r16_s {
  struct meas_result_r16_s_ {
    bool    utra_fdd_rs_cp_r16_present = false;
    bool    utra_fdd_ec_n0_r16_present = false;
    int8_t  utra_fdd_rs_cp_r16         = -5;
    uint8_t utra_fdd_ec_n0_r16         = 0;
  };

  // member variables
  uint16_t           pci_r16 = 0;
  meas_result_r16_s_ meas_result_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeedForGapsBandListNR-r16 ::= SEQUENCE (SIZE (1..1024)) OF NeedForGapsNR-r16
using need_for_gaps_band_list_nr_r16_l = dyn_array<need_for_gaps_nr_r16_s>;

// NeedForGapsIntraFreqList-r16 ::= SEQUENCE (SIZE (1..32)) OF NeedForGapsIntraFreq-r16
using need_for_gaps_intra_freq_list_r16_l = dyn_array<need_for_gaps_intra_freq_r16_s>;

// PerRACSI-RSInfo-v1660 ::= SEQUENCE
struct per_ra_csi_r_si_nfo_v1660_s {
  bool    csi_rs_idx_v1660_present = false;
  uint8_t csi_rs_idx_v1660         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PerRAInfo-r16 ::= CHOICE
struct per_ra_info_r16_c {
  struct types_opts {
    enum options { per_ra_ssb_info_list_r16, per_ra_csi_r_si_nfo_list_r16, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  per_ra_info_r16_c() = default;
  per_ra_info_r16_c(const per_ra_info_r16_c& other);
  per_ra_info_r16_c& operator=(const per_ra_info_r16_c& other);
  ~per_ra_info_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  per_ra_ssb_info_r16_s& per_ra_ssb_info_list_r16()
  {
    assert_choice_type(types::per_ra_ssb_info_list_r16, type_, "PerRAInfo-r16");
    return c.get<per_ra_ssb_info_r16_s>();
  }
  per_ra_csi_r_si_nfo_r16_s& per_ra_csi_r_si_nfo_list_r16()
  {
    assert_choice_type(types::per_ra_csi_r_si_nfo_list_r16, type_, "PerRAInfo-r16");
    return c.get<per_ra_csi_r_si_nfo_r16_s>();
  }
  const per_ra_ssb_info_r16_s& per_ra_ssb_info_list_r16() const
  {
    assert_choice_type(types::per_ra_ssb_info_list_r16, type_, "PerRAInfo-r16");
    return c.get<per_ra_ssb_info_r16_s>();
  }
  const per_ra_csi_r_si_nfo_r16_s& per_ra_csi_r_si_nfo_list_r16() const
  {
    assert_choice_type(types::per_ra_csi_r_si_nfo_list_r16, type_, "PerRAInfo-r16");
    return c.get<per_ra_csi_r_si_nfo_r16_s>();
  }
  per_ra_ssb_info_r16_s&     set_per_ra_ssb_info_list_r16();
  per_ra_csi_r_si_nfo_r16_s& set_per_ra_csi_r_si_nfo_list_r16();

private:
  types                                                             type_;
  choice_buffer_t<per_ra_csi_r_si_nfo_r16_s, per_ra_ssb_info_r16_s> c;

  void destroy_();
};

// RRCReconfigurationComplete-v1700-IEs ::= SEQUENCE
struct rrc_recfg_complete_v1700_ies_s {
  bool                               need_for_gap_ncsg_info_nr_r17_present    = false;
  bool                               need_for_gap_ncsg_info_eutra_r17_present = false;
  bool                               sel_cond_rrc_recfg_r17_present           = false;
  bool                               non_crit_ext_present                     = false;
  need_for_gap_ncsg_info_nr_r17_s    need_for_gap_ncsg_info_nr_r17;
  need_for_gap_ncsg_info_eutra_r17_s need_for_gap_ncsg_info_eutra_r17;
  uint8_t                            sel_cond_rrc_recfg_r17 = 1;
  rrc_recfg_complete_v1720_ies_s     non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReducedAggregatedBandwidth ::= ENUMERATED
struct reduced_aggr_bw_opts {
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
using reduced_aggr_bw_e = enumerated<reduced_aggr_bw_opts>;

// ReducedAggregatedBandwidth-r17 ::= ENUMERATED
struct reduced_aggr_bw_r17_opts {
  enum options { mhz0, mhz100, mhz200, mhz400, mhz800, mhz1200, mhz1600, mhz2000, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using reduced_aggr_bw_r17_e = enumerated<reduced_aggr_bw_r17_opts>;

// ResultsPerSSB-IndexList-r16 ::= SEQUENCE (SIZE (1..32)) OF ResultsPerSSB-IndexIdle-r16
using results_per_ssb_idx_list_r16_l = dyn_array<results_per_ssb_idx_idle_r16_s>;

// SL-DRX-ConfigUC-SemiStatic-r17 ::= SEQUENCE
struct sl_drx_cfg_uc_semi_static_r17_s {
  struct sl_drx_on_dur_timer_r17_c_ {
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
    sl_drx_on_dur_timer_r17_c_() = default;
    sl_drx_on_dur_timer_r17_c_(const sl_drx_on_dur_timer_r17_c_& other);
    sl_drx_on_dur_timer_r17_c_& operator=(const sl_drx_on_dur_timer_r17_c_& other);
    ~sl_drx_on_dur_timer_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sub_milli_seconds()
    {
      assert_choice_type(types::sub_milli_seconds, type_, "sl-drx-onDurationTimer-r17");
      return c.get<uint8_t>();
    }
    milli_seconds_e_& milli_seconds()
    {
      assert_choice_type(types::milli_seconds, type_, "sl-drx-onDurationTimer-r17");
      return c.get<milli_seconds_e_>();
    }
    const uint8_t& sub_milli_seconds() const
    {
      assert_choice_type(types::sub_milli_seconds, type_, "sl-drx-onDurationTimer-r17");
      return c.get<uint8_t>();
    }
    const milli_seconds_e_& milli_seconds() const
    {
      assert_choice_type(types::milli_seconds, type_, "sl-drx-onDurationTimer-r17");
      return c.get<milli_seconds_e_>();
    }
    uint8_t&          set_sub_milli_seconds();
    milli_seconds_e_& set_milli_seconds();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct sl_drx_cycle_start_offset_r17_c_ {
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
    sl_drx_cycle_start_offset_r17_c_() = default;
    sl_drx_cycle_start_offset_r17_c_(const sl_drx_cycle_start_offset_r17_c_& other);
    sl_drx_cycle_start_offset_r17_c_& operator=(const sl_drx_cycle_start_offset_r17_c_& other);
    ~sl_drx_cycle_start_offset_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ms10()
    {
      assert_choice_type(types::ms10, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms20()
    {
      assert_choice_type(types::ms20, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms32()
    {
      assert_choice_type(types::ms32, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms40()
    {
      assert_choice_type(types::ms40, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms60()
    {
      assert_choice_type(types::ms60, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms64()
    {
      assert_choice_type(types::ms64, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms70()
    {
      assert_choice_type(types::ms70, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms80()
    {
      assert_choice_type(types::ms80, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms128()
    {
      assert_choice_type(types::ms128, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms160()
    {
      assert_choice_type(types::ms160, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    uint16_t& ms256()
    {
      assert_choice_type(types::ms256, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms320()
    {
      assert_choice_type(types::ms320, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms512()
    {
      assert_choice_type(types::ms512, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms640()
    {
      assert_choice_type(types::ms640, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms1024()
    {
      assert_choice_type(types::ms1024, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms1280()
    {
      assert_choice_type(types::ms1280, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms2048()
    {
      assert_choice_type(types::ms2048, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms2560()
    {
      assert_choice_type(types::ms2560, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms5120()
    {
      assert_choice_type(types::ms5120, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms10240()
    {
      assert_choice_type(types::ms10240, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    const uint8_t& ms10() const
    {
      assert_choice_type(types::ms10, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms20() const
    {
      assert_choice_type(types::ms20, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms32() const
    {
      assert_choice_type(types::ms32, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms40() const
    {
      assert_choice_type(types::ms40, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms60() const
    {
      assert_choice_type(types::ms60, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms64() const
    {
      assert_choice_type(types::ms64, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms70() const
    {
      assert_choice_type(types::ms70, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms80() const
    {
      assert_choice_type(types::ms80, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms128() const
    {
      assert_choice_type(types::ms128, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms160() const
    {
      assert_choice_type(types::ms160, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint8_t>();
    }
    const uint16_t& ms256() const
    {
      assert_choice_type(types::ms256, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms320() const
    {
      assert_choice_type(types::ms320, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms512() const
    {
      assert_choice_type(types::ms512, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms640() const
    {
      assert_choice_type(types::ms640, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms1024() const
    {
      assert_choice_type(types::ms1024, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms1280() const
    {
      assert_choice_type(types::ms1280, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms2048() const
    {
      assert_choice_type(types::ms2048, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms2560() const
    {
      assert_choice_type(types::ms2560, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms5120() const
    {
      assert_choice_type(types::ms5120, type_, "sl-drx-CycleStartOffset-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms10240() const
    {
      assert_choice_type(types::ms10240, type_, "sl-drx-CycleStartOffset-r17");
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
  sl_drx_on_dur_timer_r17_c_       sl_drx_on_dur_timer_r17;
  sl_drx_cycle_start_offset_r17_c_ sl_drx_cycle_start_offset_r17;
  uint8_t                          sl_drx_slot_offset_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TxResourceReq-r16 ::= SEQUENCE
struct sl_tx_res_req_r16_s {
  struct sl_cast_type_r16_opts {
    enum options { broadcast, groupcast, unicast, spare1, nulltype } value;

    const char* to_string() const;
  };
  using sl_cast_type_r16_e_         = enumerated<sl_cast_type_r16_opts>;
  using sl_rlc_mode_ind_list_r16_l_ = dyn_array<sl_rlc_mode_ind_r16_s>;
  using sl_qos_info_list_r16_l_     = dyn_array<sl_qos_info_r16_s>;
  using sl_type_tx_sync_list_r16_l_ = bounded_array<sl_type_tx_sync_r16_e, 8>;

  // member variables
  fixed_bitstring<24>              sl_dest_id_r16;
  sl_cast_type_r16_e_              sl_cast_type_r16;
  sl_rlc_mode_ind_list_r16_l_      sl_rlc_mode_ind_list_r16;
  sl_qos_info_list_r16_l_          sl_qos_info_list_r16;
  sl_type_tx_sync_list_r16_l_      sl_type_tx_sync_list_r16;
  sl_tx_interested_freq_list_r16_l sl_tx_interested_freq_list_r16;
  dyn_octstring                    sl_cap_info_sidelink_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TxResourceReqL2U2N-Relay-r17 ::= SEQUENCE
struct sl_tx_res_req_l2_u2_n_relay_r17_s {
  using sl_type_tx_sync_list_l2_u2_n_r17_l_ = bounded_array<sl_type_tx_sync_r16_e, 8>;

  // member variables
  bool                                ext                                = false;
  bool                                sl_dest_id_l2_u2_n_r17_present     = false;
  bool                                sl_local_id_request_r17_present    = false;
  bool                                sl_paging_id_remote_ue_r17_present = false;
  fixed_bitstring<24>                 sl_dest_id_l2_u2_n_r17;
  sl_tx_interested_freq_list_r16_l    sl_tx_interested_freq_list_l2_u2_n_r17;
  sl_type_tx_sync_list_l2_u2_n_r17_l_ sl_type_tx_sync_list_l2_u2_n_r17;
  sl_paging_id_remote_ue_r17_s        sl_paging_id_remote_ue_r17;
  dyn_octstring                       sl_cap_info_sidelink_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Sensor-LocationInfo-r16 ::= SEQUENCE
struct sensor_location_info_r16_s {
  bool          ext = false;
  dyn_octstring sensor_meas_info_r16;
  dyn_octstring sensor_motion_info_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkTxDirectCurrentTwoCarrierList-r16 ::= SEQUENCE (SIZE (1..64)) OF UplinkTxDirectCurrentTwoCarrier-r16
using ul_tx_direct_current_two_carrier_list_r16_l = dyn_array<ul_tx_direct_current_two_carrier_r16_s>;

// VisitedPSCellInfo-r17 ::= SEQUENCE
struct visited_pscell_info_r17_s {
  struct visited_cell_id_r17_c_ {
    struct nr_cell_id_r17_c_ {
      struct types_opts {
        enum options { cgi_info_r17, pci_arfcn_r17, nulltype } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      nr_cell_id_r17_c_() = default;
      nr_cell_id_r17_c_(const nr_cell_id_r17_c_& other);
      nr_cell_id_r17_c_& operator=(const nr_cell_id_r17_c_& other);
      ~nr_cell_id_r17_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      cgi_info_logging_r16_s& cgi_info_r17()
      {
        assert_choice_type(types::cgi_info_r17, type_, "nr-CellId-r17");
        return c.get<cgi_info_logging_r16_s>();
      }
      pci_arfcn_nr_r16_s& pci_arfcn_r17()
      {
        assert_choice_type(types::pci_arfcn_r17, type_, "nr-CellId-r17");
        return c.get<pci_arfcn_nr_r16_s>();
      }
      const cgi_info_logging_r16_s& cgi_info_r17() const
      {
        assert_choice_type(types::cgi_info_r17, type_, "nr-CellId-r17");
        return c.get<cgi_info_logging_r16_s>();
      }
      const pci_arfcn_nr_r16_s& pci_arfcn_r17() const
      {
        assert_choice_type(types::pci_arfcn_r17, type_, "nr-CellId-r17");
        return c.get<pci_arfcn_nr_r16_s>();
      }
      cgi_info_logging_r16_s& set_cgi_info_r17();
      pci_arfcn_nr_r16_s&     set_pci_arfcn_r17();

    private:
      types                                                       type_;
      choice_buffer_t<cgi_info_logging_r16_s, pci_arfcn_nr_r16_s> c;

      void destroy_();
    };
    struct eutra_cell_id_r17_c_ {
      struct types_opts {
        enum options { cell_global_id_r17, pci_arfcn_r17, nulltype } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      eutra_cell_id_r17_c_() = default;
      eutra_cell_id_r17_c_(const eutra_cell_id_r17_c_& other);
      eutra_cell_id_r17_c_& operator=(const eutra_cell_id_r17_c_& other);
      ~eutra_cell_id_r17_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      cgi_info_eutra_logging_s& cell_global_id_r17()
      {
        assert_choice_type(types::cell_global_id_r17, type_, "eutra-CellId-r17");
        return c.get<cgi_info_eutra_logging_s>();
      }
      pci_arfcn_eutra_r16_s& pci_arfcn_r17()
      {
        assert_choice_type(types::pci_arfcn_r17, type_, "eutra-CellId-r17");
        return c.get<pci_arfcn_eutra_r16_s>();
      }
      const cgi_info_eutra_logging_s& cell_global_id_r17() const
      {
        assert_choice_type(types::cell_global_id_r17, type_, "eutra-CellId-r17");
        return c.get<cgi_info_eutra_logging_s>();
      }
      const pci_arfcn_eutra_r16_s& pci_arfcn_r17() const
      {
        assert_choice_type(types::pci_arfcn_r17, type_, "eutra-CellId-r17");
        return c.get<pci_arfcn_eutra_r16_s>();
      }
      cgi_info_eutra_logging_s& set_cell_global_id_r17();
      pci_arfcn_eutra_r16_s&    set_pci_arfcn_r17();

    private:
      types                                                            type_;
      choice_buffer_t<cgi_info_eutra_logging_s, pci_arfcn_eutra_r16_s> c;

      void destroy_();
    };
    struct types_opts {
      enum options { nr_cell_id_r17, eutra_cell_id_r17, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    visited_cell_id_r17_c_() = default;
    visited_cell_id_r17_c_(const visited_cell_id_r17_c_& other);
    visited_cell_id_r17_c_& operator=(const visited_cell_id_r17_c_& other);
    ~visited_cell_id_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nr_cell_id_r17_c_& nr_cell_id_r17()
    {
      assert_choice_type(types::nr_cell_id_r17, type_, "visitedCellId-r17");
      return c.get<nr_cell_id_r17_c_>();
    }
    eutra_cell_id_r17_c_& eutra_cell_id_r17()
    {
      assert_choice_type(types::eutra_cell_id_r17, type_, "visitedCellId-r17");
      return c.get<eutra_cell_id_r17_c_>();
    }
    const nr_cell_id_r17_c_& nr_cell_id_r17() const
    {
      assert_choice_type(types::nr_cell_id_r17, type_, "visitedCellId-r17");
      return c.get<nr_cell_id_r17_c_>();
    }
    const eutra_cell_id_r17_c_& eutra_cell_id_r17() const
    {
      assert_choice_type(types::eutra_cell_id_r17, type_, "visitedCellId-r17");
      return c.get<eutra_cell_id_r17_c_>();
    }
    nr_cell_id_r17_c_&    set_nr_cell_id_r17();
    eutra_cell_id_r17_c_& set_eutra_cell_id_r17();

  private:
    types                                                    type_;
    choice_buffer_t<eutra_cell_id_r17_c_, nr_cell_id_r17_c_> c;

    void destroy_();
  };

  // member variables
  bool                   ext                         = false;
  bool                   visited_cell_id_r17_present = false;
  visited_cell_id_r17_c_ visited_cell_id_r17;
  uint16_t               time_spent_r17 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AffectedCarrierFreqCombList-r16 ::= SEQUENCE (SIZE (1..128)) OF AffectedCarrierFreqComb-r16
using affected_carrier_freq_comb_list_r16_l = dyn_array<affected_carrier_freq_comb_r16_s>;

// AffectedCarrierFreqList-r16 ::= SEQUENCE (SIZE (1..128)) OF AffectedCarrierFreq-r16
using affected_carrier_freq_list_r16_l = dyn_array<affected_carrier_freq_r16_s>;

// LocationInfo-r16 ::= SEQUENCE
struct location_info_r16_s {
  bool                            ext                              = false;
  bool                            common_location_info_r16_present = false;
  bool                            sensor_location_info_r16_present = false;
  common_location_info_r16_s      common_location_info_r16;
  log_meas_result_list_bt_r16_l   bt_location_info_r16;
  log_meas_result_list_wlan_r16_l wlan_location_info_r16;
  sensor_location_info_r16_s      sensor_location_info_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MUSIM-Assistance-r17 ::= SEQUENCE
struct mu_si_m_assist_r17_s {
  struct musim_preferred_rrc_state_r17_opts {
    enum options { idle, inactive, out_of_connected, nulltype } value;

    const char* to_string() const;
  };
  using musim_preferred_rrc_state_r17_e_ = enumerated<musim_preferred_rrc_state_r17_opts>;

  // member variables
  bool                             musim_preferred_rrc_state_r17_present = false;
  musim_preferred_rrc_state_r17_e_ musim_preferred_rrc_state_r17;
  mu_si_m_gap_pref_list_r17_l      musim_gap_pref_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MaxBW-PreferenceFR2-2-r17 ::= SEQUENCE
struct max_bw_pref_fr2_2_r17_s {
  struct reduced_max_bw_fr2_2_r17_s_ {
    bool                  reduced_bw_fr2_2_dl_r17_present = false;
    bool                  reduced_bw_fr2_2_ul_r17_present = false;
    reduced_aggr_bw_r17_e reduced_bw_fr2_2_dl_r17;
    reduced_aggr_bw_r17_e reduced_bw_fr2_2_ul_r17;
  };

  // member variables
  bool                        reduced_max_bw_fr2_2_r17_present = false;
  reduced_max_bw_fr2_2_r17_s_ reduced_max_bw_fr2_2_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MaxMIMO-LayerPreferenceFR2-2-r17 ::= SEQUENCE
struct max_mimo_layer_pref_fr2_2_r17_s {
  struct reduced_max_mimo_layers_fr2_2_r17_s_ {
    uint8_t reduced_mimo_layers_fr2_2_dl_r17 = 1;
    uint8_t reduced_mimo_layers_fr2_2_ul_r17 = 1;
  };

  // member variables
  bool                                 reduced_max_mimo_layers_fr2_2_r17_present = false;
  reduced_max_mimo_layers_fr2_2_r17_s_ reduced_max_mimo_layers_fr2_2_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultCBR-NR-r16 ::= SEQUENCE
struct meas_result_cbr_nr_r16_s {
  bool    ext                   = false;
  uint8_t sl_pool_report_id_r16 = 1;
  uint8_t sl_cbr_results_nr_r16 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultCLI-RSSI-r16 ::= SEQUENCE
struct meas_result_cli_rssi_r16_s {
  uint8_t rssi_res_id_r16     = 0;
  uint8_t cli_rssi_result_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultFailedCell-r16 ::= SEQUENCE
struct meas_result_failed_cell_r16_s {
  struct meas_result_r16_s_ {
    struct cell_results_r16_s_ {
      meas_quant_results_s results_ssb_cell_r16;
    };
    struct rs_idx_results_r16_s_ {
      results_per_ssb_idx_list_l results_ssb_idxes_r16;
    };

    // member variables
    cell_results_r16_s_   cell_results_r16;
    rs_idx_results_r16_s_ rs_idx_results_r16;
  };

  // member variables
  cgi_info_logging_r16_s cgi_info;
  meas_result_r16_s_     meas_result_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultList2EUTRA-r16 ::= SEQUENCE (SIZE (1..8)) OF MeasResult2EUTRA-r16
using meas_result_list2_eutra_r16_l = dyn_array<meas_result2_eutra_r16_s>;

// MeasResultList2NR-r16 ::= SEQUENCE (SIZE (1..8)) OF MeasResult2NR-r16
using meas_result_list2_nr_r16_l = dyn_array<meas_result2_nr_r16_s>;

// MeasResultListLogging2NR-r16 ::= SEQUENCE (SIZE (1..8)) OF MeasResultLogging2NR-r16
using meas_result_list_logging2_nr_r16_l = dyn_array<meas_result_logging2_nr_r16_s>;

// MeasResultListUTRA-FDD-r16 ::= SEQUENCE (SIZE (1..8)) OF MeasResultUTRA-FDD-r16
using meas_result_list_utra_fdd_r16_l = dyn_array<meas_result_utra_fdd_r16_s>;

// MeasResultSRS-RSRP-r16 ::= SEQUENCE
struct meas_result_srs_rsrp_r16_s {
  uint8_t srs_res_id_r16      = 0;
  uint8_t srs_rsrp_result_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultServingCell-r16 ::= SEQUENCE
struct meas_result_serving_cell_r16_s {
  struct results_ssb_s_ {
    uint8_t              best_ssb_idx = 0;
    meas_quant_results_s best_ssb_results;
    uint8_t              nof_good_ssb = 1;
  };

  // member variables
  bool                 results_ssb_present = false;
  meas_quant_results_s results_ssb_cell;
  results_ssb_s_       results_ssb;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultsPerCellIdleEUTRA-r16 ::= SEQUENCE
struct meas_results_per_cell_idle_eutra_r16_s {
  struct meas_idle_result_eutra_r16_s_ {
    bool    rsrp_result_eutra_r16_present = false;
    bool    rsrq_result_eutra_r16_present = false;
    uint8_t rsrp_result_eutra_r16         = 0;
    int8_t  rsrq_result_eutra_r16         = -30;
  };

  // member variables
  bool                          ext           = false;
  uint16_t                      eutra_pci_r16 = 0;
  meas_idle_result_eutra_r16_s_ meas_idle_result_eutra_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultsPerCellIdleNR-r16 ::= SEQUENCE
struct meas_results_per_cell_idle_nr_r16_s {
  struct meas_idle_result_nr_r16_s_ {
    bool                           rsrp_result_r16_present = false;
    bool                           rsrq_result_r16_present = false;
    uint8_t                        rsrp_result_r16         = 0;
    uint8_t                        rsrq_result_r16         = 0;
    results_per_ssb_idx_list_r16_l results_ssb_idxes_r16;
  };

  // member variables
  bool                       ext     = false;
  uint16_t                   pci_r16 = 0;
  meas_idle_result_nr_r16_s_ meas_idle_result_nr_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MinSchedulingOffsetPreferenceExt-r17 ::= SEQUENCE
struct min_sched_offset_pref_ext_r17_s {
  struct preferred_k0_r17_s_ {
    struct preferred_k0_scs_480k_hz_r17_opts {
      enum options { sl8, sl16, sl32, sl48, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using preferred_k0_scs_480k_hz_r17_e_ = enumerated<preferred_k0_scs_480k_hz_r17_opts>;
    struct preferred_k0_scs_960k_hz_r17_opts {
      enum options { sl8, sl16, sl32, sl48, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using preferred_k0_scs_960k_hz_r17_e_ = enumerated<preferred_k0_scs_960k_hz_r17_opts>;

    // member variables
    bool                            preferred_k0_scs_480k_hz_r17_present = false;
    bool                            preferred_k0_scs_960k_hz_r17_present = false;
    preferred_k0_scs_480k_hz_r17_e_ preferred_k0_scs_480k_hz_r17;
    preferred_k0_scs_960k_hz_r17_e_ preferred_k0_scs_960k_hz_r17;
  };
  struct preferred_k2_r17_s_ {
    struct preferred_k2_scs_480k_hz_r17_opts {
      enum options { sl8, sl16, sl32, sl48, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using preferred_k2_scs_480k_hz_r17_e_ = enumerated<preferred_k2_scs_480k_hz_r17_opts>;
    struct preferred_k2_scs_960k_hz_r17_opts {
      enum options { sl8, sl16, sl32, sl48, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using preferred_k2_scs_960k_hz_r17_e_ = enumerated<preferred_k2_scs_960k_hz_r17_opts>;

    // member variables
    bool                            preferred_k2_scs_480k_hz_r17_present = false;
    bool                            preferred_k2_scs_960k_hz_r17_present = false;
    preferred_k2_scs_480k_hz_r17_e_ preferred_k2_scs_480k_hz_r17;
    preferred_k2_scs_960k_hz_r17_e_ preferred_k2_scs_960k_hz_r17;
  };

  // member variables
  bool                preferred_k0_r17_present = false;
  bool                preferred_k2_r17_present = false;
  preferred_k0_r17_s_ preferred_k0_r17;
  preferred_k2_r17_s_ preferred_k2_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeedForGapsInfoNR-r16 ::= SEQUENCE
struct need_for_gaps_info_nr_r16_s {
  need_for_gaps_intra_freq_list_r16_l intra_freq_need_for_gap_r16;
  need_for_gaps_band_list_nr_r16_l    inter_freq_need_for_gap_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OverheatingAssistance-r17 ::= SEQUENCE
struct overheat_assist_r17_s {
  struct reduced_max_bw_fr2_2_r17_s_ {
    reduced_aggr_bw_r17_e reduced_bw_fr2_2_dl_r17;
    reduced_aggr_bw_r17_e reduced_bw_fr2_2_ul_r17;
  };
  struct reduced_max_mimo_layers_fr2_2_s_ {
    mimo_layers_dl_e reduced_mimo_layers_fr2_2_dl;
    mimo_layers_ul_e reduced_mimo_layers_fr2_2_ul;
  };

  // member variables
  bool                             reduced_max_bw_fr2_2_r17_present      = false;
  bool                             reduced_max_mimo_layers_fr2_2_present = false;
  reduced_max_bw_fr2_2_r17_s_      reduced_max_bw_fr2_2_r17;
  reduced_max_mimo_layers_fr2_2_s_ reduced_max_mimo_layers_fr2_2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PerRAInfoList-r16 ::= SEQUENCE (SIZE (1..200)) OF PerRAInfo-r16
using per_ra_info_list_r16_l = dyn_array<per_ra_info_r16_c>;

// PerRAInfoList-v1660 ::= SEQUENCE (SIZE (1..200)) OF PerRACSI-RSInfo-v1660
using per_ra_info_list_v1660_l = dyn_array<per_ra_csi_r_si_nfo_v1660_s>;

// PropagationDelayDifference-r17 ::= SEQUENCE (SIZE (1..4)) OF INTEGER (-270..270)
using propagation_delay_difference_r17_l = bounded_array<int16_t, 4>;

// RRCReconfigurationComplete-v1640-IEs ::= SEQUENCE
struct rrc_recfg_complete_v1640_ies_s {
  bool                                        non_crit_ext_present = false;
  ul_tx_direct_current_two_carrier_list_r16_l ul_tx_direct_current_two_carrier_list_r16;
  rrc_recfg_complete_v1700_ies_s              non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeComplete-v1720-IEs ::= SEQUENCE
struct rrc_resume_complete_v1720_ies_s {
  bool                                         non_crit_ext_present = false;
  ul_tx_direct_current_more_carrier_list_r17_l ul_tx_direct_current_more_carrier_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReducedMaxBW-FRx-r16 ::= SEQUENCE
struct reduced_max_bw_f_rx_r16_s {
  reduced_aggr_bw_e reduced_bw_dl_r16;
  reduced_aggr_bw_e reduced_bw_ul_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReducedMaxCCs-r16 ::= SEQUENCE
struct reduced_max_ccs_r16_s {
  uint8_t reduced_ccs_dl_r16 = 0;
  uint8_t reduced_ccs_ul_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB-Type-r17 ::= ENUMERATED
struct sib_type_r17_opts {
  enum options {
    sib_type2,
    sib_type3,
    sib_type4,
    sib_type5,
    sib_type9,
    sib_type10_v1610,
    sib_type11_v1610,
    sib_type12_v1610,
    sib_type13_v1610,
    sib_type14_v1610,
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
using sib_type_r17_e = enumerated<sib_type_r17_opts>;

// SL-TrafficPatternInfo-r16 ::= SEQUENCE
struct sl_traffic_pattern_info_r16_s {
  struct traffic_periodicity_r16_opts {
    enum options { ms20, ms50, ms100, ms200, ms300, ms400, ms500, ms600, ms700, ms800, ms900, ms1000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using traffic_periodicity_r16_e_ = enumerated<traffic_periodicity_r16_opts>;

  // member variables
  traffic_periodicity_r16_e_ traffic_periodicity_r16;
  uint16_t                   timing_offset_r16 = 0;
  fixed_bitstring<8>         msg_size_r16;
  uint16_t                   sl_qos_flow_id_r16 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TxResourceReq-v1700 ::= SEQUENCE
struct sl_tx_res_req_v1700_s {
  using sl_drx_info_from_rx_list_r17_l_ = dyn_array<sl_drx_cfg_uc_semi_static_r17_s>;
  struct sl_drx_ind_r17_opts {
    enum options { on, off, nulltype } value;

    const char* to_string() const;
  };
  using sl_drx_ind_r17_e_ = enumerated<sl_drx_ind_r17_opts>;

  // member variables
  bool                            ext                    = false;
  bool                            sl_drx_ind_r17_present = false;
  sl_drx_info_from_rx_list_r17_l_ sl_drx_info_from_rx_list_r17;
  sl_drx_ind_r17_e_               sl_drx_ind_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TxResourceReqCommRelay-r17 ::= CHOICE
struct sl_tx_res_req_comm_relay_r17_c {
  struct types_opts {
    enum options { sl_tx_res_req_l2_u2_n_relay_r17, sl_tx_res_req_l3_u2_n_relay_r17, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  sl_tx_res_req_comm_relay_r17_c() = default;
  sl_tx_res_req_comm_relay_r17_c(const sl_tx_res_req_comm_relay_r17_c& other);
  sl_tx_res_req_comm_relay_r17_c& operator=(const sl_tx_res_req_comm_relay_r17_c& other);
  ~sl_tx_res_req_comm_relay_r17_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  sl_tx_res_req_l2_u2_n_relay_r17_s& sl_tx_res_req_l2_u2_n_relay_r17()
  {
    assert_choice_type(types::sl_tx_res_req_l2_u2_n_relay_r17, type_, "SL-TxResourceReqCommRelay-r17");
    return c.get<sl_tx_res_req_l2_u2_n_relay_r17_s>();
  }
  sl_tx_res_req_r16_s& sl_tx_res_req_l3_u2_n_relay_r17()
  {
    assert_choice_type(types::sl_tx_res_req_l3_u2_n_relay_r17, type_, "SL-TxResourceReqCommRelay-r17");
    return c.get<sl_tx_res_req_r16_s>();
  }
  const sl_tx_res_req_l2_u2_n_relay_r17_s& sl_tx_res_req_l2_u2_n_relay_r17() const
  {
    assert_choice_type(types::sl_tx_res_req_l2_u2_n_relay_r17, type_, "SL-TxResourceReqCommRelay-r17");
    return c.get<sl_tx_res_req_l2_u2_n_relay_r17_s>();
  }
  const sl_tx_res_req_r16_s& sl_tx_res_req_l3_u2_n_relay_r17() const
  {
    assert_choice_type(types::sl_tx_res_req_l3_u2_n_relay_r17, type_, "SL-TxResourceReqCommRelay-r17");
    return c.get<sl_tx_res_req_r16_s>();
  }
  sl_tx_res_req_l2_u2_n_relay_r17_s& set_sl_tx_res_req_l2_u2_n_relay_r17();
  sl_tx_res_req_r16_s&               set_sl_tx_res_req_l3_u2_n_relay_r17();

private:
  types                                                                   type_;
  choice_buffer_t<sl_tx_res_req_l2_u2_n_relay_r17_s, sl_tx_res_req_r16_s> c;

  void destroy_();
};

// UE-MeasurementsAvailable-r16 ::= SEQUENCE
struct ue_meass_available_r16_s {
  bool ext                                      = false;
  bool log_meas_available_r16_present           = false;
  bool log_meas_available_bt_r16_present        = false;
  bool log_meas_available_wlan_r16_present      = false;
  bool conn_est_fail_info_available_r16_present = false;
  bool rlf_info_available_r16_present           = false;
  // ...
  // group 0
  bool success_ho_info_available_r17_present  = false;
  bool sig_log_meas_cfg_available_r17_present = false;
  bool sig_log_meas_cfg_available_r17         = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-GapFR2-Preference-r17 ::= SEQUENCE
struct ul_gap_fr2_pref_r17_s {
  bool    ul_gap_fr2_pattern_pref_r17_present = false;
  uint8_t ul_gap_fr2_pattern_pref_r17         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// VisitedPSCellInfoList-r17 ::= SEQUENCE (SIZE (1..16)) OF VisitedPSCellInfo-r17
using visited_pscell_info_list_r17_l = dyn_array<visited_pscell_info_r17_s>;

// ChoCandidateCell-r17 ::= CHOICE
struct cho_candidate_cell_r17_c {
  struct types_opts {
    enum options { cell_global_id_r17, pci_arfcn_r17, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  cho_candidate_cell_r17_c() = default;
  cho_candidate_cell_r17_c(const cho_candidate_cell_r17_c& other);
  cho_candidate_cell_r17_c& operator=(const cho_candidate_cell_r17_c& other);
  ~cho_candidate_cell_r17_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cgi_info_logging_r16_s& cell_global_id_r17()
  {
    assert_choice_type(types::cell_global_id_r17, type_, "ChoCandidateCell-r17");
    return c.get<cgi_info_logging_r16_s>();
  }
  pci_arfcn_nr_r16_s& pci_arfcn_r17()
  {
    assert_choice_type(types::pci_arfcn_r17, type_, "ChoCandidateCell-r17");
    return c.get<pci_arfcn_nr_r16_s>();
  }
  const cgi_info_logging_r16_s& cell_global_id_r17() const
  {
    assert_choice_type(types::cell_global_id_r17, type_, "ChoCandidateCell-r17");
    return c.get<cgi_info_logging_r16_s>();
  }
  const pci_arfcn_nr_r16_s& pci_arfcn_r17() const
  {
    assert_choice_type(types::pci_arfcn_r17, type_, "ChoCandidateCell-r17");
    return c.get<pci_arfcn_nr_r16_s>();
  }
  cgi_info_logging_r16_s& set_cell_global_id_r17();
  pci_arfcn_nr_r16_s&     set_pci_arfcn_r17();

private:
  types                                                       type_;
  choice_buffer_t<cgi_info_logging_r16_s, pci_arfcn_nr_r16_s> c;

  void destroy_();
};

// ConnEstFailReport-r16 ::= SEQUENCE
struct conn_est_fail_report_r16_s {
  struct meas_result_neigh_cells_r16_s_ {
    meas_result_list2_nr_r16_l    meas_result_neigh_cell_list_nr;
    meas_result_list2_eutra_r16_l meas_result_neigh_cell_list_eutra;
  };

  // member variables
  bool                           ext                       = false;
  bool                           location_info_r16_present = false;
  meas_result_failed_cell_r16_s  meas_result_failed_cell_r16;
  location_info_r16_s            location_info_r16;
  meas_result_neigh_cells_r16_s_ meas_result_neigh_cells_r16;
  uint8_t                        nof_conn_fail_r16 = 1;
  per_ra_info_list_r16_l         per_ra_info_list_r16;
  uint32_t                       time_since_fail_r16 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-Preference-r16 ::= SEQUENCE
struct drx_pref_r16_s {
  struct preferred_drx_inactivity_timer_r16_opts {
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
  using preferred_drx_inactivity_timer_r16_e_ = enumerated<preferred_drx_inactivity_timer_r16_opts>;
  struct preferred_drx_long_cycle_r16_opts {
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
  using preferred_drx_long_cycle_r16_e_ = enumerated<preferred_drx_long_cycle_r16_opts>;
  struct preferred_drx_short_cycle_r16_opts {
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
  using preferred_drx_short_cycle_r16_e_ = enumerated<preferred_drx_short_cycle_r16_opts>;

  // member variables
  bool                                  preferred_drx_inactivity_timer_r16_present  = false;
  bool                                  preferred_drx_long_cycle_r16_present        = false;
  bool                                  preferred_drx_short_cycle_r16_present       = false;
  bool                                  preferred_drx_short_cycle_timer_r16_present = false;
  preferred_drx_inactivity_timer_r16_e_ preferred_drx_inactivity_timer_r16;
  preferred_drx_long_cycle_r16_e_       preferred_drx_long_cycle_r16;
  preferred_drx_short_cycle_r16_e_      preferred_drx_short_cycle_r16;
  uint8_t                               preferred_drx_short_cycle_timer_r16 = 1;

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

// IDC-Assistance-r16 ::= SEQUENCE
struct idc_assist_r16_s {
  bool                                  ext = false;
  affected_carrier_freq_list_r16_l      affected_carrier_freq_list_r16;
  affected_carrier_freq_comb_list_r16_l affected_carrier_freq_comb_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LogMeasInfo-r16 ::= SEQUENCE
struct log_meas_info_r16_s {
  struct meas_result_neigh_cells_r16_s_ {
    meas_result_list_logging2_nr_r16_l meas_result_neigh_cell_list_nr;
    meas_result_list2_eutra_r16_l      meas_result_neigh_cell_list_eutra;
  };

  // member variables
  bool                           ext                                  = false;
  bool                           location_info_r16_present            = false;
  bool                           serv_cell_id_r16_present             = false;
  bool                           meas_result_serving_cell_r16_present = false;
  bool                           any_cell_sel_detected_r16_present    = false;
  location_info_r16_s            location_info_r16;
  uint16_t                       relative_time_stamp_r16 = 0;
  cgi_info_logging_r16_s         serv_cell_id_r16;
  meas_result_serving_cell_r16_s meas_result_serving_cell_r16;
  meas_result_neigh_cells_r16_s_ meas_result_neigh_cells_r16;
  // ...
  // group 0
  bool in_dev_coex_detected_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MaxBW-Preference-r16 ::= SEQUENCE
struct max_bw_pref_r16_s {
  bool                      reduced_max_bw_fr1_r16_present = false;
  bool                      reduced_max_bw_fr2_r16_present = false;
  reduced_max_bw_f_rx_r16_s reduced_max_bw_fr1_r16;
  reduced_max_bw_f_rx_r16_s reduced_max_bw_fr2_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MaxCC-Preference-r16 ::= SEQUENCE
struct max_cc_pref_r16_s {
  bool                  reduced_max_ccs_r16_present = false;
  reduced_max_ccs_r16_s reduced_max_ccs_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MaxMIMO-LayerPreference-r16 ::= SEQUENCE
struct max_mimo_layer_pref_r16_s {
  struct reduced_max_mimo_layers_fr1_r16_s_ {
    uint8_t reduced_mimo_layers_fr1_dl_r16 = 1;
    uint8_t reduced_mimo_layers_fr1_ul_r16 = 1;
  };
  struct reduced_max_mimo_layers_fr2_r16_s_ {
    uint8_t reduced_mimo_layers_fr2_dl_r16 = 1;
    uint8_t reduced_mimo_layers_fr2_ul_r16 = 1;
  };

  // member variables
  bool                               reduced_max_mimo_layers_fr1_r16_present = false;
  bool                               reduced_max_mimo_layers_fr2_r16_present = false;
  reduced_max_mimo_layers_fr1_r16_s_ reduced_max_mimo_layers_fr1_r16;
  reduced_max_mimo_layers_fr2_r16_s_ reduced_max_mimo_layers_fr2_r16;

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

// MeasResult2UTRA-FDD-r16 ::= SEQUENCE
struct meas_result2_utra_fdd_r16_s {
  uint16_t                        carrier_freq_r16 = 0;
  meas_result_list_utra_fdd_r16_l meas_result_neigh_cell_list_r16;

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

// MeasResultListCLI-RSSI-r16 ::= SEQUENCE (SIZE (1..8)) OF MeasResultCLI-RSSI-r16
using meas_result_list_cli_rssi_r16_l = dyn_array<meas_result_cli_rssi_r16_s>;

// MeasResultListSRS-RSRP-r16 ::= SEQUENCE (SIZE (1..8)) OF MeasResultSRS-RSRP-r16
using meas_result_list_srs_rsrp_r16_l = dyn_array<meas_result_srs_rsrp_r16_s>;

// MeasResultNR-SL-r16 ::= SEQUENCE
struct meas_result_nr_sl_r16_s {
  using meas_result_list_cbr_nr_r16_l_ = dyn_array<meas_result_cbr_nr_r16_s>;

  // member variables
  bool                           ext = false;
  meas_result_list_cbr_nr_r16_l_ meas_result_list_cbr_nr_r16;
  // ...

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

// MeasResultSuccessHONR-r17 ::= SEQUENCE
struct meas_result_success_ho_nr_r17_s {
  struct meas_result_r17_s_ {
    struct cell_results_r17_s_ {
      bool                 results_ssb_cell_r17_present    = false;
      bool                 results_csi_rs_cell_r17_present = false;
      meas_quant_results_s results_ssb_cell_r17;
      meas_quant_results_s results_csi_rs_cell_r17;
    };
    struct rs_idx_results_r17_s_ {
      results_per_ssb_idx_list_l    results_ssb_idxes_r17;
      results_per_csi_rs_idx_list_l results_csi_rs_idxes_r17;
    };

    // member variables
    cell_results_r17_s_   cell_results_r17;
    rs_idx_results_r17_s_ rs_idx_results_r17;
  };

  // member variables
  meas_result_r17_s_ meas_result_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultsPerCarrierIdleEUTRA-r16 ::= SEQUENCE
struct meas_results_per_carrier_idle_eutra_r16_s {
  using meas_results_per_cell_list_idle_eutra_r16_l_ = dyn_array<meas_results_per_cell_idle_eutra_r16_s>;

  // member variables
  bool                                         ext                    = false;
  uint32_t                                     carrier_freq_eutra_r16 = 0;
  meas_results_per_cell_list_idle_eutra_r16_l_ meas_results_per_cell_list_idle_eutra_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultsPerCarrierIdleNR-r16 ::= SEQUENCE
struct meas_results_per_carrier_idle_nr_r16_s {
  using meas_results_per_cell_list_idle_nr_r16_l_ = dyn_array<meas_results_per_cell_idle_nr_r16_s>;

  // member variables
  bool                                      ext              = false;
  uint32_t                                  carrier_freq_r16 = 0;
  meas_results_per_cell_list_idle_nr_r16_l_ meas_results_per_cell_list_idle_nr_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MinSchedulingOffsetPreference-r16 ::= SEQUENCE
struct min_sched_offset_pref_r16_s {
  struct preferred_k0_r16_s_ {
    struct preferred_k0_scs_15k_hz_r16_opts {
      enum options { sl1, sl2, sl4, sl6, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using preferred_k0_scs_15k_hz_r16_e_ = enumerated<preferred_k0_scs_15k_hz_r16_opts>;
    struct preferred_k0_scs_30k_hz_r16_opts {
      enum options { sl1, sl2, sl4, sl6, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using preferred_k0_scs_30k_hz_r16_e_ = enumerated<preferred_k0_scs_30k_hz_r16_opts>;
    struct preferred_k0_scs_60k_hz_r16_opts {
      enum options { sl2, sl4, sl8, sl12, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using preferred_k0_scs_60k_hz_r16_e_ = enumerated<preferred_k0_scs_60k_hz_r16_opts>;
    struct preferred_k0_scs_120k_hz_r16_opts {
      enum options { sl2, sl4, sl8, sl12, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using preferred_k0_scs_120k_hz_r16_e_ = enumerated<preferred_k0_scs_120k_hz_r16_opts>;

    // member variables
    bool                            preferred_k0_scs_15k_hz_r16_present  = false;
    bool                            preferred_k0_scs_30k_hz_r16_present  = false;
    bool                            preferred_k0_scs_60k_hz_r16_present  = false;
    bool                            preferred_k0_scs_120k_hz_r16_present = false;
    preferred_k0_scs_15k_hz_r16_e_  preferred_k0_scs_15k_hz_r16;
    preferred_k0_scs_30k_hz_r16_e_  preferred_k0_scs_30k_hz_r16;
    preferred_k0_scs_60k_hz_r16_e_  preferred_k0_scs_60k_hz_r16;
    preferred_k0_scs_120k_hz_r16_e_ preferred_k0_scs_120k_hz_r16;
  };
  struct preferred_k2_r16_s_ {
    struct preferred_k2_scs_15k_hz_r16_opts {
      enum options { sl1, sl2, sl4, sl6, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using preferred_k2_scs_15k_hz_r16_e_ = enumerated<preferred_k2_scs_15k_hz_r16_opts>;
    struct preferred_k2_scs_30k_hz_r16_opts {
      enum options { sl1, sl2, sl4, sl6, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using preferred_k2_scs_30k_hz_r16_e_ = enumerated<preferred_k2_scs_30k_hz_r16_opts>;
    struct preferred_k2_scs_60k_hz_r16_opts {
      enum options { sl2, sl4, sl8, sl12, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using preferred_k2_scs_60k_hz_r16_e_ = enumerated<preferred_k2_scs_60k_hz_r16_opts>;
    struct preferred_k2_scs_120k_hz_r16_opts {
      enum options { sl2, sl4, sl8, sl12, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using preferred_k2_scs_120k_hz_r16_e_ = enumerated<preferred_k2_scs_120k_hz_r16_opts>;

    // member variables
    bool                            preferred_k2_scs_15k_hz_r16_present  = false;
    bool                            preferred_k2_scs_30k_hz_r16_present  = false;
    bool                            preferred_k2_scs_60k_hz_r16_present  = false;
    bool                            preferred_k2_scs_120k_hz_r16_present = false;
    preferred_k2_scs_15k_hz_r16_e_  preferred_k2_scs_15k_hz_r16;
    preferred_k2_scs_30k_hz_r16_e_  preferred_k2_scs_30k_hz_r16;
    preferred_k2_scs_60k_hz_r16_e_  preferred_k2_scs_60k_hz_r16;
    preferred_k2_scs_120k_hz_r16_e_ preferred_k2_scs_120k_hz_r16;
  };

  // member variables
  bool                preferred_k0_r16_present = false;
  bool                preferred_k2_r16_present = false;
  preferred_k0_r16_s_ preferred_k0_r16;
  preferred_k2_r16_s_ preferred_k2_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-PRS-MeasurementInfo-r16 ::= SEQUENCE
struct nr_prs_meas_info_r16_s {
  struct nr_meas_prs_repeat_and_offset_r16_c_ {
    struct types_opts {
      enum options { ms20_r16, ms40_r16, ms80_r16, ms160_r16, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    nr_meas_prs_repeat_and_offset_r16_c_() = default;
    nr_meas_prs_repeat_and_offset_r16_c_(const nr_meas_prs_repeat_and_offset_r16_c_& other);
    nr_meas_prs_repeat_and_offset_r16_c_& operator=(const nr_meas_prs_repeat_and_offset_r16_c_& other);
    ~nr_meas_prs_repeat_and_offset_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ms20_r16()
    {
      assert_choice_type(types::ms20_r16, type_, "nr-MeasPRS-RepetitionAndOffset-r16");
      return c.get<uint8_t>();
    }
    uint8_t& ms40_r16()
    {
      assert_choice_type(types::ms40_r16, type_, "nr-MeasPRS-RepetitionAndOffset-r16");
      return c.get<uint8_t>();
    }
    uint8_t& ms80_r16()
    {
      assert_choice_type(types::ms80_r16, type_, "nr-MeasPRS-RepetitionAndOffset-r16");
      return c.get<uint8_t>();
    }
    uint8_t& ms160_r16()
    {
      assert_choice_type(types::ms160_r16, type_, "nr-MeasPRS-RepetitionAndOffset-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& ms20_r16() const
    {
      assert_choice_type(types::ms20_r16, type_, "nr-MeasPRS-RepetitionAndOffset-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& ms40_r16() const
    {
      assert_choice_type(types::ms40_r16, type_, "nr-MeasPRS-RepetitionAndOffset-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& ms80_r16() const
    {
      assert_choice_type(types::ms80_r16, type_, "nr-MeasPRS-RepetitionAndOffset-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& ms160_r16() const
    {
      assert_choice_type(types::ms160_r16, type_, "nr-MeasPRS-RepetitionAndOffset-r16");
      return c.get<uint8_t>();
    }
    uint8_t& set_ms20_r16();
    uint8_t& set_ms40_r16();
    uint8_t& set_ms80_r16();
    uint8_t& set_ms160_r16();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct nr_meas_prs_len_r16_opts {
    enum options { ms1dot5, ms3, ms3dot5, ms4, ms5dot5, ms6, ms10, ms20, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using nr_meas_prs_len_r16_e_ = enumerated<nr_meas_prs_len_r16_opts>;

  // member variables
  bool                                 ext                = false;
  uint32_t                             dl_prs_point_a_r16 = 0;
  nr_meas_prs_repeat_and_offset_r16_c_ nr_meas_prs_repeat_and_offset_r16;
  nr_meas_prs_len_r16_e_               nr_meas_prs_len_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-TimeStamp-r17 ::= SEQUENCE
struct nr_time_stamp_r17_s {
  struct nr_slot_r17_c_ {
    struct types_opts {
      enum options { scs15_r17, scs30_r17, scs60_r17, scs120_r17, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    nr_slot_r17_c_() = default;
    nr_slot_r17_c_(const nr_slot_r17_c_& other);
    nr_slot_r17_c_& operator=(const nr_slot_r17_c_& other);
    ~nr_slot_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& scs15_r17()
    {
      assert_choice_type(types::scs15_r17, type_, "nr-Slot-r17");
      return c.get<uint8_t>();
    }
    uint8_t& scs30_r17()
    {
      assert_choice_type(types::scs30_r17, type_, "nr-Slot-r17");
      return c.get<uint8_t>();
    }
    uint8_t& scs60_r17()
    {
      assert_choice_type(types::scs60_r17, type_, "nr-Slot-r17");
      return c.get<uint8_t>();
    }
    uint8_t& scs120_r17()
    {
      assert_choice_type(types::scs120_r17, type_, "nr-Slot-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& scs15_r17() const
    {
      assert_choice_type(types::scs15_r17, type_, "nr-Slot-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& scs30_r17() const
    {
      assert_choice_type(types::scs30_r17, type_, "nr-Slot-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& scs60_r17() const
    {
      assert_choice_type(types::scs60_r17, type_, "nr-Slot-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& scs120_r17() const
    {
      assert_choice_type(types::scs120_r17, type_, "nr-Slot-r17");
      return c.get<uint8_t>();
    }
    uint8_t& set_scs15_r17();
    uint8_t& set_scs30_r17();
    uint8_t& set_scs60_r17();
    uint8_t& set_scs120_r17();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool           ext        = false;
  uint16_t       nr_sfn_r17 = 0;
  nr_slot_r17_c_ nr_slot_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RA-InformationCommon-r16 ::= SEQUENCE
struct ra_info_common_r16_s {
  struct msg1_fdm_r16_opts {
    enum options { one, two, four, eight, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using msg1_fdm_r16_e_ = enumerated<msg1_fdm_r16_opts>;
  struct msg1_fdmcfra_r16_opts {
    enum options { one, two, four, eight, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using msg1_fdmcfra_r16_e_ = enumerated<msg1_fdmcfra_r16_opts>;
  struct msg1_scs_from_prach_cfg_idx_r16_opts {
    enum options { khz1dot25, khz5, spare2, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using msg1_scs_from_prach_cfg_idx_r16_e_ = enumerated<msg1_scs_from_prach_cfg_idx_r16_opts>;
  struct msg1_scs_from_prach_cfg_idx_cfra_r16_opts {
    enum options { khz1dot25, khz5, spare2, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using msg1_scs_from_prach_cfg_idx_cfra_r16_e_ = enumerated<msg1_scs_from_prach_cfg_idx_cfra_r16_opts>;
  struct msg_a_ro_fdm_r17_opts {
    enum options { one, two, four, eight, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using msg_a_ro_fdm_r17_e_ = enumerated<msg_a_ro_fdm_r17_opts>;
  struct msg_a_ro_fdmcfra_r17_opts {
    enum options { one, two, four, eight, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using msg_a_ro_fdmcfra_r17_e_ = enumerated<msg_a_ro_fdmcfra_r17_opts>;
  struct msg_a_scs_from_prach_cfg_idx_r17_opts {
    enum options { khz1dot25, khz5, spare2, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using msg_a_scs_from_prach_cfg_idx_r17_e_ = enumerated<msg_a_scs_from_prach_cfg_idx_r17_opts>;
  struct msg_a_trans_max_r17_opts {
    enum options { n1, n2, n4, n6, n8, n10, n20, n50, n100, n200, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using msg_a_trans_max_r17_e_ = enumerated<msg_a_trans_max_r17_opts>;
  struct nrof_msg_a_po_fdm_r17_opts {
    enum options { one, two, four, eight, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrof_msg_a_po_fdm_r17_e_       = enumerated<nrof_msg_a_po_fdm_r17_opts>;
  using intended_sibs_r17_l_           = bounded_array<sib_type_r17_e, 32>;
  using ssbs_for_si_acquisition_r17_l_ = dyn_array<uint8_t>;

  // member variables
  bool                   ext                                      = false;
  bool                   msg1_freq_start_r16_present              = false;
  bool                   msg1_freq_start_cfra_r16_present         = false;
  bool                   msg1_subcarrier_spacing_r16_present      = false;
  bool                   msg1_subcarrier_spacing_cfra_r16_present = false;
  bool                   msg1_fdm_r16_present                     = false;
  bool                   msg1_fdmcfra_r16_present                 = false;
  uint32_t               absolute_freq_point_a_r16                = 0;
  uint16_t               location_and_bw_r16                      = 0;
  subcarrier_spacing_e   subcarrier_spacing_r16;
  uint16_t               msg1_freq_start_r16      = 0;
  uint16_t               msg1_freq_start_cfra_r16 = 0;
  subcarrier_spacing_e   msg1_subcarrier_spacing_r16;
  subcarrier_spacing_e   msg1_subcarrier_spacing_cfra_r16;
  msg1_fdm_r16_e_        msg1_fdm_r16;
  msg1_fdmcfra_r16_e_    msg1_fdmcfra_r16;
  per_ra_info_list_r16_l per_ra_info_list_r16;
  // ...
  // group 0
  copy_ptr<per_ra_info_list_v1660_l> per_ra_info_list_v1660;
  // group 1
  bool                               msg1_scs_from_prach_cfg_idx_r16_present = false;
  msg1_scs_from_prach_cfg_idx_r16_e_ msg1_scs_from_prach_cfg_idx_r16;
  // group 2
  bool                                    msg1_scs_from_prach_cfg_idx_cfra_r16_present = false;
  msg1_scs_from_prach_cfg_idx_cfra_r16_e_ msg1_scs_from_prach_cfg_idx_cfra_r16;
  // group 3
  bool                                     msg_a_ro_freq_start_r17_present           = false;
  bool                                     msg_a_ro_freq_start_cfra_r17_present      = false;
  bool                                     msg_a_subcarrier_spacing_r17_present      = false;
  bool                                     msg_a_ro_fdm_r17_present                  = false;
  bool                                     msg_a_ro_fdmcfra_r17_present              = false;
  bool                                     msg_a_scs_from_prach_cfg_idx_r17_present  = false;
  bool                                     msg_a_trans_max_r17_present               = false;
  bool                                     msg_a_mcs_r17_present                     = false;
  bool                                     nrof_prbs_per_msg_a_po_r17_present        = false;
  bool                                     msg_a_pusch_time_domain_alloc_r17_present = false;
  bool                                     freq_start_msg_a_pusch_r17_present        = false;
  bool                                     nrof_msg_a_po_fdm_r17_present             = false;
  bool                                     dl_pathloss_rsrp_r17_present              = false;
  bool                                     msg_a_pusch_payload_size_r17_present      = false;
  bool                                     on_demand_si_success_r17_present          = false;
  uint16_t                                 msg_a_ro_freq_start_r17                   = 0;
  uint16_t                                 msg_a_ro_freq_start_cfra_r17              = 0;
  subcarrier_spacing_e                     msg_a_subcarrier_spacing_r17;
  msg_a_ro_fdm_r17_e_                      msg_a_ro_fdm_r17;
  msg_a_ro_fdmcfra_r17_e_                  msg_a_ro_fdmcfra_r17;
  msg_a_scs_from_prach_cfg_idx_r17_e_      msg_a_scs_from_prach_cfg_idx_r17;
  msg_a_trans_max_r17_e_                   msg_a_trans_max_r17;
  uint8_t                                  msg_a_mcs_r17                     = 0;
  uint8_t                                  nrof_prbs_per_msg_a_po_r17        = 1;
  uint8_t                                  msg_a_pusch_time_domain_alloc_r17 = 1;
  uint16_t                                 freq_start_msg_a_pusch_r17        = 0;
  nrof_msg_a_po_fdm_r17_e_                 nrof_msg_a_po_fdm_r17;
  uint8_t                                  dl_pathloss_rsrp_r17 = 0;
  copy_ptr<intended_sibs_r17_l_>           intended_sibs_r17;
  copy_ptr<ssbs_for_si_acquisition_r17_l_> ssbs_for_si_acquisition_r17;
  fixed_bitstring<5>                       msg_a_pusch_payload_size_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RAN-VisibleMeasurements-r17 ::= SEQUENCE
struct ran_visible_meass_r17_s {
  using app_layer_buffer_level_list_r17_l_ = bounded_array<uint16_t, 8>;
  using pdu_session_id_list_r17_l_         = dyn_array<uint16_t>;

  // member variables
  bool                               ext                                         = false;
  bool                               playout_delay_for_media_startup_r17_present = false;
  app_layer_buffer_level_list_r17_l_ app_layer_buffer_level_list_r17;
  uint16_t                           playout_delay_for_media_startup_r17 = 0;
  pdu_session_id_list_r17_l_         pdu_session_id_list_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCReconfigurationComplete-v1610-IEs ::= SEQUENCE
struct rrc_recfg_complete_v1610_ies_s {
  bool                           ue_meass_available_r16_present    = false;
  bool                           need_for_gaps_info_nr_r16_present = false;
  bool                           non_crit_ext_present              = false;
  ue_meass_available_r16_s       ue_meass_available_r16;
  need_for_gaps_info_nr_r16_s    need_for_gaps_info_nr_r16;
  rrc_recfg_complete_v1640_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeComplete-v1700-IEs ::= SEQUENCE
struct rrc_resume_complete_v1700_ies_s {
  bool                               need_for_gap_ncsg_info_nr_r17_present    = false;
  bool                               need_for_gap_ncsg_info_eutra_r17_present = false;
  bool                               non_crit_ext_present                     = false;
  need_for_gap_ncsg_info_nr_r17_s    need_for_gap_ncsg_info_nr_r17;
  need_for_gap_ncsg_info_eutra_r17_s need_for_gap_ncsg_info_eutra_r17;
  rrc_resume_complete_v1720_ies_s    non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetupComplete-v1700-IEs ::= SEQUENCE
struct rrc_setup_complete_v1700_ies_s {
  bool onboarding_request_r17_present = false;
  bool non_crit_ext_present           = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReleasePreference-r16 ::= SEQUENCE
struct release_pref_r16_s {
  struct preferred_rrc_state_r16_opts {
    enum options { idle, inactive, connected, out_of_connected, nulltype } value;

    const char* to_string() const;
  };
  using preferred_rrc_state_r16_e_ = enumerated<preferred_rrc_state_r16_opts>;

  // member variables
  preferred_rrc_state_r16_e_ preferred_rrc_state_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SHR-Cause-r17 ::= SEQUENCE
struct shr_cause_r17_s {
  bool ext                          = false;
  bool t304_cause_r17_present       = false;
  bool t310_cause_r17_present       = false;
  bool t312_cause_r17_present       = false;
  bool source_daps_fail_r17_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-RxDRX-Report-v1700 ::= SEQUENCE
struct sl_rx_drx_report_v1700_s {
  bool                            ext = false;
  sl_drx_cfg_uc_semi_static_r17_s sl_drx_cfg_from_tx_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-RxInterestedGC-BC-Dest-r17 ::= SEQUENCE
struct sl_rx_interested_gc_bc_dest_r17_s {
  using sl_rx_interested_qos_info_list_r17_l_ = dyn_array<sl_qos_info_r16_s>;

  // member variables
  sl_rx_interested_qos_info_list_r17_l_ sl_rx_interested_qos_info_list_r17;
  fixed_bitstring<24>                   sl_dest_id_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TxResourceReqCommRelayInfo-r17 ::= SEQUENCE
struct sl_tx_res_req_comm_relay_info_r17_s {
  bool                           sl_relay_drx_cfg_r17_present = false;
  sl_tx_res_req_v1700_s          sl_relay_drx_cfg_r17;
  sl_tx_res_req_comm_relay_r17_c sl_tx_res_req_comm_relay_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TxResourceReqDisc-r17 ::= SEQUENCE
struct sl_tx_res_req_disc_r17_s {
  struct sl_cast_type_disc_r17_opts {
    enum options { broadcast, groupcast, unicast, spare1, nulltype } value;

    const char* to_string() const;
  };
  using sl_cast_type_disc_r17_e_         = enumerated<sl_cast_type_disc_r17_opts>;
  using sl_type_tx_sync_list_disc_r17_l_ = bounded_array<sl_type_tx_sync_r16_e, 8>;
  struct sl_discovery_type_r17_opts {
    enum options { relay, non_relay, nulltype } value;

    const char* to_string() const;
  };
  using sl_discovery_type_r17_e_ = enumerated<sl_discovery_type_r17_opts>;

  // member variables
  bool                             ext                               = false;
  bool                             sl_source_id_relay_ue_r17_present = false;
  fixed_bitstring<24>              sl_dest_id_disc_r17;
  fixed_bitstring<24>              sl_source_id_relay_ue_r17;
  sl_cast_type_disc_r17_e_         sl_cast_type_disc_r17;
  sl_tx_interested_freq_list_r16_l sl_tx_interested_freq_list_disc_r17;
  sl_type_tx_sync_list_disc_r17_l_ sl_type_tx_sync_list_disc_r17;
  sl_discovery_type_r17_e_         sl_discovery_type_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-UE-AssistanceInformationNR-r16 ::= SEQUENCE (SIZE (1..8)) OF SL-TrafficPatternInfo-r16
using sl_ue_assist_info_nr_r16_l = dyn_array<sl_traffic_pattern_info_r16_s>;

// UEAssistanceInformation-v1700-IEs ::= SEQUENCE
struct ue_assist_info_v1700_ies_s {
  struct non_sdt_data_ind_r17_s_ {
    bool           resume_cause_r17_present = false;
    resume_cause_e resume_cause_r17;
  };
  struct scg_deactivation_pref_r17_opts {
    enum options { scg_deactivation_preferred, no_pref, nulltype } value;

    const char* to_string() const;
  };
  using scg_deactivation_pref_r17_e_ = enumerated<scg_deactivation_pref_r17_opts>;

  // member variables
  bool                               ul_gap_fr2_pref_r17_present                = false;
  bool                               musim_assist_r17_present                   = false;
  bool                               overheat_assist_r17_present                = false;
  bool                               max_bw_pref_fr2_2_r17_present              = false;
  bool                               max_mimo_layer_pref_fr2_2_r17_present      = false;
  bool                               min_sched_offset_pref_ext_r17_present      = false;
  bool                               rlm_meas_relaxation_state_r17_present      = false;
  bool                               bfd_meas_relaxation_state_r17_present      = false;
  bool                               non_sdt_data_ind_r17_present               = false;
  bool                               scg_deactivation_pref_r17_present          = false;
  bool                               ul_data_r17_present                        = false;
  bool                               rrm_meas_relaxation_fulfilment_r17_present = false;
  bool                               non_crit_ext_present                       = false;
  ul_gap_fr2_pref_r17_s              ul_gap_fr2_pref_r17;
  mu_si_m_assist_r17_s               musim_assist_r17;
  overheat_assist_r17_s              overheat_assist_r17;
  max_bw_pref_fr2_2_r17_s            max_bw_pref_fr2_2_r17;
  max_mimo_layer_pref_fr2_2_r17_s    max_mimo_layer_pref_fr2_2_r17;
  min_sched_offset_pref_ext_r17_s    min_sched_offset_pref_ext_r17;
  bool                               rlm_meas_relaxation_state_r17 = false;
  bounded_bitstring<1, 32>           bfd_meas_relaxation_state_r17;
  non_sdt_data_ind_r17_s_            non_sdt_data_ind_r17;
  scg_deactivation_pref_r17_e_       scg_deactivation_pref_r17;
  bool                               rrm_meas_relaxation_fulfilment_r17 = false;
  propagation_delay_difference_r17_l propagation_delay_difference_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-PDCP-DelayValueResult-r16 ::= SEQUENCE
struct ul_pdcp_delay_value_result_r16_s {
  bool     ext               = false;
  uint8_t  drb_id_r16        = 1;
  uint16_t average_delay_r16 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-PDCP-ExcessDelayResult-r17 ::= SEQUENCE
struct ul_pdcp_excess_delay_result_r17_s {
  bool    ext              = false;
  uint8_t drb_id_r17       = 1;
  uint8_t excess_delay_r17 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// VisitedCellInfo-r16 ::= SEQUENCE
struct visited_cell_info_r16_s {
  struct visited_cell_id_r16_c_ {
    struct nr_cell_id_r16_c_ {
      struct types_opts {
        enum options { cgi_info, pci_arfcn_r16, nulltype } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      nr_cell_id_r16_c_() = default;
      nr_cell_id_r16_c_(const nr_cell_id_r16_c_& other);
      nr_cell_id_r16_c_& operator=(const nr_cell_id_r16_c_& other);
      ~nr_cell_id_r16_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      cgi_info_logging_r16_s& cgi_info()
      {
        assert_choice_type(types::cgi_info, type_, "nr-CellId-r16");
        return c.get<cgi_info_logging_r16_s>();
      }
      pci_arfcn_nr_r16_s& pci_arfcn_r16()
      {
        assert_choice_type(types::pci_arfcn_r16, type_, "nr-CellId-r16");
        return c.get<pci_arfcn_nr_r16_s>();
      }
      const cgi_info_logging_r16_s& cgi_info() const
      {
        assert_choice_type(types::cgi_info, type_, "nr-CellId-r16");
        return c.get<cgi_info_logging_r16_s>();
      }
      const pci_arfcn_nr_r16_s& pci_arfcn_r16() const
      {
        assert_choice_type(types::pci_arfcn_r16, type_, "nr-CellId-r16");
        return c.get<pci_arfcn_nr_r16_s>();
      }
      cgi_info_logging_r16_s& set_cgi_info();
      pci_arfcn_nr_r16_s&     set_pci_arfcn_r16();

    private:
      types                                                       type_;
      choice_buffer_t<cgi_info_logging_r16_s, pci_arfcn_nr_r16_s> c;

      void destroy_();
    };
    struct eutra_cell_id_r16_c_ {
      struct types_opts {
        enum options { cell_global_id_r16, pci_arfcn_r16, nulltype } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      eutra_cell_id_r16_c_() = default;
      eutra_cell_id_r16_c_(const eutra_cell_id_r16_c_& other);
      eutra_cell_id_r16_c_& operator=(const eutra_cell_id_r16_c_& other);
      ~eutra_cell_id_r16_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      cgi_info_eutra_s& cell_global_id_r16()
      {
        assert_choice_type(types::cell_global_id_r16, type_, "eutra-CellId-r16");
        return c.get<cgi_info_eutra_s>();
      }
      pci_arfcn_eutra_r16_s& pci_arfcn_r16()
      {
        assert_choice_type(types::pci_arfcn_r16, type_, "eutra-CellId-r16");
        return c.get<pci_arfcn_eutra_r16_s>();
      }
      const cgi_info_eutra_s& cell_global_id_r16() const
      {
        assert_choice_type(types::cell_global_id_r16, type_, "eutra-CellId-r16");
        return c.get<cgi_info_eutra_s>();
      }
      const pci_arfcn_eutra_r16_s& pci_arfcn_r16() const
      {
        assert_choice_type(types::pci_arfcn_r16, type_, "eutra-CellId-r16");
        return c.get<pci_arfcn_eutra_r16_s>();
      }
      cgi_info_eutra_s&      set_cell_global_id_r16();
      pci_arfcn_eutra_r16_s& set_pci_arfcn_r16();

    private:
      types                                                    type_;
      choice_buffer_t<cgi_info_eutra_s, pci_arfcn_eutra_r16_s> c;

      void destroy_();
    };
    struct types_opts {
      enum options { nr_cell_id_r16, eutra_cell_id_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    visited_cell_id_r16_c_() = default;
    visited_cell_id_r16_c_(const visited_cell_id_r16_c_& other);
    visited_cell_id_r16_c_& operator=(const visited_cell_id_r16_c_& other);
    ~visited_cell_id_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nr_cell_id_r16_c_& nr_cell_id_r16()
    {
      assert_choice_type(types::nr_cell_id_r16, type_, "visitedCellId-r16");
      return c.get<nr_cell_id_r16_c_>();
    }
    eutra_cell_id_r16_c_& eutra_cell_id_r16()
    {
      assert_choice_type(types::eutra_cell_id_r16, type_, "visitedCellId-r16");
      return c.get<eutra_cell_id_r16_c_>();
    }
    const nr_cell_id_r16_c_& nr_cell_id_r16() const
    {
      assert_choice_type(types::nr_cell_id_r16, type_, "visitedCellId-r16");
      return c.get<nr_cell_id_r16_c_>();
    }
    const eutra_cell_id_r16_c_& eutra_cell_id_r16() const
    {
      assert_choice_type(types::eutra_cell_id_r16, type_, "visitedCellId-r16");
      return c.get<eutra_cell_id_r16_c_>();
    }
    nr_cell_id_r16_c_&    set_nr_cell_id_r16();
    eutra_cell_id_r16_c_& set_eutra_cell_id_r16();

  private:
    types                                                    type_;
    choice_buffer_t<eutra_cell_id_r16_c_, nr_cell_id_r16_c_> c;

    void destroy_();
  };

  // member variables
  bool                   ext                         = false;
  bool                   visited_cell_id_r16_present = false;
  visited_cell_id_r16_c_ visited_cell_id_r16;
  uint16_t               time_spent_r16 = 0;
  // ...
  // group 0
  copy_ptr<visited_pscell_info_list_r17_l> visited_pscell_info_list_report_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ChoCandidateCellList-r17 ::= SEQUENCE (SIZE (1..8)) OF ChoCandidateCell-r17
using cho_candidate_cell_list_r17_l = dyn_array<cho_candidate_cell_r17_c>;

// ConnEstFailReportList-r17 ::= SEQUENCE (SIZE (1..4)) OF ConnEstFailReport-r16
using conn_est_fail_report_list_r17_l = dyn_array<conn_est_fail_report_r16_s>;

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

// FailureInfoDAPS-r16 ::= SEQUENCE
struct fail_info_daps_r16_s {
  struct fail_type_r16_opts {
    enum options { daps_fail, spare3, spare2, spare1, nulltype } value;

    const char* to_string() const;
  };
  using fail_type_r16_e_ = enumerated<fail_type_r16_opts>;

  // member variables
  fail_type_r16_e_ fail_type_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LogMeasInfoList-r16 ::= SEQUENCE (SIZE (1..520)) OF LogMeasInfo-r16
using log_meas_info_list_r16_l = dyn_array<log_meas_info_r16_s>;

// MBS-ServiceInfo-r17 ::= SEQUENCE
struct mbs_service_info_r17_s {
  tmgi_r17_s tmgi_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasReportAppLayer-r17 ::= SEQUENCE
struct meas_report_app_layer_r17_s {
  struct app_layer_session_status_r17_opts {
    enum options { started, stopped, nulltype } value;

    const char* to_string() const;
  };
  using app_layer_session_status_r17_e_ = enumerated<app_layer_session_status_r17_opts>;

  // member variables
  bool                            app_layer_session_status_r17_present = false;
  bool                            ran_visible_meass_r17_present        = false;
  uint8_t                         meas_cfg_app_layer_id_r17            = 0;
  dyn_octstring                   meas_report_app_layer_container_r17;
  app_layer_session_status_r17_e_ app_layer_session_status_r17;
  ran_visible_meass_r17_s         ran_visible_meass_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultCLI-r16 ::= SEQUENCE
struct meas_result_cli_r16_s {
  meas_result_list_srs_rsrp_r16_l meas_result_list_srs_rsrp_r16;
  meas_result_list_cli_rssi_r16_l meas_result_list_cli_rssi_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultCellListSFTD-NR ::= SEQUENCE (SIZE (1..3)) OF MeasResultCellSFTD-NR
using meas_result_cell_list_sftd_nr_l = dyn_array<meas_result_cell_sftd_nr_s>;

// MeasResultForRSSI-r16 ::= SEQUENCE
struct meas_result_for_rssi_r16_s {
  uint8_t rssi_result_r16  = 0;
  uint8_t ch_occupancy_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultFreqList ::= SEQUENCE (SIZE (1..8)) OF MeasResult2NR
using meas_result_freq_list_l = dyn_array<meas_result2_nr_s>;

// MeasResultFreqListFailMRDC ::= SEQUENCE (SIZE (1..8)) OF MeasResult2EUTRA
using meas_result_freq_list_fail_mrdc_l = dyn_array<meas_result2_eutra_s>;

// MeasResultIdleEUTRA-r16 ::= SEQUENCE
struct meas_result_idle_eutra_r16_s {
  using meas_results_per_carrier_list_idle_eutra_r16_l_ = dyn_array<meas_results_per_carrier_idle_eutra_r16_s>;

  // member variables
  bool                                            ext = false;
  meas_results_per_carrier_list_idle_eutra_r16_l_ meas_results_per_carrier_list_idle_eutra_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultIdleNR-r16 ::= SEQUENCE
struct meas_result_idle_nr_r16_s {
  struct meas_result_serving_cell_r16_s_ {
    bool                           rsrp_result_r16_present = false;
    bool                           rsrq_result_r16_present = false;
    uint8_t                        rsrp_result_r16         = 0;
    uint8_t                        rsrq_result_r16         = 0;
    results_per_ssb_idx_list_r16_l results_ssb_idxes_r16;
  };
  using meas_results_per_carrier_list_idle_nr_r16_l_ = dyn_array<meas_results_per_carrier_idle_nr_r16_s>;

  // member variables
  bool                                         ext = false;
  meas_result_serving_cell_r16_s_              meas_result_serving_cell_r16;
  meas_results_per_carrier_list_idle_nr_r16_l_ meas_results_per_carrier_list_idle_nr_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultList2EUTRA ::= SEQUENCE (SIZE (1..8)) OF MeasResult2EUTRA-r16
using meas_result_list2_eutra_l = dyn_array<meas_result2_eutra_r16_s>;

// MeasResultList2NR ::= SEQUENCE (SIZE (1..8)) OF MeasResult2NR
using meas_result_list2_nr_l = dyn_array<meas_result2_nr_s>;

// MeasResultList2UTRA ::= SEQUENCE (SIZE (1..8)) OF MeasResult2UTRA-FDD-r16
using meas_result_list2_utra_l = dyn_array<meas_result2_utra_fdd_r16_s>;

// MeasResultRLFNR-r16 ::= SEQUENCE
struct meas_result_rlf_nr_r16_s {
  struct meas_result_r16_s_ {
    struct cell_results_r16_s_ {
      bool                 results_ssb_cell_r16_present    = false;
      bool                 results_csi_rs_cell_r16_present = false;
      meas_quant_results_s results_ssb_cell_r16;
      meas_quant_results_s results_csi_rs_cell_r16;
    };
    struct rs_idx_results_r16_s_ {
      bool                          ssb_rlm_cfg_bitmap_r16_present    = false;
      bool                          csi_rs_rlm_cfg_bitmap_r16_present = false;
      results_per_ssb_idx_list_l    results_ssb_idxes_r16;
      fixed_bitstring<64>           ssb_rlm_cfg_bitmap_r16;
      results_per_csi_rs_idx_list_l results_csi_rs_idxes_r16;
      fixed_bitstring<96>           csi_rs_rlm_cfg_bitmap_r16;
    };

    // member variables
    bool                  rs_idx_results_r16_present = false;
    cell_results_r16_s_   cell_results_r16;
    rs_idx_results_r16_s_ rs_idx_results_r16;
  };

  // member variables
  meas_result_r16_s_ meas_result_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultRxTxTimeDiff-r17 ::= SEQUENCE
struct meas_result_rx_tx_time_diff_r17_s {
  bool                  ext                            = false;
  bool                  rx_tx_time_diff_ue_r17_present = false;
  rx_tx_time_diff_r17_s rx_tx_time_diff_ue_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// MeasResultsSL-r16 ::= SEQUENCE
struct meas_results_sl_r16_s {
  struct meas_results_list_sl_r16_c_ {
    struct types_opts {
      enum options { meas_result_nr_sl_r16, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    types       type() const { return types::meas_result_nr_sl_r16; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    meas_result_nr_sl_r16_s&       meas_result_nr_sl_r16() { return c; }
    const meas_result_nr_sl_r16_s& meas_result_nr_sl_r16() const { return c; }

  private:
    meas_result_nr_sl_r16_s c;
  };

  // member variables
  bool                        ext = false;
  meas_results_list_sl_r16_c_ meas_results_list_sl_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-PRS-MeasurementInfoList-r16 ::= SEQUENCE (SIZE (1..4)) OF NR-PRS-MeasurementInfo-r16
using nr_prs_meas_info_list_r16_l = dyn_array<nr_prs_meas_info_r16_s>;

// OverheatingAssistance ::= SEQUENCE
struct overheat_assist_s {
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
  reduced_max_ccs_r16_s          reduced_max_ccs;
  reduced_max_bw_f_rx_r16_s      reduced_max_bw_fr1;
  reduced_max_bw_f_rx_r16_s      reduced_max_bw_fr2;
  reduced_max_mimo_layers_fr1_s_ reduced_max_mimo_layers_fr1;
  reduced_max_mimo_layers_fr2_s_ reduced_max_mimo_layers_fr2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RA-Report-r16 ::= SEQUENCE
struct ra_report_r16_s {
  struct cell_id_r16_c_ {
    struct types_opts {
      enum options { cell_global_id_r16, pci_arfcn_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    cell_id_r16_c_() = default;
    cell_id_r16_c_(const cell_id_r16_c_& other);
    cell_id_r16_c_& operator=(const cell_id_r16_c_& other);
    ~cell_id_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cgi_info_logging_r16_s& cell_global_id_r16()
    {
      assert_choice_type(types::cell_global_id_r16, type_, "cellId-r16");
      return c.get<cgi_info_logging_r16_s>();
    }
    pci_arfcn_nr_r16_s& pci_arfcn_r16()
    {
      assert_choice_type(types::pci_arfcn_r16, type_, "cellId-r16");
      return c.get<pci_arfcn_nr_r16_s>();
    }
    const cgi_info_logging_r16_s& cell_global_id_r16() const
    {
      assert_choice_type(types::cell_global_id_r16, type_, "cellId-r16");
      return c.get<cgi_info_logging_r16_s>();
    }
    const pci_arfcn_nr_r16_s& pci_arfcn_r16() const
    {
      assert_choice_type(types::pci_arfcn_r16, type_, "cellId-r16");
      return c.get<pci_arfcn_nr_r16_s>();
    }
    cgi_info_logging_r16_s& set_cell_global_id_r16();
    pci_arfcn_nr_r16_s&     set_pci_arfcn_r16();

  private:
    types                                                       type_;
    choice_buffer_t<cgi_info_logging_r16_s, pci_arfcn_nr_r16_s> c;

    void destroy_();
  };
  struct ra_purpose_r16_opts {
    enum options {
      access_related,
      beam_fail_recovery,
      recfg_with_sync,
      ul_un_synchronized,
      sched_request_fail,
      no_pucch_res_available,
      request_for_other_si,
      msg3_request_for_other_si_r17,
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
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ra_purpose_r16_e_ = enumerated<ra_purpose_r16_opts>;

  // member variables
  bool                 ext                        = false;
  bool                 ra_info_common_r16_present = false;
  cell_id_r16_c_       cell_id_r16;
  ra_info_common_r16_s ra_info_common_r16;
  ra_purpose_r16_e_    ra_purpose_r16;
  // ...
  // group 0
  copy_ptr<cgi_info_logging_r16_s> sp_cell_id_r17;

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
    using types = enumerated<types_opts>;

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
  bool                           scg_resp_present     = false;
  bool                           non_crit_ext_present = false;
  scg_resp_c_                    scg_resp;
  rrc_recfg_complete_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeComplete-v1640-IEs ::= SEQUENCE
struct rrc_resume_complete_v1640_ies_s {
  bool                                        non_crit_ext_present = false;
  ul_tx_direct_current_two_carrier_list_r16_l ul_tx_direct_current_two_carrier_list_r16;
  rrc_resume_complete_v1700_ies_s             non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetupComplete-v1690-IEs ::= SEQUENCE
struct rrc_setup_complete_v1690_ies_s {
  bool                           ul_rrc_segmentation_r16_present = false;
  bool                           non_crit_ext_present            = false;
  rrc_setup_complete_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-Failure-r16 ::= SEQUENCE
struct sl_fail_r16_s {
  struct sl_fail_r16_opts {
    enum options { rlf, cfg_fail, drx_reject_v1710, spare5, spare4, spare3, spare2, spare1, nulltype } value;

    const char* to_string() const;
  };
  using sl_fail_r16_e_ = enumerated<sl_fail_r16_opts>;

  // member variables
  fixed_bitstring<24> sl_dest_id_r16;
  sl_fail_r16_e_      sl_fail_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-InterestedFreqList-r16 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..8)
using sl_interested_freq_list_r16_l = bounded_array<uint8_t, 8>;

// SL-RxDRX-ReportList-v1700 ::= SEQUENCE (SIZE (1..32)) OF SL-RxDRX-Report-v1700
using sl_rx_drx_report_list_v1700_l = dyn_array<sl_rx_drx_report_v1700_s>;

// SL-RxInterestedGC-BC-DestList-r17 ::= SEQUENCE (SIZE (1..32)) OF SL-RxInterestedGC-BC-Dest-r17
using sl_rx_interested_gc_bc_dest_list_r17_l = dyn_array<sl_rx_interested_gc_bc_dest_r17_s>;

// SL-TxResourceReqList-v1700 ::= SEQUENCE (SIZE (1..32)) OF SL-TxResourceReq-v1700
using sl_tx_res_req_list_v1700_l = dyn_array<sl_tx_res_req_v1700_s>;

// SL-TxResourceReqListCommRelay-r17 ::= SEQUENCE (SIZE (1..32)) OF SL-TxResourceReqCommRelayInfo-r17
using sl_tx_res_req_list_comm_relay_r17_l = dyn_array<sl_tx_res_req_comm_relay_info_r17_s>;

// SL-TxResourceReqListDisc-r17 ::= SEQUENCE (SIZE (1..32)) OF SL-TxResourceReqDisc-r17
using sl_tx_res_req_list_disc_r17_l = dyn_array<sl_tx_res_req_disc_r17_s>;

// SuccessHO-Report-r17 ::= SEQUENCE
struct success_ho_report_r17_s {
  struct source_cell_info_r17_s_ {
    bool                            source_cell_meas_r17_present   = false;
    bool                            rlf_in_source_daps_r17_present = false;
    cgi_info_logging_r16_s          source_pcell_id_r17;
    meas_result_success_ho_nr_r17_s source_cell_meas_r17;
  };
  struct target_cell_info_r17_s_ {
    bool                            target_cell_meas_r17_present = false;
    cgi_info_logging_r16_s          target_pcell_id_r17;
    meas_result_success_ho_nr_r17_s target_cell_meas_r17;
  };
  struct meas_result_neigh_cells_r17_s_ {
    meas_result_list2_nr_r16_l    meas_result_list_nr_r17;
    meas_result_list2_eutra_r16_l meas_result_list_eutra_r17;
  };

  // member variables
  bool                           ext                                    = false;
  bool                           meas_result_neigh_cells_r17_present    = false;
  bool                           location_info_r17_present              = false;
  bool                           time_since_cho_recfg_r17_present       = false;
  bool                           shr_cause_r17_present                  = false;
  bool                           ra_info_common_r17_present             = false;
  bool                           up_interruption_time_at_ho_r17_present = false;
  bool                           c_rnti_r17_present                     = false;
  source_cell_info_r17_s_        source_cell_info_r17;
  target_cell_info_r17_s_        target_cell_info_r17;
  meas_result_neigh_cells_r17_s_ meas_result_neigh_cells_r17;
  location_info_r16_s            location_info_r17;
  uint16_t                       time_since_cho_recfg_r17 = 0;
  shr_cause_r17_s                shr_cause_r17;
  ra_info_common_r16_s           ra_info_common_r17;
  uint16_t                       up_interruption_time_at_ho_r17 = 0;
  uint32_t                       c_rnti_r17                     = 0;
  // ...

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

// UE-TxTEG-Association-r17 ::= SEQUENCE
struct ue_tx_teg_assoc_r17_s {
  using associated_srs_pos_res_id_list_r17_l_ = dyn_array<uint8_t>;

  // member variables
  bool                                  serv_cell_id_r17_present = false;
  uint8_t                               ue_tx_teg_id_r17         = 0;
  nr_time_stamp_r17_s                   nr_time_stamp_r17;
  associated_srs_pos_res_id_list_r17_l_ associated_srs_pos_res_id_list_r17;
  uint8_t                               serv_cell_id_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEAssistanceInformation-v1610-IEs ::= SEQUENCE
struct ue_assist_info_v1610_ies_s {
  bool                        idc_assist_r16_present            = false;
  bool                        drx_pref_r16_present              = false;
  bool                        max_bw_pref_r16_present           = false;
  bool                        max_cc_pref_r16_present           = false;
  bool                        max_mimo_layer_pref_r16_present   = false;
  bool                        min_sched_offset_pref_r16_present = false;
  bool                        release_pref_r16_present          = false;
  bool                        ref_time_info_pref_r16_present    = false;
  bool                        non_crit_ext_present              = false;
  idc_assist_r16_s            idc_assist_r16;
  drx_pref_r16_s              drx_pref_r16;
  max_bw_pref_r16_s           max_bw_pref_r16;
  max_cc_pref_r16_s           max_cc_pref_r16;
  max_mimo_layer_pref_r16_s   max_mimo_layer_pref_r16;
  min_sched_offset_pref_r16_s min_sched_offset_pref_r16;
  release_pref_r16_s          release_pref_r16;
  sl_ue_assist_info_nr_r16_l  sl_ue_assist_info_nr_r16;
  bool                        ref_time_info_pref_r16 = false;
  ue_assist_info_v1700_ies_s  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-PDCP-DelayValueResultList-r16 ::= SEQUENCE (SIZE (1..29)) OF UL-PDCP-DelayValueResult-r16
using ul_pdcp_delay_value_result_list_r16_l = dyn_array<ul_pdcp_delay_value_result_r16_s>;

// UL-PDCP-ExcessDelayResultList-r17 ::= SEQUENCE (SIZE (1..29)) OF UL-PDCP-ExcessDelayResult-r17
using ul_pdcp_excess_delay_result_list_r17_l = dyn_array<ul_pdcp_excess_delay_result_r17_s>;

// UplinkTxDirectCurrentList ::= SEQUENCE (SIZE (1..32)) OF UplinkTxDirectCurrentCell
using ul_tx_direct_current_list_l = dyn_array<ul_tx_direct_current_cell_s>;

// VisitedCellInfoList-r16 ::= SEQUENCE (SIZE (1..16)) OF VisitedCellInfo-r16
using visited_cell_info_list_r16_l = dyn_array<visited_cell_info_r16_s>;

// CarrierFreqListMBS-r17 ::= SEQUENCE (SIZE (1..16)) OF INTEGER (0..3279165)
using carrier_freq_list_mbs_r17_l = bounded_array<uint32_t, 16>;

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
  using type1_e_ = enumerated<type1_opts>;
  struct types_opts {
    enum options { type1, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts, true>;

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
  using fail_type_e_ = enumerated<fail_type_opts>;

  // member variables
  uint8_t      cell_group_id = 0;
  uint8_t      lc_ch_id      = 1;
  fail_type_e_ fail_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailureInformation-v1610-IEs ::= SEQUENCE
struct fail_info_v1610_ies_s {
  bool                 fail_info_daps_r16_present = false;
  bool                 non_crit_ext_present       = false;
  fail_info_daps_r16_s fail_info_daps_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailureReportMCG-r16 ::= SEQUENCE
struct fail_report_mcg_r16_s {
  struct fail_type_r16_opts {
    enum options {
      t310_expiry,
      random_access_problem,
      rlc_max_num_retx,
      t312_expiry_r16,
      lbt_fail_r16,
      beam_fail_recovery_fail_r16,
      bh_rlf_r16,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using fail_type_r16_e_ = enumerated<fail_type_r16_opts>;

  // member variables
  bool                      ext                   = false;
  bool                      fail_type_r16_present = false;
  fail_type_r16_e_          fail_type_r16;
  meas_result_list2_nr_l    meas_result_freq_list_r16;
  meas_result_list2_eutra_l meas_result_freq_list_eutra_r16;
  dyn_octstring             meas_result_scg_r16;
  dyn_octstring             meas_result_scg_eutra_r16;
  meas_result_list2_utra_l  meas_result_freq_list_utra_fdd_r16;
  // ...

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
      other_r16,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using fail_type_e_ = enumerated<fail_type_opts>;
  struct fail_type_v1610_opts {
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
  using fail_type_v1610_e_ = enumerated<fail_type_v1610_opts>;
  struct prev_pscell_id_r17_s_ {
    uint16_t pci_r17          = 0;
    uint32_t carrier_freq_r17 = 0;
  };
  struct failed_pscell_id_r17_s_ {
    uint16_t pci_r17          = 0;
    uint32_t carrier_freq_r17 = 0;
  };

  // member variables
  bool                    ext = false;
  fail_type_e_            fail_type;
  meas_result_freq_list_l meas_result_freq_list;
  dyn_octstring           meas_result_scg_fail;
  // ...
  // group 0
  bool                          fail_type_v1610_present = false;
  copy_ptr<location_info_r16_s> location_info_r16;
  fail_type_v1610_e_            fail_type_v1610;
  // group 1
  bool                              time_scg_fail_r17_present = false;
  copy_ptr<prev_pscell_id_r17_s_>   prev_pscell_id_r17;
  copy_ptr<failed_pscell_id_r17_s_> failed_pscell_id_r17;
  uint16_t                          time_scg_fail_r17 = 0;
  copy_ptr<per_ra_info_list_r16_l>  per_ra_info_list_r17;

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
  using fail_type_e_ = enumerated<fail_type_opts>;

  // member variables
  bool                              ext = false;
  fail_type_e_                      fail_type;
  meas_result_freq_list_fail_mrdc_l meas_result_freq_list_mrdc;
  dyn_octstring                     meas_result_scg_fail_mrdc;
  // ...
  // group 0
  copy_ptr<location_info_r16_s> location_info_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IAB-IP-AddressAndTraffic-r16 ::= SEQUENCE
struct iab_ip_address_and_traffic_r16_s {
  using all_traffic_iab_ip_address_r16_l_ = dyn_array<iab_ip_address_r16_c>;
  using f1_c_traffic_ip_address_r16_l_    = dyn_array<iab_ip_address_r16_c>;
  using f1_u_traffic_ip_address_r16_l_    = dyn_array<iab_ip_address_r16_c>;
  using non_f1_traffic_ip_address_r16_l_  = dyn_array<iab_ip_address_r16_c>;

  // member variables
  all_traffic_iab_ip_address_r16_l_ all_traffic_iab_ip_address_r16;
  f1_c_traffic_ip_address_r16_l_    f1_c_traffic_ip_address_r16;
  f1_u_traffic_ip_address_r16_l_    f1_u_traffic_ip_address_r16;
  non_f1_traffic_ip_address_r16_l_  non_f1_traffic_ip_address_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IAB-IP-AddressNumReq-r16 ::= SEQUENCE
struct iab_ip_address_num_req_r16_s {
  bool    ext                                = false;
  bool    all_traffic_num_req_r16_present    = false;
  bool    f1_c_traffic_num_req_r16_present   = false;
  bool    f1_u_traffic_num_req_r16_present   = false;
  bool    non_f1_traffic_num_req_r16_present = false;
  uint8_t all_traffic_num_req_r16            = 1;
  uint8_t f1_c_traffic_num_req_r16           = 1;
  uint8_t f1_u_traffic_num_req_r16           = 1;
  uint8_t non_f1_traffic_num_req_r16         = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IAB-IP-AddressPrefixReq-r16 ::= SEQUENCE
struct iab_ip_address_prefix_req_r16_s {
  bool ext                                   = false;
  bool all_traffic_prefix_req_r16_present    = false;
  bool f1_c_traffic_prefix_req_r16_present   = false;
  bool f1_u_traffic_prefix_req_r16_present   = false;
  bool non_f1_traffic_prefix_req_r16_present = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IAB-IP-PrefixAndTraffic-r16 ::= SEQUENCE
struct iab_ip_prefix_and_traffic_r16_s {
  bool                 all_traffic_iab_ip_address_r16_present = false;
  bool                 f1_c_traffic_ip_address_r16_present    = false;
  bool                 f1_u_traffic_ip_address_r16_present    = false;
  bool                 non_f1_traffic_ip_address_r16_present  = false;
  iab_ip_address_r16_c all_traffic_iab_ip_address_r16;
  iab_ip_address_r16_c f1_c_traffic_ip_address_r16;
  iab_ip_address_r16_c f1_u_traffic_ip_address_r16;
  iab_ip_address_r16_c non_f1_traffic_ip_address_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationMeasurementInfo ::= CHOICE
struct location_meas_info_c {
  struct types_opts {
    enum options { eutra_rstd, /*...*/ eutra_fine_timing_detection, nr_prs_meas_r16, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true, 2>;

  // choice methods
  location_meas_info_c() = default;
  location_meas_info_c(const location_meas_info_c& other);
  location_meas_info_c& operator=(const location_meas_info_c& other);
  ~location_meas_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_rstd_info_list_l& eutra_rstd()
  {
    assert_choice_type(types::eutra_rstd, type_, "LocationMeasurementInfo");
    return c.get<eutra_rstd_info_list_l>();
  }
  nr_prs_meas_info_list_r16_l& nr_prs_meas_r16()
  {
    assert_choice_type(types::nr_prs_meas_r16, type_, "LocationMeasurementInfo");
    return c.get<nr_prs_meas_info_list_r16_l>();
  }
  const eutra_rstd_info_list_l& eutra_rstd() const
  {
    assert_choice_type(types::eutra_rstd, type_, "LocationMeasurementInfo");
    return c.get<eutra_rstd_info_list_l>();
  }
  const nr_prs_meas_info_list_r16_l& nr_prs_meas_r16() const
  {
    assert_choice_type(types::nr_prs_meas_r16, type_, "LocationMeasurementInfo");
    return c.get<nr_prs_meas_info_list_r16_l>();
  }
  eutra_rstd_info_list_l&      set_eutra_rstd();
  void                         set_eutra_fine_timing_detection();
  nr_prs_meas_info_list_r16_l& set_nr_prs_meas_r16();

private:
  types                                                                type_;
  choice_buffer_t<eutra_rstd_info_list_l, nr_prs_meas_info_list_r16_l> c;

  void destroy_();
};

// LogMeasReport-r16 ::= SEQUENCE
struct log_meas_report_r16_s {
  bool                     ext                                 = false;
  bool                     log_meas_available_r16_present      = false;
  bool                     log_meas_available_bt_r16_present   = false;
  bool                     log_meas_available_wlan_r16_present = false;
  fixed_bitstring<48>      absolute_time_stamp_r16;
  trace_ref_r16_s          trace_ref_r16;
  fixed_octstring<2>       trace_recording_session_ref_r16;
  fixed_octstring<1>       tce_id_r16;
  log_meas_info_list_r16_l log_meas_info_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-ServiceList-r17 ::= SEQUENCE (SIZE (1..16)) OF MBS-ServiceInfo-r17
using mbs_service_list_r17_l = dyn_array<mbs_service_info_r17_s>;

// MeasResults ::= SEQUENCE
struct meas_results_s {
  struct meas_result_neigh_cells_c_ {
    struct types_opts {
      enum options {
        meas_result_list_nr,
        // ...
        meas_result_list_eutra,
        meas_result_list_utra_fdd_r16,
        sl_meas_results_cand_relay_r17,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true, 3>;

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
    meas_result_list_utra_fdd_r16_l& meas_result_list_utra_fdd_r16()
    {
      assert_choice_type(types::meas_result_list_utra_fdd_r16, type_, "measResultNeighCells");
      return c.get<meas_result_list_utra_fdd_r16_l>();
    }
    dyn_octstring& sl_meas_results_cand_relay_r17()
    {
      assert_choice_type(types::sl_meas_results_cand_relay_r17, type_, "measResultNeighCells");
      return c.get<dyn_octstring>();
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
    const meas_result_list_utra_fdd_r16_l& meas_result_list_utra_fdd_r16() const
    {
      assert_choice_type(types::meas_result_list_utra_fdd_r16, type_, "measResultNeighCells");
      return c.get<meas_result_list_utra_fdd_r16_l>();
    }
    const dyn_octstring& sl_meas_results_cand_relay_r17() const
    {
      assert_choice_type(types::sl_meas_results_cand_relay_r17, type_, "measResultNeighCells");
      return c.get<dyn_octstring>();
    }
    meas_result_list_nr_l&           set_meas_result_list_nr();
    meas_result_list_eutra_l&        set_meas_result_list_eutra();
    meas_result_list_utra_fdd_r16_l& set_meas_result_list_utra_fdd_r16();
    dyn_octstring&                   set_sl_meas_results_cand_relay_r17();

  private:
    types type_;
    choice_buffer_t<dyn_octstring, meas_result_list_eutra_l, meas_result_list_nr_l, meas_result_list_utra_fdd_r16_l> c;

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
  // group 2
  copy_ptr<meas_result_for_rssi_r16_s>            meas_result_for_rssi_r16;
  copy_ptr<location_info_r16_s>                   location_info_r16;
  copy_ptr<ul_pdcp_delay_value_result_list_r16_l> ul_pdcp_delay_value_result_list_r16;
  copy_ptr<meas_results_sl_r16_s>                 meas_results_sl_r16;
  copy_ptr<meas_result_cli_r16_s>                 meas_result_cli_r16;
  // group 3
  copy_ptr<meas_result_rx_tx_time_diff_r17_s>      meas_result_rx_tx_time_diff_r17;
  dyn_octstring                                    sl_meas_result_serving_relay_r17;
  copy_ptr<ul_pdcp_excess_delay_result_list_r17_l> ul_pdcp_excess_delay_result_list_r17;
  dyn_octstring                                    coarse_location_info_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityHistoryReport-r16 ::= VisitedCellInfoList-r16
using mob_history_report_r16_l = visited_cell_info_list_r16_l;

// PosSIB-ReqInfo-r16 ::= SEQUENCE
struct pos_sib_req_info_r16_s {
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
      pos_sib_type1_neg9_v1710,
      pos_sib_type1_neg10_v1710,
      pos_sib_type2_neg24_v1710,
      pos_sib_type2_neg25_v1710,
      pos_sib_type6_neg4_v1710,
      pos_sib_type6_neg5_v1710,
      pos_sib_type6_neg6_v1710,
      nulltype
    } value;

    const char* to_string() const;
  };
  using pos_sib_type_r16_e_ = enumerated<pos_sib_type_r16_opts, true, 7>;

  // member variables
  bool                gnss_id_r16_present = false;
  bool                sbas_id_r16_present = false;
  gnss_id_r16_s       gnss_id_r16;
  sbas_id_r16_s       sbas_id_r16;
  pos_sib_type_r16_e_ pos_sib_type_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RA-ReportList-r16 ::= SEQUENCE (SIZE (1..8)) OF RA-Report-r16
using ra_report_list_r16_l = dyn_array<ra_report_r16_s>;

// RLF-Report-r16 ::= CHOICE
struct rlf_report_r16_c {
  struct nr_rlf_report_r16_s_ {
    struct meas_result_neigh_cells_r16_s_ {
      meas_result_list2_nr_r16_l    meas_result_list_nr_r16;
      meas_result_list2_eutra_r16_l meas_result_list_eutra_r16;
    };
    struct prev_pcell_id_r16_c_ {
      struct types_opts {
        enum options { nr_prev_cell_r16, eutra_prev_cell_r16, nulltype } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      prev_pcell_id_r16_c_() = default;
      prev_pcell_id_r16_c_(const prev_pcell_id_r16_c_& other);
      prev_pcell_id_r16_c_& operator=(const prev_pcell_id_r16_c_& other);
      ~prev_pcell_id_r16_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      cgi_info_logging_r16_s& nr_prev_cell_r16()
      {
        assert_choice_type(types::nr_prev_cell_r16, type_, "previousPCellId-r16");
        return c.get<cgi_info_logging_r16_s>();
      }
      cgi_info_eutra_logging_s& eutra_prev_cell_r16()
      {
        assert_choice_type(types::eutra_prev_cell_r16, type_, "previousPCellId-r16");
        return c.get<cgi_info_eutra_logging_s>();
      }
      const cgi_info_logging_r16_s& nr_prev_cell_r16() const
      {
        assert_choice_type(types::nr_prev_cell_r16, type_, "previousPCellId-r16");
        return c.get<cgi_info_logging_r16_s>();
      }
      const cgi_info_eutra_logging_s& eutra_prev_cell_r16() const
      {
        assert_choice_type(types::eutra_prev_cell_r16, type_, "previousPCellId-r16");
        return c.get<cgi_info_eutra_logging_s>();
      }
      cgi_info_logging_r16_s&   set_nr_prev_cell_r16();
      cgi_info_eutra_logging_s& set_eutra_prev_cell_r16();

    private:
      types                                                             type_;
      choice_buffer_t<cgi_info_eutra_logging_s, cgi_info_logging_r16_s> c;

      void destroy_();
    };
    struct failed_pcell_id_r16_c_ {
      struct nr_failed_pcell_id_r16_c_ {
        struct types_opts {
          enum options { cell_global_id_r16, pci_arfcn_r16, nulltype } value;

          const char* to_string() const;
        };
        using types = enumerated<types_opts>;

        // choice methods
        nr_failed_pcell_id_r16_c_() = default;
        nr_failed_pcell_id_r16_c_(const nr_failed_pcell_id_r16_c_& other);
        nr_failed_pcell_id_r16_c_& operator=(const nr_failed_pcell_id_r16_c_& other);
        ~nr_failed_pcell_id_r16_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        cgi_info_logging_r16_s& cell_global_id_r16()
        {
          assert_choice_type(types::cell_global_id_r16, type_, "nrFailedPCellId-r16");
          return c.get<cgi_info_logging_r16_s>();
        }
        pci_arfcn_nr_r16_s& pci_arfcn_r16()
        {
          assert_choice_type(types::pci_arfcn_r16, type_, "nrFailedPCellId-r16");
          return c.get<pci_arfcn_nr_r16_s>();
        }
        const cgi_info_logging_r16_s& cell_global_id_r16() const
        {
          assert_choice_type(types::cell_global_id_r16, type_, "nrFailedPCellId-r16");
          return c.get<cgi_info_logging_r16_s>();
        }
        const pci_arfcn_nr_r16_s& pci_arfcn_r16() const
        {
          assert_choice_type(types::pci_arfcn_r16, type_, "nrFailedPCellId-r16");
          return c.get<pci_arfcn_nr_r16_s>();
        }
        cgi_info_logging_r16_s& set_cell_global_id_r16();
        pci_arfcn_nr_r16_s&     set_pci_arfcn_r16();

      private:
        types                                                       type_;
        choice_buffer_t<cgi_info_logging_r16_s, pci_arfcn_nr_r16_s> c;

        void destroy_();
      };
      struct eutra_failed_pcell_id_r16_c_ {
        struct types_opts {
          enum options { cell_global_id_r16, pci_arfcn_r16, nulltype } value;

          const char* to_string() const;
        };
        using types = enumerated<types_opts>;

        // choice methods
        eutra_failed_pcell_id_r16_c_() = default;
        eutra_failed_pcell_id_r16_c_(const eutra_failed_pcell_id_r16_c_& other);
        eutra_failed_pcell_id_r16_c_& operator=(const eutra_failed_pcell_id_r16_c_& other);
        ~eutra_failed_pcell_id_r16_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        cgi_info_eutra_logging_s& cell_global_id_r16()
        {
          assert_choice_type(types::cell_global_id_r16, type_, "eutraFailedPCellId-r16");
          return c.get<cgi_info_eutra_logging_s>();
        }
        pci_arfcn_eutra_r16_s& pci_arfcn_r16()
        {
          assert_choice_type(types::pci_arfcn_r16, type_, "eutraFailedPCellId-r16");
          return c.get<pci_arfcn_eutra_r16_s>();
        }
        const cgi_info_eutra_logging_s& cell_global_id_r16() const
        {
          assert_choice_type(types::cell_global_id_r16, type_, "eutraFailedPCellId-r16");
          return c.get<cgi_info_eutra_logging_s>();
        }
        const pci_arfcn_eutra_r16_s& pci_arfcn_r16() const
        {
          assert_choice_type(types::pci_arfcn_r16, type_, "eutraFailedPCellId-r16");
          return c.get<pci_arfcn_eutra_r16_s>();
        }
        cgi_info_eutra_logging_s& set_cell_global_id_r16();
        pci_arfcn_eutra_r16_s&    set_pci_arfcn_r16();

      private:
        types                                                            type_;
        choice_buffer_t<cgi_info_eutra_logging_s, pci_arfcn_eutra_r16_s> c;

        void destroy_();
      };
      struct types_opts {
        enum options { nr_failed_pcell_id_r16, eutra_failed_pcell_id_r16, nulltype } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      failed_pcell_id_r16_c_() = default;
      failed_pcell_id_r16_c_(const failed_pcell_id_r16_c_& other);
      failed_pcell_id_r16_c_& operator=(const failed_pcell_id_r16_c_& other);
      ~failed_pcell_id_r16_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      nr_failed_pcell_id_r16_c_& nr_failed_pcell_id_r16()
      {
        assert_choice_type(types::nr_failed_pcell_id_r16, type_, "failedPCellId-r16");
        return c.get<nr_failed_pcell_id_r16_c_>();
      }
      eutra_failed_pcell_id_r16_c_& eutra_failed_pcell_id_r16()
      {
        assert_choice_type(types::eutra_failed_pcell_id_r16, type_, "failedPCellId-r16");
        return c.get<eutra_failed_pcell_id_r16_c_>();
      }
      const nr_failed_pcell_id_r16_c_& nr_failed_pcell_id_r16() const
      {
        assert_choice_type(types::nr_failed_pcell_id_r16, type_, "failedPCellId-r16");
        return c.get<nr_failed_pcell_id_r16_c_>();
      }
      const eutra_failed_pcell_id_r16_c_& eutra_failed_pcell_id_r16() const
      {
        assert_choice_type(types::eutra_failed_pcell_id_r16, type_, "failedPCellId-r16");
        return c.get<eutra_failed_pcell_id_r16_c_>();
      }
      nr_failed_pcell_id_r16_c_&    set_nr_failed_pcell_id_r16();
      eutra_failed_pcell_id_r16_c_& set_eutra_failed_pcell_id_r16();

    private:
      types                                                                    type_;
      choice_buffer_t<eutra_failed_pcell_id_r16_c_, nr_failed_pcell_id_r16_c_> c;

      void destroy_();
    };
    struct reconnect_cell_id_r16_c_ {
      struct types_opts {
        enum options { nr_reconnect_cell_id_r16, eutra_reconnect_cell_id_r16, nulltype } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      reconnect_cell_id_r16_c_() = default;
      reconnect_cell_id_r16_c_(const reconnect_cell_id_r16_c_& other);
      reconnect_cell_id_r16_c_& operator=(const reconnect_cell_id_r16_c_& other);
      ~reconnect_cell_id_r16_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      cgi_info_logging_r16_s& nr_reconnect_cell_id_r16()
      {
        assert_choice_type(types::nr_reconnect_cell_id_r16, type_, "reconnectCellId-r16");
        return c.get<cgi_info_logging_r16_s>();
      }
      cgi_info_eutra_logging_s& eutra_reconnect_cell_id_r16()
      {
        assert_choice_type(types::eutra_reconnect_cell_id_r16, type_, "reconnectCellId-r16");
        return c.get<cgi_info_eutra_logging_s>();
      }
      const cgi_info_logging_r16_s& nr_reconnect_cell_id_r16() const
      {
        assert_choice_type(types::nr_reconnect_cell_id_r16, type_, "reconnectCellId-r16");
        return c.get<cgi_info_logging_r16_s>();
      }
      const cgi_info_eutra_logging_s& eutra_reconnect_cell_id_r16() const
      {
        assert_choice_type(types::eutra_reconnect_cell_id_r16, type_, "reconnectCellId-r16");
        return c.get<cgi_info_eutra_logging_s>();
      }
      cgi_info_logging_r16_s&   set_nr_reconnect_cell_id_r16();
      cgi_info_eutra_logging_s& set_eutra_reconnect_cell_id_r16();

    private:
      types                                                             type_;
      choice_buffer_t<cgi_info_eutra_logging_s, cgi_info_logging_r16_s> c;

      void destroy_();
    };
    struct conn_fail_type_r16_opts {
      enum options { rlf, hof, nulltype } value;

      const char* to_string() const;
    };
    using conn_fail_type_r16_e_ = enumerated<conn_fail_type_r16_opts>;
    struct rlf_cause_r16_opts {
      enum options {
        t310_expiry,
        random_access_problem,
        rlc_max_num_retx,
        beam_fail_recovery_fail,
        lbt_fail_r16,
        bh_rlf_recovery_fail,
        t312_expiry_r17,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using rlf_cause_r16_e_ = enumerated<rlf_cause_r16_opts>;
    struct last_ho_type_r17_opts {
      enum options { cho, daps, spare2, spare1, nulltype } value;

      const char* to_string() const;
    };
    using last_ho_type_r17_e_ = enumerated<last_ho_type_r17_opts>;
    struct cho_cell_id_r17_c_ {
      struct types_opts {
        enum options { cell_global_id_r17, pci_arfcn_r17, nulltype } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      cho_cell_id_r17_c_() = default;
      cho_cell_id_r17_c_(const cho_cell_id_r17_c_& other);
      cho_cell_id_r17_c_& operator=(const cho_cell_id_r17_c_& other);
      ~cho_cell_id_r17_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      cgi_info_logging_r16_s& cell_global_id_r17()
      {
        assert_choice_type(types::cell_global_id_r17, type_, "choCellId-r17");
        return c.get<cgi_info_logging_r16_s>();
      }
      pci_arfcn_nr_r16_s& pci_arfcn_r17()
      {
        assert_choice_type(types::pci_arfcn_r17, type_, "choCellId-r17");
        return c.get<pci_arfcn_nr_r16_s>();
      }
      const cgi_info_logging_r16_s& cell_global_id_r17() const
      {
        assert_choice_type(types::cell_global_id_r17, type_, "choCellId-r17");
        return c.get<cgi_info_logging_r16_s>();
      }
      const pci_arfcn_nr_r16_s& pci_arfcn_r17() const
      {
        assert_choice_type(types::pci_arfcn_r17, type_, "choCellId-r17");
        return c.get<pci_arfcn_nr_r16_s>();
      }
      cgi_info_logging_r16_s& set_cell_global_id_r17();
      pci_arfcn_nr_r16_s&     set_pci_arfcn_r17();

    private:
      types                                                       type_;
      choice_buffer_t<cgi_info_logging_r16_s, pci_arfcn_nr_r16_s> c;

      void destroy_();
    };

    // member variables
    bool                           ext                                 = false;
    bool                           meas_result_neigh_cells_r16_present = false;
    bool                           prev_pcell_id_r16_present           = false;
    bool                           reconnect_cell_id_r16_present       = false;
    bool                           time_until_reconn_r16_present       = false;
    bool                           reest_cell_id_r16_present           = false;
    bool                           time_conn_fail_r16_present          = false;
    bool                           location_info_r16_present           = false;
    bool                           no_suitable_cell_found_r16_present  = false;
    bool                           ra_info_common_r16_present          = false;
    meas_result_rlf_nr_r16_s       meas_result_last_serv_cell_r16;
    meas_result_neigh_cells_r16_s_ meas_result_neigh_cells_r16;
    uint32_t                       c_rnti_r16 = 0;
    prev_pcell_id_r16_c_           prev_pcell_id_r16;
    failed_pcell_id_r16_c_         failed_pcell_id_r16;
    reconnect_cell_id_r16_c_       reconnect_cell_id_r16;
    uint32_t                       time_until_reconn_r16 = 0;
    cgi_info_logging_r16_s         reest_cell_id_r16;
    uint16_t                       time_conn_fail_r16  = 0;
    uint32_t                       time_since_fail_r16 = 0;
    conn_fail_type_r16_e_          conn_fail_type_r16;
    rlf_cause_r16_e_               rlf_cause_r16;
    location_info_r16_s            location_info_r16;
    ra_info_common_r16_s           ra_info_common_r16;
    // ...
    // group 0
    bool                csi_rs_rlm_cfg_bitmap_v1650_present = false;
    fixed_bitstring<96> csi_rs_rlm_cfg_bitmap_v1650;
    // group 1
    bool                                    last_ho_type_r17_present               = false;
    bool                                    time_conn_source_daps_fail_r17_present = false;
    bool                                    time_since_cho_recfg_r17_present       = false;
    last_ho_type_r17_e_                     last_ho_type_r17;
    uint16_t                                time_conn_source_daps_fail_r17 = 0;
    uint16_t                                time_since_cho_recfg_r17       = 0;
    copy_ptr<cho_cell_id_r17_c_>            cho_cell_id_r17;
    copy_ptr<cho_candidate_cell_list_r17_l> cho_candidate_cell_list_r17;

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };
  struct eutra_rlf_report_r16_s_ {
    bool                     ext = false;
    cgi_info_eutra_logging_s failed_pcell_id_eutra;
    dyn_octstring            meas_result_rlf_report_eutra_r16;
    // ...
    // group 0
    dyn_octstring meas_result_rlf_report_eutra_v1690;

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };
  struct types_opts {
    enum options { nr_rlf_report_r16, eutra_rlf_report_r16, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  rlf_report_r16_c() = default;
  rlf_report_r16_c(const rlf_report_r16_c& other);
  rlf_report_r16_c& operator=(const rlf_report_r16_c& other);
  ~rlf_report_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  nr_rlf_report_r16_s_& nr_rlf_report_r16()
  {
    assert_choice_type(types::nr_rlf_report_r16, type_, "RLF-Report-r16");
    return c.get<nr_rlf_report_r16_s_>();
  }
  eutra_rlf_report_r16_s_& eutra_rlf_report_r16()
  {
    assert_choice_type(types::eutra_rlf_report_r16, type_, "RLF-Report-r16");
    return c.get<eutra_rlf_report_r16_s_>();
  }
  const nr_rlf_report_r16_s_& nr_rlf_report_r16() const
  {
    assert_choice_type(types::nr_rlf_report_r16, type_, "RLF-Report-r16");
    return c.get<nr_rlf_report_r16_s_>();
  }
  const eutra_rlf_report_r16_s_& eutra_rlf_report_r16() const
  {
    assert_choice_type(types::eutra_rlf_report_r16, type_, "RLF-Report-r16");
    return c.get<eutra_rlf_report_r16_s_>();
  }
  nr_rlf_report_r16_s_&    set_nr_rlf_report_r16();
  eutra_rlf_report_r16_s_& set_eutra_rlf_report_r16();

private:
  types                                                          type_;
  choice_buffer_t<eutra_rlf_report_r16_s_, nr_rlf_report_r16_s_> c;

  void destroy_();
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

// RRCReestablishmentComplete-v1610-IEs ::= SEQUENCE
struct rrc_reest_complete_v1610_ies_s {
  bool                     ue_meass_available_r16_present = false;
  bool                     non_crit_ext_present           = false;
  ue_meass_available_r16_s ue_meass_available_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeComplete-v1610-IEs ::= SEQUENCE
struct rrc_resume_complete_v1610_ies_s {
  struct scg_resp_r16_c_ {
    struct types_opts {
      enum options { nr_scg_resp, eutra_scg_resp, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    scg_resp_r16_c_() = default;
    scg_resp_r16_c_(const scg_resp_r16_c_& other);
    scg_resp_r16_c_& operator=(const scg_resp_r16_c_& other);
    ~scg_resp_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dyn_octstring& nr_scg_resp()
    {
      assert_choice_type(types::nr_scg_resp, type_, "scg-Response-r16");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& eutra_scg_resp()
    {
      assert_choice_type(types::eutra_scg_resp, type_, "scg-Response-r16");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& nr_scg_resp() const
    {
      assert_choice_type(types::nr_scg_resp, type_, "scg-Response-r16");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& eutra_scg_resp() const
    {
      assert_choice_type(types::eutra_scg_resp, type_, "scg-Response-r16");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& set_nr_scg_resp();
    dyn_octstring& set_eutra_scg_resp();

  private:
    types                          type_;
    choice_buffer_t<dyn_octstring> c;

    void destroy_();
  };
  struct mob_state_r16_opts {
    enum options { normal, medium, high, spare, nulltype } value;

    const char* to_string() const;
  };
  using mob_state_r16_e_ = enumerated<mob_state_r16_opts>;

  // member variables
  bool                            idle_meas_available_r16_present    = false;
  bool                            meas_result_idle_eutra_r16_present = false;
  bool                            meas_result_idle_nr_r16_present    = false;
  bool                            scg_resp_r16_present               = false;
  bool                            ue_meass_available_r16_present     = false;
  bool                            mob_history_avail_r16_present      = false;
  bool                            mob_state_r16_present              = false;
  bool                            need_for_gaps_info_nr_r16_present  = false;
  bool                            non_crit_ext_present               = false;
  meas_result_idle_eutra_r16_s    meas_result_idle_eutra_r16;
  meas_result_idle_nr_r16_s       meas_result_idle_nr_r16;
  scg_resp_r16_c_                 scg_resp_r16;
  ue_meass_available_r16_s        ue_meass_available_r16;
  mob_state_r16_e_                mob_state_r16;
  need_for_gaps_info_nr_r16_s     need_for_gaps_info_nr_r16;
  rrc_resume_complete_v1640_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCSetupComplete-v1610-IEs ::= SEQUENCE
struct rrc_setup_complete_v1610_ies_s {
  struct mob_state_r16_opts {
    enum options { normal, medium, high, spare, nulltype } value;

    const char* to_string() const;
  };
  using mob_state_r16_e_ = enumerated<mob_state_r16_opts>;

  // member variables
  bool                           iab_node_ind_r16_present        = false;
  bool                           idle_meas_available_r16_present = false;
  bool                           ue_meass_available_r16_present  = false;
  bool                           mob_history_avail_r16_present   = false;
  bool                           mob_state_r16_present           = false;
  bool                           non_crit_ext_present            = false;
  ue_meass_available_r16_s       ue_meass_available_r16;
  mob_state_r16_e_               mob_state_r16;
  rrc_setup_complete_v1690_ies_s non_crit_ext;

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
  using types = enumerated<types_opts>;

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
    return c.get<fixed_bitstring<8>>();
  }
  fixed_bitstring<32>& sst_sd()
  {
    assert_choice_type(types::sst_sd, type_, "S-NSSAI");
    return c.get<fixed_bitstring<32>>();
  }
  const fixed_bitstring<8>& sst() const
  {
    assert_choice_type(types::sst, type_, "S-NSSAI");
    return c.get<fixed_bitstring<8>>();
  }
  const fixed_bitstring<32>& sst_sd() const
  {
    assert_choice_type(types::sst_sd, type_, "S-NSSAI");
    return c.get<fixed_bitstring<32>>();
  }
  fixed_bitstring<8>&  set_sst();
  fixed_bitstring<32>& set_sst_sd();

private:
  types                                type_;
  choice_buffer_t<fixed_bitstring<32>> c;

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

// SIB-ReqInfo-r16 ::= ENUMERATED
struct sib_req_info_r16_opts {
  enum options { sib12, sib13, sib14, sib20_v1700, sib21_v1700, spare3, spare2, spare1, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using sib_req_info_r16_e = enumerated<sib_req_info_r16_opts>;

// SL-FailureList-r16 ::= SEQUENCE (SIZE (1..32)) OF SL-Failure-r16
using sl_fail_list_r16_l = dyn_array<sl_fail_r16_s>;

// SL-TxResourceReqList-r16 ::= SEQUENCE (SIZE (1..32)) OF SL-TxResourceReq-r16
using sl_tx_res_req_list_r16_l = dyn_array<sl_tx_res_req_r16_s>;

// SidelinkUEInformationNR-v1700-IEs ::= SEQUENCE
struct sidelink_ue_info_nr_v1700_ies_s {
  struct ue_type_r17_opts {
    enum options { relay_ue, remote_ue, nulltype } value;

    const char* to_string() const;
  };
  using ue_type_r17_e_ = enumerated<ue_type_r17_opts>;

  // member variables
  bool                                   ue_type_r17_present                = false;
  bool                                   sl_source_id_remote_ue_r17_present = false;
  bool                                   non_crit_ext_present               = false;
  sl_tx_res_req_list_v1700_l             sl_tx_res_req_list_v1700;
  sl_rx_drx_report_list_v1700_l          sl_rx_drx_report_list_v1700;
  sl_rx_interested_gc_bc_dest_list_r17_l sl_rx_interested_gc_bc_dest_list_r17;
  sl_interested_freq_list_r16_l          sl_rx_interested_freq_list_disc_r17;
  sl_tx_res_req_list_disc_r17_l          sl_tx_res_req_list_disc_r17;
  sl_tx_res_req_list_comm_relay_r17_l    sl_tx_res_req_list_comm_relay_r17;
  ue_type_r17_e_                         ue_type_r17;
  fixed_bitstring<24>                    sl_source_id_remote_ue_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRAT-ContainerList ::= SEQUENCE (SIZE (0..8)) OF UE-CapabilityRAT-Container
using ue_cap_rat_container_list_l = dyn_array<ue_cap_rat_container_s>;

// UE-TxTEG-AssociationList-r17 ::= SEQUENCE (SIZE (1..256)) OF UE-TxTEG-Association-r17
using ue_tx_teg_assoc_list_r17_l = dyn_array<ue_tx_teg_assoc_r17_s>;

// UEAssistanceInformation-v1540-IEs ::= SEQUENCE
struct ue_assist_info_v1540_ies_s {
  bool                       overheat_assist_present = false;
  bool                       non_crit_ext_present    = false;
  overheat_assist_s          overheat_assist;
  ue_assist_info_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationResponse-v1700-IEs ::= SEQUENCE
struct ue_info_resp_v1700_ies_s {
  bool                            success_ho_report_r17_present = false;
  bool                            non_crit_ext_present          = false;
  success_ho_report_r17_s         success_ho_report_r17;
  conn_est_fail_report_list_r17_l conn_est_fail_report_list_r17;
  dyn_octstring                   coarse_location_info_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEPositioningAssistanceInfo-v1720-IEs ::= SEQUENCE
struct ue_positioning_assist_info_v1720_ies_s {
  struct ue_tx_teg_timing_error_margin_value_r17_opts {
    enum options {
      tc0,
      tc2,
      tc4,
      tc6,
      tc8,
      tc12,
      tc16,
      tc20,
      tc24,
      tc32,
      tc40,
      tc48,
      tc56,
      tc64,
      tc72,
      tc80,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ue_tx_teg_timing_error_margin_value_r17_e_ = enumerated<ue_tx_teg_timing_error_margin_value_r17_opts>;

  // member variables
  bool                                       ue_tx_teg_timing_error_margin_value_r17_present = false;
  bool                                       non_crit_ext_present                            = false;
  ue_tx_teg_timing_error_margin_value_r17_e_ ue_tx_teg_timing_error_margin_value_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULInformationTransfer-v1700-IEs ::= SEQUENCE
struct ul_info_transfer_v1700_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring ded_info_f1c_r17;

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

// DedicatedSIBRequest-r16-IEs ::= SEQUENCE
struct ded_sib_request_r16_ies_s {
  struct on_demand_sib_request_list_r16_s_ {
    using requested_sib_list_r16_l_     = bounded_array<sib_req_info_r16_e, 8>;
    using requested_pos_sib_list_r16_l_ = dyn_array<pos_sib_req_info_r16_s>;

    // member variables
    requested_sib_list_r16_l_     requested_sib_list_r16;
    requested_pos_sib_list_r16_l_ requested_pos_sib_list_r16;
  };

  // member variables
  bool                              on_demand_sib_request_list_r16_present = false;
  bool                              non_crit_ext_present                   = false;
  on_demand_sib_request_list_r16_s_ on_demand_sib_request_list_r16;
  dyn_octstring                     late_non_crit_ext;

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
  fail_info_v1610_ies_s  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IABOtherInformation-r16-IEs ::= SEQUENCE
struct iab_other_info_r16_ies_s {
  struct ip_info_type_r16_c_ {
    struct iab_ip_request_r16_s_ {
      struct iab_ip_v6_address_req_r16_c_ {
        struct types_opts {
          enum options { iab_ip_v6_address_num_req_r16, iab_ip_v6_address_prefix_req_r16, /*...*/ nulltype } value;

          const char* to_string() const;
        };
        using types = enumerated<types_opts, true>;

        // choice methods
        iab_ip_v6_address_req_r16_c_() = default;
        iab_ip_v6_address_req_r16_c_(const iab_ip_v6_address_req_r16_c_& other);
        iab_ip_v6_address_req_r16_c_& operator=(const iab_ip_v6_address_req_r16_c_& other);
        ~iab_ip_v6_address_req_r16_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        iab_ip_address_num_req_r16_s& iab_ip_v6_address_num_req_r16()
        {
          assert_choice_type(types::iab_ip_v6_address_num_req_r16, type_, "iab-IPv6-AddressReq-r16");
          return c.get<iab_ip_address_num_req_r16_s>();
        }
        iab_ip_address_prefix_req_r16_s& iab_ip_v6_address_prefix_req_r16()
        {
          assert_choice_type(types::iab_ip_v6_address_prefix_req_r16, type_, "iab-IPv6-AddressReq-r16");
          return c.get<iab_ip_address_prefix_req_r16_s>();
        }
        const iab_ip_address_num_req_r16_s& iab_ip_v6_address_num_req_r16() const
        {
          assert_choice_type(types::iab_ip_v6_address_num_req_r16, type_, "iab-IPv6-AddressReq-r16");
          return c.get<iab_ip_address_num_req_r16_s>();
        }
        const iab_ip_address_prefix_req_r16_s& iab_ip_v6_address_prefix_req_r16() const
        {
          assert_choice_type(types::iab_ip_v6_address_prefix_req_r16, type_, "iab-IPv6-AddressReq-r16");
          return c.get<iab_ip_address_prefix_req_r16_s>();
        }
        iab_ip_address_num_req_r16_s&    set_iab_ip_v6_address_num_req_r16();
        iab_ip_address_prefix_req_r16_s& set_iab_ip_v6_address_prefix_req_r16();

      private:
        types                                                                          type_;
        choice_buffer_t<iab_ip_address_num_req_r16_s, iab_ip_address_prefix_req_r16_s> c;

        void destroy_();
      };

      // member variables
      bool                         iab_ip_v4_address_num_req_r16_present = false;
      bool                         iab_ip_v6_address_req_r16_present     = false;
      iab_ip_address_num_req_r16_s iab_ip_v4_address_num_req_r16;
      iab_ip_v6_address_req_r16_c_ iab_ip_v6_address_req_r16;
    };
    struct iab_ip_report_r16_s_ {
      struct iab_ip_v6_report_r16_c_ {
        struct types_opts {
          enum options { iab_ip_v6_address_report_r16, iab_ip_v6_prefix_report_r16, /*...*/ nulltype } value;

          const char* to_string() const;
        };
        using types = enumerated<types_opts, true>;

        // choice methods
        iab_ip_v6_report_r16_c_() = default;
        iab_ip_v6_report_r16_c_(const iab_ip_v6_report_r16_c_& other);
        iab_ip_v6_report_r16_c_& operator=(const iab_ip_v6_report_r16_c_& other);
        ~iab_ip_v6_report_r16_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        iab_ip_address_and_traffic_r16_s& iab_ip_v6_address_report_r16()
        {
          assert_choice_type(types::iab_ip_v6_address_report_r16, type_, "iab-IPv6-Report-r16");
          return c.get<iab_ip_address_and_traffic_r16_s>();
        }
        iab_ip_prefix_and_traffic_r16_s& iab_ip_v6_prefix_report_r16()
        {
          assert_choice_type(types::iab_ip_v6_prefix_report_r16, type_, "iab-IPv6-Report-r16");
          return c.get<iab_ip_prefix_and_traffic_r16_s>();
        }
        const iab_ip_address_and_traffic_r16_s& iab_ip_v6_address_report_r16() const
        {
          assert_choice_type(types::iab_ip_v6_address_report_r16, type_, "iab-IPv6-Report-r16");
          return c.get<iab_ip_address_and_traffic_r16_s>();
        }
        const iab_ip_prefix_and_traffic_r16_s& iab_ip_v6_prefix_report_r16() const
        {
          assert_choice_type(types::iab_ip_v6_prefix_report_r16, type_, "iab-IPv6-Report-r16");
          return c.get<iab_ip_prefix_and_traffic_r16_s>();
        }
        iab_ip_address_and_traffic_r16_s& set_iab_ip_v6_address_report_r16();
        iab_ip_prefix_and_traffic_r16_s&  set_iab_ip_v6_prefix_report_r16();

      private:
        types                                                                              type_;
        choice_buffer_t<iab_ip_address_and_traffic_r16_s, iab_ip_prefix_and_traffic_r16_s> c;

        void destroy_();
      };

      // member variables
      bool                             iab_ip_v4_address_report_r16_present = false;
      bool                             iab_ip_v6_report_r16_present         = false;
      iab_ip_address_and_traffic_r16_s iab_ip_v4_address_report_r16;
      iab_ip_v6_report_r16_c_          iab_ip_v6_report_r16;
    };
    struct types_opts {
      enum options { iab_ip_request_r16, iab_ip_report_r16, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    ip_info_type_r16_c_() = default;
    ip_info_type_r16_c_(const ip_info_type_r16_c_& other);
    ip_info_type_r16_c_& operator=(const ip_info_type_r16_c_& other);
    ~ip_info_type_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    iab_ip_request_r16_s_& iab_ip_request_r16()
    {
      assert_choice_type(types::iab_ip_request_r16, type_, "ip-InfoType-r16");
      return c.get<iab_ip_request_r16_s_>();
    }
    iab_ip_report_r16_s_& iab_ip_report_r16()
    {
      assert_choice_type(types::iab_ip_report_r16, type_, "ip-InfoType-r16");
      return c.get<iab_ip_report_r16_s_>();
    }
    const iab_ip_request_r16_s_& iab_ip_request_r16() const
    {
      assert_choice_type(types::iab_ip_request_r16, type_, "ip-InfoType-r16");
      return c.get<iab_ip_request_r16_s_>();
    }
    const iab_ip_report_r16_s_& iab_ip_report_r16() const
    {
      assert_choice_type(types::iab_ip_report_r16, type_, "ip-InfoType-r16");
      return c.get<iab_ip_report_r16_s_>();
    }
    iab_ip_request_r16_s_& set_iab_ip_request_r16();
    iab_ip_report_r16_s_&  set_iab_ip_report_r16();

  private:
    types                                                        type_;
    choice_buffer_t<iab_ip_report_r16_s_, iab_ip_request_r16_s_> c;

    void destroy_();
  };

  // member variables
  bool                non_crit_ext_present = false;
  ip_info_type_r16_c_ ip_info_type_r16;
  dyn_octstring       late_non_crit_ext;

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

// MBSInterestIndication-r17-IEs ::= SEQUENCE
struct mb_si_nterest_ind_r17_ies_s {
  bool                        mbs_prio_r17_present = false;
  bool                        non_crit_ext_present = false;
  carrier_freq_list_mbs_r17_l mbs_freq_list_r17;
  mbs_service_list_r17_l      mbs_service_list_r17;
  dyn_octstring               late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MCGFailureInformation-r16-IEs ::= SEQUENCE
struct mcg_fail_info_r16_ies_s {
  bool                  fail_report_mcg_r16_present = false;
  bool                  non_crit_ext_present        = false;
  fail_report_mcg_r16_s fail_report_mcg_r16;
  dyn_octstring         late_non_crit_ext;

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
  bool                           non_crit_ext_present = false;
  dyn_octstring                  late_non_crit_ext;
  rrc_reest_complete_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCResumeComplete-IEs ::= SEQUENCE
struct rrc_resume_complete_ies_s {
  bool                            sel_plmn_id_present  = false;
  bool                            non_crit_ext_present = false;
  dyn_octstring                   ded_nas_msg;
  uint8_t                         sel_plmn_id = 1;
  ul_tx_direct_current_list_l     ul_tx_direct_current_list;
  dyn_octstring                   late_non_crit_ext;
  rrc_resume_complete_v1610_ies_s non_crit_ext;

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
  using guami_type_e_   = enumerated<guami_type_opts>;
  using s_nssai_list_l_ = dyn_array<s_nssai_c>;
  struct ng_5_g_s_tmsi_value_c_ {
    struct types_opts {
      enum options { ng_5_g_s_tmsi, ng_5_g_s_tmsi_part2, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ng_5_g_s_tmsi_value_c_() = default;
    ng_5_g_s_tmsi_value_c_(const ng_5_g_s_tmsi_value_c_& other);
    ng_5_g_s_tmsi_value_c_& operator=(const ng_5_g_s_tmsi_value_c_& other);
    ~ng_5_g_s_tmsi_value_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<48>& ng_5_g_s_tmsi()
    {
      assert_choice_type(types::ng_5_g_s_tmsi, type_, "ng-5G-S-TMSI-Value");
      return c.get<fixed_bitstring<48>>();
    }
    fixed_bitstring<9>& ng_5_g_s_tmsi_part2()
    {
      assert_choice_type(types::ng_5_g_s_tmsi_part2, type_, "ng-5G-S-TMSI-Value");
      return c.get<fixed_bitstring<9>>();
    }
    const fixed_bitstring<48>& ng_5_g_s_tmsi() const
    {
      assert_choice_type(types::ng_5_g_s_tmsi, type_, "ng-5G-S-TMSI-Value");
      return c.get<fixed_bitstring<48>>();
    }
    const fixed_bitstring<9>& ng_5_g_s_tmsi_part2() const
    {
      assert_choice_type(types::ng_5_g_s_tmsi_part2, type_, "ng-5G-S-TMSI-Value");
      return c.get<fixed_bitstring<9>>();
    }
    fixed_bitstring<48>& set_ng_5_g_s_tmsi();
    fixed_bitstring<9>&  set_ng_5_g_s_tmsi_part2();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<48>> c;

    void destroy_();
  };

  // member variables
  bool                           registered_amf_present      = false;
  bool                           guami_type_present          = false;
  bool                           ng_5_g_s_tmsi_value_present = false;
  bool                           non_crit_ext_present        = false;
  uint8_t                        sel_plmn_id                 = 1;
  registered_amf_s               registered_amf;
  guami_type_e_                  guami_type;
  s_nssai_list_l_                s_nssai_list;
  dyn_octstring                  ded_nas_msg;
  ng_5_g_s_tmsi_value_c_         ng_5_g_s_tmsi_value;
  dyn_octstring                  late_non_crit_ext;
  rrc_setup_complete_v1610_ies_s non_crit_ext;

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

// SidelinkUEInformationNR-r16-IEs ::= SEQUENCE
struct sidelink_ue_info_nr_r16_ies_s {
  bool                            non_crit_ext_present = false;
  sl_interested_freq_list_r16_l   sl_rx_interested_freq_list_r16;
  sl_tx_res_req_list_r16_l        sl_tx_res_req_list_r16;
  sl_fail_list_r16_l              sl_fail_list_r16;
  dyn_octstring                   late_non_crit_ext;
  sidelink_ue_info_nr_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEAssistanceInformation-IEs ::= SEQUENCE
struct ue_assist_info_ies_s {
  bool                       delay_budget_report_present = false;
  bool                       non_crit_ext_present        = false;
  delay_budget_report_c      delay_budget_report;
  dyn_octstring              late_non_crit_ext;
  ue_assist_info_v1540_ies_s non_crit_ext;

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

// UEInformationResponse-r16-IEs ::= SEQUENCE
struct ue_info_resp_r16_ies_s {
  bool                         meas_result_idle_eutra_r16_present = false;
  bool                         meas_result_idle_nr_r16_present    = false;
  bool                         log_meas_report_r16_present        = false;
  bool                         conn_est_fail_report_r16_present   = false;
  bool                         rlf_report_r16_present             = false;
  bool                         non_crit_ext_present               = false;
  meas_result_idle_eutra_r16_s meas_result_idle_eutra_r16;
  meas_result_idle_nr_r16_s    meas_result_idle_nr_r16;
  log_meas_report_r16_s        log_meas_report_r16;
  conn_est_fail_report_r16_s   conn_est_fail_report_r16;
  ra_report_list_r16_l         ra_report_list_r16;
  rlf_report_r16_c             rlf_report_r16;
  mob_history_report_r16_l     mob_history_report_r16;
  dyn_octstring                late_non_crit_ext;
  ue_info_resp_v1700_ies_s     non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEPositioningAssistanceInfo-r17-IEs ::= SEQUENCE
struct ue_positioning_assist_info_r17_ies_s {
  bool                                   non_crit_ext_present = false;
  ue_tx_teg_assoc_list_r17_l             ue_tx_teg_assoc_list_r17;
  dyn_octstring                          late_non_crit_ext;
  ue_positioning_assist_info_v1720_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULDedicatedMessageSegment-r16-IEs ::= SEQUENCE
struct ul_ded_msg_segment_r16_ies_s {
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

// ULInformationTransfer-IEs ::= SEQUENCE
struct ul_info_transfer_ies_s {
  bool                         non_crit_ext_present = false;
  dyn_octstring                ded_nas_msg;
  dyn_octstring                late_non_crit_ext;
  ul_info_transfer_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULInformationTransferIRAT-r16-IEs ::= SEQUENCE
struct ul_info_transfer_irat_r16_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring ul_dcch_msg_eutra_r16;
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
    using types = enumerated<types_opts>;

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

// DedicatedSIBRequest-r16 ::= SEQUENCE
struct ded_sib_request_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ded_sib_request_r16, crit_exts_future, nulltype } value;

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
    ded_sib_request_r16_ies_s& ded_sib_request_r16()
    {
      assert_choice_type(types::ded_sib_request_r16, type_, "criticalExtensions");
      return c;
    }
    const ded_sib_request_r16_ies_s& ded_sib_request_r16() const
    {
      assert_choice_type(types::ded_sib_request_r16, type_, "criticalExtensions");
      return c;
    }
    ded_sib_request_r16_ies_s& set_ded_sib_request_r16();
    void                       set_crit_exts_future();

  private:
    types                     type_;
    ded_sib_request_r16_ies_s c;
  };

  // member variables
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
    using types = enumerated<types_opts>;

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

// IABOtherInformation-r16 ::= SEQUENCE
struct iab_other_info_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { iab_other_info_r16, crit_exts_future, nulltype } value;

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
    iab_other_info_r16_ies_s& iab_other_info_r16()
    {
      assert_choice_type(types::iab_other_info_r16, type_, "criticalExtensions");
      return c;
    }
    const iab_other_info_r16_ies_s& iab_other_info_r16() const
    {
      assert_choice_type(types::iab_other_info_r16, type_, "criticalExtensions");
      return c;
    }
    iab_other_info_r16_ies_s& set_iab_other_info_r16();
    void                      set_crit_exts_future();

  private:
    types                    type_;
    iab_other_info_r16_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
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
    using types = enumerated<types_opts>;

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

// MBSInterestIndication-r17 ::= SEQUENCE
struct mb_si_nterest_ind_r17_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { mbs_interest_ind_r17, crit_exts_future, nulltype } value;

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
    mb_si_nterest_ind_r17_ies_s& mbs_interest_ind_r17()
    {
      assert_choice_type(types::mbs_interest_ind_r17, type_, "criticalExtensions");
      return c;
    }
    const mb_si_nterest_ind_r17_ies_s& mbs_interest_ind_r17() const
    {
      assert_choice_type(types::mbs_interest_ind_r17, type_, "criticalExtensions");
      return c;
    }
    mb_si_nterest_ind_r17_ies_s& set_mbs_interest_ind_r17();
    void                         set_crit_exts_future();

  private:
    types                       type_;
    mb_si_nterest_ind_r17_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MCGFailureInformation-r16 ::= SEQUENCE
struct mcg_fail_info_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { mcg_fail_info_r16, crit_exts_future, nulltype } value;

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
    mcg_fail_info_r16_ies_s& mcg_fail_info_r16()
    {
      assert_choice_type(types::mcg_fail_info_r16, type_, "criticalExtensions");
      return c;
    }
    const mcg_fail_info_r16_ies_s& mcg_fail_info_r16() const
    {
      assert_choice_type(types::mcg_fail_info_r16, type_, "criticalExtensions");
      return c;
    }
    mcg_fail_info_r16_ies_s& set_mcg_fail_info_r16();
    void                     set_crit_exts_future();

  private:
    types                   type_;
    mcg_fail_info_r16_ies_s c;
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
    using types = enumerated<types_opts>;

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
    using types = enumerated<types_opts>;

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
    using types = enumerated<types_opts>;

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
    using types = enumerated<types_opts>;

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
    using types = enumerated<types_opts>;

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
    using types = enumerated<types_opts>;

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
    using types = enumerated<types_opts>;

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
    using types = enumerated<types_opts>;

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
    using types = enumerated<types_opts>;

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

// SidelinkUEInformationNR-r16 ::= SEQUENCE
struct sidelink_ue_info_nr_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { sidelink_ue_info_nr_r16, crit_exts_future, nulltype } value;

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
    sidelink_ue_info_nr_r16_ies_s& sidelink_ue_info_nr_r16()
    {
      assert_choice_type(types::sidelink_ue_info_nr_r16, type_, "criticalExtensions");
      return c;
    }
    const sidelink_ue_info_nr_r16_ies_s& sidelink_ue_info_nr_r16() const
    {
      assert_choice_type(types::sidelink_ue_info_nr_r16, type_, "criticalExtensions");
      return c;
    }
    sidelink_ue_info_nr_r16_ies_s& set_sidelink_ue_info_nr_r16();
    void                           set_crit_exts_future();

  private:
    types                         type_;
    sidelink_ue_info_nr_r16_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEAssistanceInformation ::= SEQUENCE
struct ue_assist_info_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_assist_info, crit_exts_future, nulltype } value;

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
    ue_assist_info_ies_s& ue_assist_info()
    {
      assert_choice_type(types::ue_assist_info, type_, "criticalExtensions");
      return c;
    }
    const ue_assist_info_ies_s& ue_assist_info() const
    {
      assert_choice_type(types::ue_assist_info, type_, "criticalExtensions");
      return c;
    }
    ue_assist_info_ies_s& set_ue_assist_info();
    void                  set_crit_exts_future();

  private:
    types                type_;
    ue_assist_info_ies_s c;
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
    using types = enumerated<types_opts>;

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

// UEInformationResponse-r16 ::= SEQUENCE
struct ue_info_resp_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_info_resp_r16, crit_exts_future, nulltype } value;

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
    ue_info_resp_r16_ies_s& ue_info_resp_r16()
    {
      assert_choice_type(types::ue_info_resp_r16, type_, "criticalExtensions");
      return c;
    }
    const ue_info_resp_r16_ies_s& ue_info_resp_r16() const
    {
      assert_choice_type(types::ue_info_resp_r16, type_, "criticalExtensions");
      return c;
    }
    ue_info_resp_r16_ies_s& set_ue_info_resp_r16();
    void                    set_crit_exts_future();

  private:
    types                  type_;
    ue_info_resp_r16_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEPositioningAssistanceInfo-r17 ::= SEQUENCE
struct ue_positioning_assist_info_r17_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_positioning_assist_info_r17, crit_exts_future, nulltype } value;

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
    ue_positioning_assist_info_r17_ies_s& ue_positioning_assist_info_r17()
    {
      assert_choice_type(types::ue_positioning_assist_info_r17, type_, "criticalExtensions");
      return c;
    }
    const ue_positioning_assist_info_r17_ies_s& ue_positioning_assist_info_r17() const
    {
      assert_choice_type(types::ue_positioning_assist_info_r17, type_, "criticalExtensions");
      return c;
    }
    ue_positioning_assist_info_r17_ies_s& set_ue_positioning_assist_info_r17();
    void                                  set_crit_exts_future();

  private:
    types                                type_;
    ue_positioning_assist_info_r17_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULDedicatedMessageSegment-r16 ::= SEQUENCE
struct ul_ded_msg_segment_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ul_ded_msg_segment_r16, crit_exts_future, nulltype } value;

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
    ul_ded_msg_segment_r16_ies_s& ul_ded_msg_segment_r16()
    {
      assert_choice_type(types::ul_ded_msg_segment_r16, type_, "criticalExtensions");
      return c;
    }
    const ul_ded_msg_segment_r16_ies_s& ul_ded_msg_segment_r16() const
    {
      assert_choice_type(types::ul_ded_msg_segment_r16, type_, "criticalExtensions");
      return c;
    }
    ul_ded_msg_segment_r16_ies_s& set_ul_ded_msg_segment_r16();
    void                          set_crit_exts_future();

  private:
    types                        type_;
    ul_ded_msg_segment_r16_ies_s c;
  };

  // member variables
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
    using types = enumerated<types_opts>;

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

// ULInformationTransferIRAT-r16 ::= SEQUENCE
struct ul_info_transfer_irat_r16_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ul_info_transfer_irat_r16, spare3, spare2, spare1, nulltype } value;

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
      ul_info_transfer_irat_r16_ies_s& ul_info_transfer_irat_r16()
      {
        assert_choice_type(types::ul_info_transfer_irat_r16, type_, "c1");
        return c;
      }
      const ul_info_transfer_irat_r16_ies_s& ul_info_transfer_irat_r16() const
      {
        assert_choice_type(types::ul_info_transfer_irat_r16, type_, "c1");
        return c;
      }
      ul_info_transfer_irat_r16_ies_s& set_ul_info_transfer_irat_r16();
      void                             set_spare3();
      void                             set_spare2();
      void                             set_spare1();

    private:
      types                           type_;
      ul_info_transfer_irat_r16_ies_s c;
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

// ULInformationTransferMRDC ::= SEQUENCE
struct ul_info_transfer_mrdc_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ul_info_transfer_mrdc, spare3, spare2, spare1, nulltype } value;

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
    ue_assist_info_s& ue_assist_info()
    {
      assert_choice_type(types::ue_assist_info, type_, "c1");
      return c.get<ue_assist_info_s>();
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
    const ue_assist_info_s& ue_assist_info() const
    {
      assert_choice_type(types::ue_assist_info, type_, "c1");
      return c.get<ue_assist_info_s>();
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
    ue_assist_info_s&         set_ue_assist_info();
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
                    ue_assist_info_s,
                    ue_cap_info_s,
                    ul_info_transfer_mrdc_s,
                    ul_info_transfer_s>
        c;

    void destroy_();
  };
  struct msg_class_ext_c_ {
    struct c2_c_ {
      struct types_opts {
        enum options {
          ul_ded_msg_segment_r16,
          ded_sib_request_r16,
          mcg_fail_info_r16,
          ue_info_resp_r16,
          sidelink_ue_info_nr_r16,
          ul_info_transfer_irat_r16,
          iab_other_info_r16,
          mbs_interest_ind_r17,
          ue_positioning_assist_info_r17,
          meas_report_app_layer_r17,
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
      ul_ded_msg_segment_r16_s& ul_ded_msg_segment_r16()
      {
        assert_choice_type(types::ul_ded_msg_segment_r16, type_, "c2");
        return c.get<ul_ded_msg_segment_r16_s>();
      }
      ded_sib_request_r16_s& ded_sib_request_r16()
      {
        assert_choice_type(types::ded_sib_request_r16, type_, "c2");
        return c.get<ded_sib_request_r16_s>();
      }
      mcg_fail_info_r16_s& mcg_fail_info_r16()
      {
        assert_choice_type(types::mcg_fail_info_r16, type_, "c2");
        return c.get<mcg_fail_info_r16_s>();
      }
      ue_info_resp_r16_s& ue_info_resp_r16()
      {
        assert_choice_type(types::ue_info_resp_r16, type_, "c2");
        return c.get<ue_info_resp_r16_s>();
      }
      sidelink_ue_info_nr_r16_s& sidelink_ue_info_nr_r16()
      {
        assert_choice_type(types::sidelink_ue_info_nr_r16, type_, "c2");
        return c.get<sidelink_ue_info_nr_r16_s>();
      }
      ul_info_transfer_irat_r16_s& ul_info_transfer_irat_r16()
      {
        assert_choice_type(types::ul_info_transfer_irat_r16, type_, "c2");
        return c.get<ul_info_transfer_irat_r16_s>();
      }
      iab_other_info_r16_s& iab_other_info_r16()
      {
        assert_choice_type(types::iab_other_info_r16, type_, "c2");
        return c.get<iab_other_info_r16_s>();
      }
      mb_si_nterest_ind_r17_s& mbs_interest_ind_r17()
      {
        assert_choice_type(types::mbs_interest_ind_r17, type_, "c2");
        return c.get<mb_si_nterest_ind_r17_s>();
      }
      ue_positioning_assist_info_r17_s& ue_positioning_assist_info_r17()
      {
        assert_choice_type(types::ue_positioning_assist_info_r17, type_, "c2");
        return c.get<ue_positioning_assist_info_r17_s>();
      }
      meas_report_app_layer_r17_s& meas_report_app_layer_r17()
      {
        assert_choice_type(types::meas_report_app_layer_r17, type_, "c2");
        return c.get<meas_report_app_layer_r17_s>();
      }
      const ul_ded_msg_segment_r16_s& ul_ded_msg_segment_r16() const
      {
        assert_choice_type(types::ul_ded_msg_segment_r16, type_, "c2");
        return c.get<ul_ded_msg_segment_r16_s>();
      }
      const ded_sib_request_r16_s& ded_sib_request_r16() const
      {
        assert_choice_type(types::ded_sib_request_r16, type_, "c2");
        return c.get<ded_sib_request_r16_s>();
      }
      const mcg_fail_info_r16_s& mcg_fail_info_r16() const
      {
        assert_choice_type(types::mcg_fail_info_r16, type_, "c2");
        return c.get<mcg_fail_info_r16_s>();
      }
      const ue_info_resp_r16_s& ue_info_resp_r16() const
      {
        assert_choice_type(types::ue_info_resp_r16, type_, "c2");
        return c.get<ue_info_resp_r16_s>();
      }
      const sidelink_ue_info_nr_r16_s& sidelink_ue_info_nr_r16() const
      {
        assert_choice_type(types::sidelink_ue_info_nr_r16, type_, "c2");
        return c.get<sidelink_ue_info_nr_r16_s>();
      }
      const ul_info_transfer_irat_r16_s& ul_info_transfer_irat_r16() const
      {
        assert_choice_type(types::ul_info_transfer_irat_r16, type_, "c2");
        return c.get<ul_info_transfer_irat_r16_s>();
      }
      const iab_other_info_r16_s& iab_other_info_r16() const
      {
        assert_choice_type(types::iab_other_info_r16, type_, "c2");
        return c.get<iab_other_info_r16_s>();
      }
      const mb_si_nterest_ind_r17_s& mbs_interest_ind_r17() const
      {
        assert_choice_type(types::mbs_interest_ind_r17, type_, "c2");
        return c.get<mb_si_nterest_ind_r17_s>();
      }
      const ue_positioning_assist_info_r17_s& ue_positioning_assist_info_r17() const
      {
        assert_choice_type(types::ue_positioning_assist_info_r17, type_, "c2");
        return c.get<ue_positioning_assist_info_r17_s>();
      }
      const meas_report_app_layer_r17_s& meas_report_app_layer_r17() const
      {
        assert_choice_type(types::meas_report_app_layer_r17, type_, "c2");
        return c.get<meas_report_app_layer_r17_s>();
      }
      ul_ded_msg_segment_r16_s&         set_ul_ded_msg_segment_r16();
      ded_sib_request_r16_s&            set_ded_sib_request_r16();
      mcg_fail_info_r16_s&              set_mcg_fail_info_r16();
      ue_info_resp_r16_s&               set_ue_info_resp_r16();
      sidelink_ue_info_nr_r16_s&        set_sidelink_ue_info_nr_r16();
      ul_info_transfer_irat_r16_s&      set_ul_info_transfer_irat_r16();
      iab_other_info_r16_s&             set_iab_other_info_r16();
      mb_si_nterest_ind_r17_s&          set_mbs_interest_ind_r17();
      ue_positioning_assist_info_r17_s& set_ue_positioning_assist_info_r17();
      meas_report_app_layer_r17_s&      set_meas_report_app_layer_r17();
      void                              set_spare6();
      void                              set_spare5();
      void                              set_spare4();
      void                              set_spare3();
      void                              set_spare2();
      void                              set_spare1();

    private:
      types type_;
      choice_buffer_t<ded_sib_request_r16_s,
                      iab_other_info_r16_s,
                      mb_si_nterest_ind_r17_s,
                      mcg_fail_info_r16_s,
                      meas_report_app_layer_r17_s,
                      sidelink_ue_info_nr_r16_s,
                      ue_info_resp_r16_s,
                      ue_positioning_assist_info_r17_s,
                      ul_ded_msg_segment_r16_s,
                      ul_info_transfer_irat_r16_s>
          c;

      void destroy_();
    };
    struct types_opts {
      enum options { c2, msg_class_ext_future_r16, nulltype } value;
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
    void   set_msg_class_ext_future_r16();

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
  ul_dcch_msg_type_c() = default;
  ul_dcch_msg_type_c(const ul_dcch_msg_type_c& other);
  ul_dcch_msg_type_c& operator=(const ul_dcch_msg_type_c& other);
  ~ul_dcch_msg_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "UL-DCCH-MessageType");
    return c.get<c1_c_>();
  }
  msg_class_ext_c_& msg_class_ext()
  {
    assert_choice_type(types::msg_class_ext, type_, "UL-DCCH-MessageType");
    return c.get<msg_class_ext_c_>();
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "UL-DCCH-MessageType");
    return c.get<c1_c_>();
  }
  const msg_class_ext_c_& msg_class_ext() const
  {
    assert_choice_type(types::msg_class_ext, type_, "UL-DCCH-MessageType");
    return c.get<msg_class_ext_c_>();
  }
  c1_c_&            set_c1();
  msg_class_ext_c_& set_msg_class_ext();

private:
  types                                    type_;
  choice_buffer_t<c1_c_, msg_class_ext_c_> c;

  void destroy_();
};

// UL-DCCH-Message ::= SEQUENCE
struct ul_dcch_msg_s {
  ul_dcch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1

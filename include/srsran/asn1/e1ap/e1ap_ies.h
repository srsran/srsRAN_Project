/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
 *                     3GPP TS ASN1 E1AP v18.5.0 (2025-06)
 *
 ******************************************************************************/

#pragma once

#include "srsran/asn1/asn1_ap_utils.h"

namespace asn1 {
namespace e1ap {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// DRB-Activity ::= ENUMERATED
struct drb_activity_opts {
  enum options { active, not_active, /*...*/ nulltype } value;

  const char* to_string() const;
};
using drb_activity_e = enumerated<drb_activity_opts, true>;

// DRB-Activity-ItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_activity_item_ext_ies_o = protocol_ext_empty_o;

// PDU-Session-Resource-Activity ::= ENUMERATED
struct pdu_session_res_activity_opts {
  enum options { active, not_active, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdu_session_res_activity_e = enumerated<pdu_session_res_activity_opts, true>;

// PDU-Session-Resource-Activity-ItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdu_session_res_activity_item_ext_ies_o = protocol_ext_empty_o;

using drb_activity_item_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Activity-Item ::= SEQUENCE
struct drb_activity_item_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  uint8_t                             drb_id          = 1;
  drb_activity_e                      drb_activity;
  drb_activity_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_activity_item_ext_ies_container = protocol_ext_container_empty_l;

// PDU-Session-Resource-Activity-Item ::= SEQUENCE
struct pdu_session_res_activity_item_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  uint16_t                                        pdu_session_id  = 0;
  pdu_session_res_activity_e                      pdu_session_res_activity;
  pdu_session_res_activity_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ActivityInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using activity_info_ext_ies_o = protocol_ies_empty_o;

// DRB-Activity-List ::= SEQUENCE (SIZE (1..32)) OF DRB-Activity-Item
using drb_activity_list_l = dyn_array<drb_activity_item_s>;

// PDU-Session-Resource-Activity-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Activity-Item
using pdu_session_res_activity_list_l = dyn_array<pdu_session_res_activity_item_s>;

// UE-Activity ::= ENUMERATED
struct ue_activity_opts {
  enum options { active, not_active, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ue_activity_e = enumerated<ue_activity_opts, true>;

// ActivityInformation ::= CHOICE
struct activity_info_c {
  struct types_opts {
    enum options { drb_activity_list, pdu_session_res_activity_list, ue_activity, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  activity_info_c() = default;
  activity_info_c(const activity_info_c& other);
  activity_info_c& operator=(const activity_info_c& other);
  ~activity_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  drb_activity_list_l& drb_activity_list()
  {
    assert_choice_type(types::drb_activity_list, type_, "ActivityInformation");
    return c.get<drb_activity_list_l>();
  }
  pdu_session_res_activity_list_l& pdu_session_res_activity_list()
  {
    assert_choice_type(types::pdu_session_res_activity_list, type_, "ActivityInformation");
    return c.get<pdu_session_res_activity_list_l>();
  }
  ue_activity_e& ue_activity()
  {
    assert_choice_type(types::ue_activity, type_, "ActivityInformation");
    return c.get<ue_activity_e>();
  }
  protocol_ie_single_container_s<activity_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "ActivityInformation");
    return c.get<protocol_ie_single_container_s<activity_info_ext_ies_o>>();
  }
  const drb_activity_list_l& drb_activity_list() const
  {
    assert_choice_type(types::drb_activity_list, type_, "ActivityInformation");
    return c.get<drb_activity_list_l>();
  }
  const pdu_session_res_activity_list_l& pdu_session_res_activity_list() const
  {
    assert_choice_type(types::pdu_session_res_activity_list, type_, "ActivityInformation");
    return c.get<pdu_session_res_activity_list_l>();
  }
  const ue_activity_e& ue_activity() const
  {
    assert_choice_type(types::ue_activity, type_, "ActivityInformation");
    return c.get<ue_activity_e>();
  }
  const protocol_ie_single_container_s<activity_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "ActivityInformation");
    return c.get<protocol_ie_single_container_s<activity_info_ext_ies_o>>();
  }
  drb_activity_list_l&                                     set_drb_activity_list();
  pdu_session_res_activity_list_l&                         set_pdu_session_res_activity_list();
  ue_activity_e&                                           set_ue_activity();
  protocol_ie_single_container_s<activity_info_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<drb_activity_list_l,
                  pdu_session_res_activity_list_l,
                  protocol_ie_single_container_s<activity_info_ext_ies_o>>
      c;

  void destroy_();
};

// PacketErrorRate-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using packet_error_rate_ext_ies_o = protocol_ext_empty_o;

// AlternativeQoSParaSetItem-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct alt_qos_para_set_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { max_data_burst_volume, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::max_data_burst_volume; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&       max_data_burst_volume() { return c; }
    const uint16_t& max_data_burst_volume() const { return c; }

  private:
    uint16_t c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using packet_error_rate_ext_ies_container = protocol_ext_container_empty_l;

// PacketErrorRate ::= SEQUENCE
struct packet_error_rate_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  uint8_t                             per_scalar      = 0;
  uint8_t                             per_exponent    = 0;
  packet_error_rate_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AlternativeQoSParaSetItem ::= SEQUENCE
struct alt_qos_para_set_item_s {
  bool                                                      ext                                 = false;
  bool                                                      guaranteed_flow_bit_rate_dl_present = false;
  bool                                                      guaranteed_flow_bit_rate_ul_present = false;
  bool                                                      packet_delay_budget_present         = false;
  bool                                                      packet_error_rate_present           = false;
  uint8_t                                                   alt_qos_param_idx                   = 1;
  uint64_t                                                  guaranteed_flow_bit_rate_dl         = 0;
  uint64_t                                                  guaranteed_flow_bit_rate_ul         = 0;
  uint16_t                                                  packet_delay_budget                 = 0;
  packet_error_rate_s                                       packet_error_rate;
  protocol_ext_container_l<alt_qos_para_set_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AlternativeQoSParaSetList ::= SEQUENCE (SIZE (1..8)) OF AlternativeQoSParaSetItem
using alt_qos_para_set_list_l = dyn_array<alt_qos_para_set_item_s>;

// GTPTunnel-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gtp_tunnel_ext_ies_o = protocol_ext_empty_o;

using gtp_tunnel_ext_ies_container = protocol_ext_container_empty_l;

// GTPTunnel ::= SEQUENCE
struct gtp_tunnel_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  bounded_bitstring<1, 160, true, true> transport_layer_address;
  fixed_octstring<4, true>              gtp_teid;
  gtp_tunnel_ext_ies_container          ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UP-TNL-Information-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using up_tnl_info_ext_ies_o = protocol_ies_empty_o;

// LocationDependentMBSF1UInformationAtCU-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using location_dependent_mbsf1_u_info_at_cu_item_ext_ies_o = protocol_ext_empty_o;

// UP-TNL-Information ::= CHOICE
struct up_tnl_info_c {
  struct types_opts {
    enum options { gtp_tunnel, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  up_tnl_info_c() = default;
  up_tnl_info_c(const up_tnl_info_c& other);
  up_tnl_info_c& operator=(const up_tnl_info_c& other);
  ~up_tnl_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  gtp_tunnel_s& gtp_tunnel()
  {
    assert_choice_type(types::gtp_tunnel, type_, "UP-TNL-Information");
    return c.get<gtp_tunnel_s>();
  }
  protocol_ie_single_container_s<up_tnl_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "UP-TNL-Information");
    return c.get<protocol_ie_single_container_s<up_tnl_info_ext_ies_o>>();
  }
  const gtp_tunnel_s& gtp_tunnel() const
  {
    assert_choice_type(types::gtp_tunnel, type_, "UP-TNL-Information");
    return c.get<gtp_tunnel_s>();
  }
  const protocol_ie_single_container_s<up_tnl_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "UP-TNL-Information");
    return c.get<protocol_ie_single_container_s<up_tnl_info_ext_ies_o>>();
  }
  gtp_tunnel_s&                                          set_gtp_tunnel();
  protocol_ie_single_container_s<up_tnl_info_ext_ies_o>& set_choice_ext();

private:
  types                                                                                type_;
  choice_buffer_t<gtp_tunnel_s, protocol_ie_single_container_s<up_tnl_info_ext_ies_o>> c;

  void destroy_();
};

using location_dependent_mbsf1_u_info_at_cu_item_ext_ies_container = protocol_ext_container_empty_l;

// LocationDependentMBSF1UInformationAtCU-Item ::= SEQUENCE
struct location_dependent_mbsf1_u_info_at_cu_item_s {
  bool                                                         ext                 = false;
  bool                                                         ie_exts_present     = false;
  uint32_t                                                     mbs_area_session_id = 0;
  up_tnl_info_c                                                mbs_f1u_info_at_cu;
  location_dependent_mbsf1_u_info_at_cu_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSF1UInformationAtCU-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mbsf1_u_info_at_cu_ext_ies_o = protocol_ext_empty_o;

// BCBearerContextF1U-TNLInfoatCU-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using bc_bearer_context_f1_u_tnl_infoat_cu_ext_ies_o = protocol_ies_empty_o;

// LocationDependentMBSF1UInformationAtCU ::= SEQUENCE (SIZE (1..256)) OF LocationDependentMBSF1UInformationAtCU-Item
using location_dependent_mbsf1_u_info_at_cu_l = dyn_array<location_dependent_mbsf1_u_info_at_cu_item_s>;

using mbsf1_u_info_at_cu_ext_ies_container = protocol_ext_container_empty_l;

// MBSF1UInformationAtCU ::= SEQUENCE
struct mbsf1_u_info_at_cu_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  up_tnl_info_c                        mbs_f1u_info_at_cu;
  mbsf1_u_info_at_cu_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCBearerContextF1U-TNLInfoatCU ::= CHOICE
struct bc_bearer_context_f1_u_tnl_infoat_cu_c {
  struct types_opts {
    enum options { locationindependent, locationdependent, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  bc_bearer_context_f1_u_tnl_infoat_cu_c() = default;
  bc_bearer_context_f1_u_tnl_infoat_cu_c(const bc_bearer_context_f1_u_tnl_infoat_cu_c& other);
  bc_bearer_context_f1_u_tnl_infoat_cu_c& operator=(const bc_bearer_context_f1_u_tnl_infoat_cu_c& other);
  ~bc_bearer_context_f1_u_tnl_infoat_cu_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  mbsf1_u_info_at_cu_s& locationindependent()
  {
    assert_choice_type(types::locationindependent, type_, "BCBearerContextF1U-TNLInfoatCU");
    return c.get<mbsf1_u_info_at_cu_s>();
  }
  location_dependent_mbsf1_u_info_at_cu_l& locationdependent()
  {
    assert_choice_type(types::locationdependent, type_, "BCBearerContextF1U-TNLInfoatCU");
    return c.get<location_dependent_mbsf1_u_info_at_cu_l>();
  }
  protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_cu_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "BCBearerContextF1U-TNLInfoatCU");
    return c.get<protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_cu_ext_ies_o>>();
  }
  const mbsf1_u_info_at_cu_s& locationindependent() const
  {
    assert_choice_type(types::locationindependent, type_, "BCBearerContextF1U-TNLInfoatCU");
    return c.get<mbsf1_u_info_at_cu_s>();
  }
  const location_dependent_mbsf1_u_info_at_cu_l& locationdependent() const
  {
    assert_choice_type(types::locationdependent, type_, "BCBearerContextF1U-TNLInfoatCU");
    return c.get<location_dependent_mbsf1_u_info_at_cu_l>();
  }
  const protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_cu_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "BCBearerContextF1U-TNLInfoatCU");
    return c.get<protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_cu_ext_ies_o>>();
  }
  mbsf1_u_info_at_cu_s&                                                           set_locationindependent();
  location_dependent_mbsf1_u_info_at_cu_l&                                        set_locationdependent();
  protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_cu_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<location_dependent_mbsf1_u_info_at_cu_l,
                  mbsf1_u_info_at_cu_s,
                  protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_cu_ext_ies_o>>
      c;

  void destroy_();
};

// F1UTunnelNotEstablished ::= ENUMERATED
struct f1_u_tunnel_not_established_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using f1_u_tunnel_not_established_e = enumerated<f1_u_tunnel_not_established_opts, true>;

// LocationDependentMBSF1UInformationAtDU-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct location_dependent_mbsf1_u_info_at_du_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { f1_u_tunnel_not_established, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::f1_u_tunnel_not_established; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    f1_u_tunnel_not_established_e&       f1_u_tunnel_not_established() { return c; }
    const f1_u_tunnel_not_established_e& f1_u_tunnel_not_established() const { return c; }

  private:
    f1_u_tunnel_not_established_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// LocationDependentMBSF1UInformationAtDU-Item ::= SEQUENCE
struct location_dependent_mbsf1_u_info_at_du_item_s {
  bool                                                                           ext                 = false;
  uint32_t                                                                       mbs_area_session_id = 0;
  up_tnl_info_c                                                                  mbs_f1u_info_at_du;
  protocol_ext_container_l<location_dependent_mbsf1_u_info_at_du_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSF1UInformationAtDU-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct mbsf1_u_info_at_du_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { f1_u_tunnel_not_established, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::f1_u_tunnel_not_established; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    f1_u_tunnel_not_established_e&       f1_u_tunnel_not_established() { return c; }
    const f1_u_tunnel_not_established_e& f1_u_tunnel_not_established() const { return c; }

  private:
    f1_u_tunnel_not_established_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BCBearerContextF1U-TNLInfoatDU-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using bc_bearer_context_f1_u_tnl_infoat_du_ext_ies_o = protocol_ies_empty_o;

// LocationDependentMBSF1UInformationAtDU ::= SEQUENCE (SIZE (1..256)) OF LocationDependentMBSF1UInformationAtDU-Item
using location_dependent_mbsf1_u_info_at_du_l = dyn_array<location_dependent_mbsf1_u_info_at_du_item_s>;

// MBSF1UInformationAtDU ::= SEQUENCE
struct mbsf1_u_info_at_du_s {
  bool                                                   ext = false;
  up_tnl_info_c                                          mbs_f1u_info_at_du;
  protocol_ext_container_l<mbsf1_u_info_at_du_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCBearerContextF1U-TNLInfoatDU ::= CHOICE
struct bc_bearer_context_f1_u_tnl_infoat_du_c {
  struct types_opts {
    enum options { locationindependent, locationdependent, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  bc_bearer_context_f1_u_tnl_infoat_du_c() = default;
  bc_bearer_context_f1_u_tnl_infoat_du_c(const bc_bearer_context_f1_u_tnl_infoat_du_c& other);
  bc_bearer_context_f1_u_tnl_infoat_du_c& operator=(const bc_bearer_context_f1_u_tnl_infoat_du_c& other);
  ~bc_bearer_context_f1_u_tnl_infoat_du_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  mbsf1_u_info_at_du_s& locationindependent()
  {
    assert_choice_type(types::locationindependent, type_, "BCBearerContextF1U-TNLInfoatDU");
    return c.get<mbsf1_u_info_at_du_s>();
  }
  location_dependent_mbsf1_u_info_at_du_l& locationdependent()
  {
    assert_choice_type(types::locationdependent, type_, "BCBearerContextF1U-TNLInfoatDU");
    return c.get<location_dependent_mbsf1_u_info_at_du_l>();
  }
  protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_du_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "BCBearerContextF1U-TNLInfoatDU");
    return c.get<protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_du_ext_ies_o>>();
  }
  const mbsf1_u_info_at_du_s& locationindependent() const
  {
    assert_choice_type(types::locationindependent, type_, "BCBearerContextF1U-TNLInfoatDU");
    return c.get<mbsf1_u_info_at_du_s>();
  }
  const location_dependent_mbsf1_u_info_at_du_l& locationdependent() const
  {
    assert_choice_type(types::locationdependent, type_, "BCBearerContextF1U-TNLInfoatDU");
    return c.get<location_dependent_mbsf1_u_info_at_du_l>();
  }
  const protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_du_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "BCBearerContextF1U-TNLInfoatDU");
    return c.get<protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_du_ext_ies_o>>();
  }
  mbsf1_u_info_at_du_s&                                                           set_locationindependent();
  location_dependent_mbsf1_u_info_at_du_l&                                        set_locationdependent();
  protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_du_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<location_dependent_mbsf1_u_info_at_du_l,
                  mbsf1_u_info_at_du_s,
                  protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_du_ext_ies_o>>
      c;

  void destroy_();
};

// CriticalityDiagnostics-IE-List-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using crit_diagnostics_ie_list_ext_ies_o = protocol_ext_empty_o;

// TypeOfError ::= ENUMERATED
struct type_of_error_opts {
  enum options { not_understood, missing, /*...*/ nulltype } value;

  const char* to_string() const;
};
using type_of_error_e = enumerated<type_of_error_opts, true>;

// BCBearerContextToModifyConfirm-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using bc_bearer_context_to_modify_confirm_ext_ies_o = protocol_ext_empty_o;

// CriticalityDiagnostics-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using crit_diagnostics_ext_ies_o = protocol_ext_empty_o;

using crit_diagnostics_ie_list_ext_ies_container = protocol_ext_container_empty_l;

struct crit_diagnostics_ie_list_item_s_ {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  crit_e                                     ie_crit;
  uint32_t                                   ie_id = 0;
  type_of_error_e                            type_of_error;
  crit_diagnostics_ie_list_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CriticalityDiagnostics-IE-List ::= SEQUENCE (SIZE (1..256)) OF CriticalityDiagnostics-IE-List-item
using crit_diagnostics_ie_list_l = dyn_array<crit_diagnostics_ie_list_item_s_>;

// TriggeringMessage ::= ENUMERATED
struct trigger_msg_opts {
  enum options { init_msg, successful_outcome, unsuccessful_outcome, nulltype } value;

  const char* to_string() const;
};
using trigger_msg_e = enumerated<trigger_msg_opts>;

using bc_bearer_context_to_modify_confirm_ext_ies_container = protocol_ext_container_empty_l;

// BCBearerContextToModifyConfirm ::= SEQUENCE
struct bc_bearer_context_to_modify_confirm_s {
  bool                                                  ext             = false;
  bool                                                  ie_exts_present = false;
  bc_bearer_context_to_modify_confirm_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using crit_diagnostics_ext_ies_container = protocol_ext_container_empty_l;

// CriticalityDiagnostics ::= SEQUENCE
struct crit_diagnostics_s {
  bool                               ext                    = false;
  bool                               proc_code_present      = false;
  bool                               trigger_msg_present    = false;
  bool                               proc_crit_present      = false;
  bool                               transaction_id_present = false;
  bool                               ie_exts_present        = false;
  uint16_t                           proc_code              = 0;
  trigger_msg_e                      trigger_msg;
  crit_e                             proc_crit;
  uint16_t                           transaction_id = 0;
  crit_diagnostics_ie_list_l         ies_crit_diagnostics;
  crit_diagnostics_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cause-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using cause_ext_ies_o = protocol_ies_empty_o;

// CauseMisc ::= ENUMERATED
struct cause_misc_opts {
  enum options {
    ctrl_processing_overload,
    not_enough_user_plane_processing_res,
    hardware_fail,
    om_intervention,
    unspecified,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
using cause_misc_e = enumerated<cause_misc_opts, true>;

// CauseProtocol ::= ENUMERATED
struct cause_protocol_opts {
  enum options {
    transfer_syntax_error,
    abstract_syntax_error_reject,
    abstract_syntax_error_ignore_and_notify,
    msg_not_compatible_with_receiver_state,
    semantic_error,
    abstract_syntax_error_falsely_constructed_msg,
    unspecified,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
using cause_protocol_e = enumerated<cause_protocol_opts, true>;

// CauseRadioNetwork ::= ENUMERATED
struct cause_radio_network_opts {
  enum options {
    unspecified,
    unknown_or_already_allocated_gnb_cu_cp_ue_e1ap_id,
    unknown_or_already_allocated_gnb_cu_up_ue_e1ap_id,
    unknown_or_inconsistent_pair_of_ue_e1ap_id,
    interaction_with_other_proc,
    ppdcp_count_wrap_around,
    not_supported_qci_value,
    not_supported_5qi_value,
    encryption_algorithms_not_supported,
    integrity_protection_algorithms_not_supported,
    up_integrity_protection_not_possible,
    up_confidentiality_protection_not_possible,
    multiple_pdu_session_id_instances,
    unknown_pdu_session_id,
    multiple_qos_flow_id_instances,
    unknown_qos_flow_id,
    multiple_drb_id_instances,
    unknown_drb_id,
    invalid_qos_combination,
    proc_cancelled,
    normal_release,
    no_radio_res_available,
    action_desirable_for_radio_reasons,
    res_not_available_for_the_slice,
    pdcp_cfg_not_supported,
    // ...
    ue_dl_max_ip_data_rate_reason,
    up_integrity_protection_fail,
    release_due_to_pre_emption,
    rsn_not_available_for_the_up,
    npn_not_supported,
    report_characteristic_empty,
    existing_meas_id,
    meas_temporarily_not_available,
    meas_not_supported_for_the_obj,
    scg_activation_deactivation_fail,
    scg_deactivation_fail_due_to_data_tx,
    unknown_or_already_allocated_gnb_cu_cp_mbs_e1ap_id,
    unknown_or_already_allocated_gnb_cu_up_mbs_e1ap_id,
    unknown_or_inconsistent_pair_of_mbs_e1ap_id,
    unknown_or_inconsistent_mrb_id,
    nulltype
  } value;

  const char* to_string() const;
};
using cause_radio_network_e = enumerated<cause_radio_network_opts, true, 15>;

// CauseTransport ::= ENUMERATED
struct cause_transport_opts {
  enum options { unspecified, transport_res_unavailable, /*...*/ unknown_tnl_address_for_iab, nulltype } value;

  const char* to_string() const;
};
using cause_transport_e = enumerated<cause_transport_opts, true, 1>;

// Cause ::= CHOICE
struct cause_c {
  struct types_opts {
    enum options { radio_network, transport, protocol, misc, choice_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  cause_c() = default;
  cause_c(const cause_c& other);
  cause_c& operator=(const cause_c& other);
  ~cause_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cause_radio_network_e& radio_network()
  {
    assert_choice_type(types::radio_network, type_, "Cause");
    return c.get<cause_radio_network_e>();
  }
  cause_transport_e& transport()
  {
    assert_choice_type(types::transport, type_, "Cause");
    return c.get<cause_transport_e>();
  }
  cause_protocol_e& protocol()
  {
    assert_choice_type(types::protocol, type_, "Cause");
    return c.get<cause_protocol_e>();
  }
  cause_misc_e& misc()
  {
    assert_choice_type(types::misc, type_, "Cause");
    return c.get<cause_misc_e>();
  }
  protocol_ie_single_container_s<cause_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "Cause");
    return c.get<protocol_ie_single_container_s<cause_ext_ies_o>>();
  }
  const cause_radio_network_e& radio_network() const
  {
    assert_choice_type(types::radio_network, type_, "Cause");
    return c.get<cause_radio_network_e>();
  }
  const cause_transport_e& transport() const
  {
    assert_choice_type(types::transport, type_, "Cause");
    return c.get<cause_transport_e>();
  }
  const cause_protocol_e& protocol() const
  {
    assert_choice_type(types::protocol, type_, "Cause");
    return c.get<cause_protocol_e>();
  }
  const cause_misc_e& misc() const
  {
    assert_choice_type(types::misc, type_, "Cause");
    return c.get<cause_misc_e>();
  }
  const protocol_ie_single_container_s<cause_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "Cause");
    return c.get<protocol_ie_single_container_s<cause_ext_ies_o>>();
  }
  cause_radio_network_e&                           set_radio_network();
  cause_transport_e&                               set_transport();
  cause_protocol_e&                                set_protocol();
  cause_misc_e&                                    set_misc();
  protocol_ie_single_container_s<cause_ext_ies_o>& set_choice_ext();

private:
  types                                                            type_;
  choice_buffer_t<protocol_ie_single_container_s<cause_ext_ies_o>> c;

  void destroy_();
};

// N6JitterInformationExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using n6_jitter_info_ext_ies_o = protocol_ext_empty_o;

using n6_jitter_info_ext_ies_container = protocol_ext_container_empty_l;

// N6JitterInformation ::= SEQUENCE
struct n6_jitter_info_s {
  bool                             ext                   = false;
  bool                             ie_exts_present       = false;
  int16_t                          n6_jitter_lower_bound = -127;
  int16_t                          n6_jitter_upper_bound = -127;
  n6_jitter_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSetQoSInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdu_set_qos_info_ext_ies_o = protocol_ext_empty_o;

// Dynamic5QIDescriptor-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct dyn_5qi_descriptor_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        extended_packet_delay_budget,
        cn_packet_delay_budget_dl,
        cn_packet_delay_budget_ul,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&       extended_packet_delay_budget();
    uint32_t&       cn_packet_delay_budget_dl();
    uint32_t&       cn_packet_delay_budget_ul();
    const uint32_t& extended_packet_delay_budget() const;
    const uint32_t& cn_packet_delay_budget_dl() const;
    const uint32_t& cn_packet_delay_budget_ul() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GBR-QosFlowInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct gbr_qos_flow_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { alt_qos_para_set_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::alt_qos_para_set_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    alt_qos_para_set_list_l&       alt_qos_para_set_list() { return c; }
    const alt_qos_para_set_list_l& alt_qos_para_set_list() const { return c; }

  private:
    alt_qos_para_set_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Non-Dynamic5QIDescriptor-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct non_dyn_5qi_descriptor_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { cn_packet_delay_budget_dl, cn_packet_delay_budget_ul, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&       cn_packet_delay_budget_dl();
    uint32_t&       cn_packet_delay_budget_ul();
    const uint32_t& cn_packet_delay_budget_dl() const;
    const uint32_t& cn_packet_delay_budget_ul() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using pdu_set_qos_info_ext_ies_container = protocol_ext_container_empty_l;

// PDUSetQoSInformation ::= SEQUENCE
struct pdu_set_qos_info_s {
  struct pdu_set_integrated_handling_info_opts {
    enum options { true_value, false_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using pdu_set_integrated_handling_info_e_ = enumerated<pdu_set_integrated_handling_info_opts, true>;

  // member variables
  bool                                pdu_set_delay_budget_present             = false;
  bool                                pdu_set_error_rate_present               = false;
  bool                                pdu_set_integrated_handling_info_present = false;
  bool                                ie_exts_present                          = false;
  uint32_t                            pdu_set_delay_budget                     = 1;
  packet_error_rate_s                 pdu_set_error_rate;
  pdu_set_integrated_handling_info_e_ pdu_set_integrated_handling_info;
  pdu_set_qos_info_ext_ies_container  ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSetQoSParameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdu_set_qos_params_ext_ies_o = protocol_ext_empty_o;

// TSCAssistanceInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct tsc_assist_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { survival_time, n6_jitter_info, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&               survival_time();
    n6_jitter_info_s&       n6_jitter_info();
    const uint32_t&         survival_time() const;
    const n6_jitter_info_s& n6_jitter_info() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct dyn_5qi_descriptor_ext_ies_container {
  bool     extended_packet_delay_budget_present = false;
  bool     cn_packet_delay_budget_dl_present    = false;
  bool     cn_packet_delay_budget_ul_present    = false;
  uint32_t extended_packet_delay_budget;
  uint32_t cn_packet_delay_budget_dl;
  uint32_t cn_packet_delay_budget_ul;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Dynamic5QIDescriptor ::= SEQUENCE
struct dyn_5qi_descriptor_s {
  struct delay_crit_opts {
    enum options { delay_crit, non_delay_crit, nulltype } value;

    const char* to_string() const;
  };
  using delay_crit_e_ = enumerated<delay_crit_opts>;

  // member variables
  bool                                 five_qi_present               = false;
  bool                                 delay_crit_present            = false;
  bool                                 averaging_win_present         = false;
  bool                                 max_data_burst_volume_present = false;
  bool                                 ie_exts_present               = false;
  uint8_t                              qos_prio_level                = 0;
  uint16_t                             packet_delay_budget           = 0;
  packet_error_rate_s                  packet_error_rate;
  uint16_t                             five_qi = 0;
  delay_crit_e_                        delay_crit;
  uint16_t                             averaging_win         = 0;
  uint16_t                             max_data_burst_volume = 0;
  dyn_5qi_descriptor_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ECNMarkingorCongestionInformationReportingRequest-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using ecn_markingor_congestion_info_report_request_ext_ies_o = protocol_ies_empty_o;

// EHC-Common-Parameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ehc_common_params_ext_ies_o = protocol_ext_empty_o;

// EHC-Downlink-Parameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct ehc_dl_params_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { max_c_id_ehc_dl, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::max_c_id_ehc_dl; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&       max_c_id_ehc_dl() { return c; }
    const uint16_t& max_c_id_ehc_dl() const { return c; }

  private:
    uint16_t c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// EHC-Uplink-Parameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ehc_ul_params_ext_ies_o = protocol_ext_empty_o;

// GBR-QoSFlowInformation ::= SEQUENCE
struct gbr_qos_flow_info_s {
  bool                                                  ext                             = false;
  bool                                                  max_packet_loss_rate_dl_present = false;
  bool                                                  max_packet_loss_rate_ul_present = false;
  uint64_t                                              max_flow_bit_rate_dl            = 0;
  uint64_t                                              max_flow_bit_rate_ul            = 0;
  uint64_t                                              guaranteed_flow_bit_rate_dl     = 0;
  uint64_t                                              guaranteed_flow_bit_rate_ul     = 0;
  uint16_t                                              max_packet_loss_rate_dl         = 0;
  uint16_t                                              max_packet_loss_rate_ul         = 0;
  protocol_ext_container_l<gbr_qos_flow_info_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGRANAllocationAndRetentionPriority-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ngran_alloc_and_retention_prio_ext_ies_o = protocol_ext_empty_o;

struct non_dyn_5qi_descriptor_ext_ies_container {
  bool     cn_packet_delay_budget_dl_present = false;
  bool     cn_packet_delay_budget_ul_present = false;
  uint32_t cn_packet_delay_budget_dl;
  uint32_t cn_packet_delay_budget_ul;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Non-Dynamic5QIDescriptor ::= SEQUENCE
struct non_dyn_5qi_descriptor_s {
  bool                                     qos_prio_level_present        = false;
  bool                                     averaging_win_present         = false;
  bool                                     max_data_burst_volume_present = false;
  bool                                     ie_exts_present               = false;
  uint16_t                                 five_qi                       = 0;
  uint8_t                                  qos_prio_level                = 0;
  uint16_t                                 averaging_win                 = 0;
  uint16_t                                 max_data_burst_volume         = 0;
  non_dyn_5qi_descriptor_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_set_qos_params_ext_ies_container = protocol_ext_container_empty_l;

// PDUSetQoSParameters ::= SEQUENCE
struct pdu_set_qos_params_s {
  bool                                 ul_pdu_set_qos_info_present = false;
  bool                                 dl_pdu_set_qos_info_present = false;
  bool                                 ie_exts_present             = false;
  pdu_set_qos_info_s                   ul_pdu_set_qos_info;
  pdu_set_qos_info_s                   dl_pdu_set_qos_info;
  pdu_set_qos_params_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Pre-emptionCapability ::= ENUMERATED
struct pre_emption_cap_opts {
  enum options { shall_not_trigger_pre_emption, may_trigger_pre_emption, nulltype } value;

  const char* to_string() const;
};
using pre_emption_cap_e = enumerated<pre_emption_cap_opts>;

// Pre-emptionVulnerability ::= ENUMERATED
struct pre_emption_vulnerability_opts {
  enum options { not_pre_emptable, pre_emptable, nulltype } value;

  const char* to_string() const;
};
using pre_emption_vulnerability_e = enumerated<pre_emption_vulnerability_opts>;

// QoS-Characteristics-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using qos_characteristics_ext_ies_o = protocol_ies_empty_o;

// QosMonitoringDisabled ::= ENUMERATED
struct qos_monitoring_disabled_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using qos_monitoring_disabled_e = enumerated<qos_monitoring_disabled_opts, true>;

// QosMonitoringRequest ::= ENUMERATED
struct qos_monitoring_request_opts {
  enum options { ul, dl, both, nulltype } value;

  const char* to_string() const;
};
using qos_monitoring_request_e = enumerated<qos_monitoring_request_opts>;

struct tsc_assist_info_ext_ies_container {
  bool             survival_time_present  = false;
  bool             n6_jitter_info_present = false;
  uint32_t         survival_time;
  n6_jitter_info_s n6_jitter_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TSCAssistanceInformation ::= SEQUENCE
struct tsc_assist_info_s {
  bool                              ie_exts_present = false;
  uint32_t                          periodicity     = 0;
  unbounded_octstring<true>         burst_arrival_time;
  tsc_assist_info_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TSCTrafficCharacteristics-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using tsc_traffic_characteristics_ext_ies_o = protocol_ext_empty_o;

// BufferSize ::= ENUMERATED
struct buffer_size_opts {
  enum options { kbyte2, kbyte4, kbyte8, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using buffer_size_e = enumerated<buffer_size_opts, true>;

// Dictionary ::= ENUMERATED
struct dictionary_opts {
  enum options { sip_sdp, operator_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using dictionary_e = enumerated<dictionary_opts, true>;

// ECNMarkingorCongestionInformationReportingRequest ::= CHOICE
struct ecn_markingor_congestion_info_report_request_c {
  struct ecn_markingat_ngran_opts {
    enum options { ul, dl, both, stop, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ecn_markingat_ngran_e_ = enumerated<ecn_markingat_ngran_opts, true>;
  struct ecn_markingat_up_f_opts {
    enum options { ul, dl, both, stop, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ecn_markingat_up_f_e_ = enumerated<ecn_markingat_up_f_opts, true>;
  struct congestion_info_opts {
    enum options { ul, dl, both, stop, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using congestion_info_e_ = enumerated<congestion_info_opts, true>;
  struct types_opts {
    enum options { ecn_markingat_ngran, ecn_markingat_up_f, congestion_info, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ecn_markingor_congestion_info_report_request_c() = default;
  ecn_markingor_congestion_info_report_request_c(const ecn_markingor_congestion_info_report_request_c& other);
  ecn_markingor_congestion_info_report_request_c&
  operator=(const ecn_markingor_congestion_info_report_request_c& other);
  ~ecn_markingor_congestion_info_report_request_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ecn_markingat_ngran_e_& ecn_markingat_ngran()
  {
    assert_choice_type(types::ecn_markingat_ngran, type_, "ECNMarkingorCongestionInformationReportingRequest");
    return c.get<ecn_markingat_ngran_e_>();
  }
  ecn_markingat_up_f_e_& ecn_markingat_up_f()
  {
    assert_choice_type(types::ecn_markingat_up_f, type_, "ECNMarkingorCongestionInformationReportingRequest");
    return c.get<ecn_markingat_up_f_e_>();
  }
  congestion_info_e_& congestion_info()
  {
    assert_choice_type(types::congestion_info, type_, "ECNMarkingorCongestionInformationReportingRequest");
    return c.get<congestion_info_e_>();
  }
  protocol_ie_single_container_s<ecn_markingor_congestion_info_report_request_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "ECNMarkingorCongestionInformationReportingRequest");
    return c.get<protocol_ie_single_container_s<ecn_markingor_congestion_info_report_request_ext_ies_o>>();
  }
  const ecn_markingat_ngran_e_& ecn_markingat_ngran() const
  {
    assert_choice_type(types::ecn_markingat_ngran, type_, "ECNMarkingorCongestionInformationReportingRequest");
    return c.get<ecn_markingat_ngran_e_>();
  }
  const ecn_markingat_up_f_e_& ecn_markingat_up_f() const
  {
    assert_choice_type(types::ecn_markingat_up_f, type_, "ECNMarkingorCongestionInformationReportingRequest");
    return c.get<ecn_markingat_up_f_e_>();
  }
  const congestion_info_e_& congestion_info() const
  {
    assert_choice_type(types::congestion_info, type_, "ECNMarkingorCongestionInformationReportingRequest");
    return c.get<congestion_info_e_>();
  }
  const protocol_ie_single_container_s<ecn_markingor_congestion_info_report_request_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "ECNMarkingorCongestionInformationReportingRequest");
    return c.get<protocol_ie_single_container_s<ecn_markingor_congestion_info_report_request_ext_ies_o>>();
  }
  ecn_markingat_ngran_e_&                                                                 set_ecn_markingat_ngran();
  ecn_markingat_up_f_e_&                                                                  set_ecn_markingat_up_f();
  congestion_info_e_&                                                                     set_congestion_info();
  protocol_ie_single_container_s<ecn_markingor_congestion_info_report_request_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                   type_;
  choice_buffer_t<protocol_ie_single_container_s<ecn_markingor_congestion_info_report_request_ext_ies_o>> c;

  void destroy_();
};

using ehc_common_params_ext_ies_container = protocol_ext_container_empty_l;

// EHC-Common-Parameters ::= SEQUENCE
struct ehc_common_params_s {
  struct ehc_c_id_len_opts {
    enum options { bits7, bits15, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ehc_c_id_len_e_ = enumerated<ehc_c_id_len_opts, true>;

  // member variables
  bool                                ie_exts_present = false;
  ehc_c_id_len_e_                     ehc_c_id_len;
  ehc_common_params_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EHC-Downlink-Parameters ::= SEQUENCE
struct ehc_dl_params_s {
  struct drb_continue_ehc_dl_opts {
    enum options { true_value, /*...*/ false_value, nulltype } value;

    const char* to_string() const;
  };
  using drb_continue_ehc_dl_e_ = enumerated<drb_continue_ehc_dl_opts, true, 1>;

  // member variables
  drb_continue_ehc_dl_e_                            drb_continue_ehc_dl;
  protocol_ext_container_l<ehc_dl_params_ext_ies_o> ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EHC-Parameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ehc_params_ext_ies_o = protocol_ext_empty_o;

using ehc_ul_params_ext_ies_container = protocol_ext_container_empty_l;

// EHC-Uplink-Parameters ::= SEQUENCE
struct ehc_ul_params_s {
  struct drb_continue_ehc_ul_opts {
    enum options { true_value, /*...*/ false_value, nulltype } value;

    const char* to_string() const;
  };
  using drb_continue_ehc_ul_e_ = enumerated<drb_continue_ehc_ul_opts, true, 1>;

  // member variables
  bool                            ie_exts_present = false;
  drb_continue_ehc_ul_e_          drb_continue_ehc_ul;
  ehc_ul_params_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1U-TNL-InfoToAdd-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using f1_u_tnl_info_to_add_item_ext_ies_o = protocol_ext_empty_o;

// F1U-TNL-InfoToAddOrModify-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using f1_u_tnl_info_to_add_or_modify_item_ext_ies_o = protocol_ext_empty_o;

// F1U-TNL-InfoToRelease-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using f1_u_tnl_info_to_release_item_ext_ies_o = protocol_ext_empty_o;

// MBSNGUInformationAt5GC-Multicast-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mb_sn_gu_info_at5_gc_multicast_ext_ies_o = protocol_ext_empty_o;

using ngran_alloc_and_retention_prio_ext_ies_container = protocol_ext_container_empty_l;

// NGRANAllocationAndRetentionPriority ::= SEQUENCE
struct ngran_alloc_and_retention_prio_s {
  bool                                             ie_exts_present = false;
  uint8_t                                          prio_level      = 0;
  pre_emption_cap_e                                pre_emption_cap;
  pre_emption_vulnerability_e                      pre_emption_vulnerability;
  ngran_alloc_and_retention_prio_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoS-Characteristics ::= CHOICE
struct qos_characteristics_c {
  struct types_opts {
    enum options { non_dyn_5qi, dyn_5qi, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  qos_characteristics_c() = default;
  qos_characteristics_c(const qos_characteristics_c& other);
  qos_characteristics_c& operator=(const qos_characteristics_c& other);
  ~qos_characteristics_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  non_dyn_5qi_descriptor_s& non_dyn_5qi()
  {
    assert_choice_type(types::non_dyn_5qi, type_, "QoS-Characteristics");
    return c.get<non_dyn_5qi_descriptor_s>();
  }
  dyn_5qi_descriptor_s& dyn_5qi()
  {
    assert_choice_type(types::dyn_5qi, type_, "QoS-Characteristics");
    return c.get<dyn_5qi_descriptor_s>();
  }
  protocol_ie_single_container_s<qos_characteristics_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "QoS-Characteristics");
    return c.get<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>();
  }
  const non_dyn_5qi_descriptor_s& non_dyn_5qi() const
  {
    assert_choice_type(types::non_dyn_5qi, type_, "QoS-Characteristics");
    return c.get<non_dyn_5qi_descriptor_s>();
  }
  const dyn_5qi_descriptor_s& dyn_5qi() const
  {
    assert_choice_type(types::dyn_5qi, type_, "QoS-Characteristics");
    return c.get<dyn_5qi_descriptor_s>();
  }
  const protocol_ie_single_container_s<qos_characteristics_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "QoS-Characteristics");
    return c.get<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>();
  }
  non_dyn_5qi_descriptor_s&                                      set_non_dyn_5qi();
  dyn_5qi_descriptor_s&                                          set_dyn_5qi();
  protocol_ie_single_container_s<qos_characteristics_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<dyn_5qi_descriptor_s,
                  non_dyn_5qi_descriptor_s,
                  protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>
      c;

  void destroy_();
};

// QoSFlowLevelQoSParameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct qos_flow_level_qos_params_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        qos_monitoring_request,
        mcg_offered_gbr_qos_flow_info,
        qos_monitoring_report_freq,
        qos_monitoring_disabled,
        data_forwarding_source_ip_address,
        pdu_set_qos_params,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_monitoring_request_e&                    qos_monitoring_request();
    gbr_qos_flow_info_s&                         mcg_offered_gbr_qos_flow_info();
    uint16_t&                                    qos_monitoring_report_freq();
    qos_monitoring_disabled_e&                   qos_monitoring_disabled();
    bounded_bitstring<1, 160, true, true>&       data_forwarding_source_ip_address();
    pdu_set_qos_params_s&                        pdu_set_qos_params();
    const qos_monitoring_request_e&              qos_monitoring_request() const;
    const gbr_qos_flow_info_s&                   mcg_offered_gbr_qos_flow_info() const;
    const uint16_t&                              qos_monitoring_report_freq() const;
    const qos_monitoring_disabled_e&             qos_monitoring_disabled() const;
    const bounded_bitstring<1, 160, true, true>& data_forwarding_source_ip_address() const;
    const pdu_set_qos_params_s&                  pdu_set_qos_params() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ROHC-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using rohc_ext_ies_o = protocol_ext_empty_o;

// RedundantQoSFlowIndicator ::= ENUMERATED
struct redundant_qos_flow_ind_opts {
  enum options { true_value, false_value, nulltype } value;

  const char* to_string() const;
};
using redundant_qos_flow_ind_e = enumerated<redundant_qos_flow_ind_opts>;

using tsc_traffic_characteristics_ext_ies_container = protocol_ext_container_empty_l;

// TSCTrafficCharacteristics ::= SEQUENCE
struct tsc_traffic_characteristics_s {
  bool                                          tsc_traffic_characteristics_ul_present = false;
  bool                                          tsc_traffic_characteristics_dl_present = false;
  bool                                          ie_exts_present                        = false;
  tsc_assist_info_s                             tsc_traffic_characteristics_ul;
  tsc_assist_info_s                             tsc_traffic_characteristics_dl;
  tsc_traffic_characteristics_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UDC-Parameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct udc_params_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { version_id, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::version_id; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&       version_id() { return c; }
    const uint8_t& version_id() const { return c; }

  private:
    uint8_t c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UplinkOnlyROHC-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ul_only_rohc_ext_ies_o = protocol_ext_empty_o;

// AdditionalPDCPduplicationInformation ::= ENUMERATED
struct add_pdcp_dupl_info_opts {
  enum options { three, four, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using add_pdcp_dupl_info_e = enumerated<add_pdcp_dupl_info_opts, true>;

// DiscardTimerExtended ::= ENUMERATED
struct discard_timer_extended_opts {
  enum options { ms0dot5, ms1, ms2, ms4, ms6, ms8, /*...*/ ms2000, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using discard_timer_extended_e = enumerated<discard_timer_extended_opts, true, 1>;

using ehc_params_ext_ies_container = protocol_ext_container_empty_l;

// EHC-Parameters ::= SEQUENCE
struct ehc_params_s {
  bool                         ehc_dl_present  = false;
  bool                         ehc_ul_present  = false;
  bool                         ie_exts_present = false;
  ehc_common_params_s          ehc_common;
  ehc_dl_params_s              ehc_dl;
  ehc_ul_params_s              ehc_ul;
  ehc_params_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using f1_u_tnl_info_to_add_item_ext_ies_container = protocol_ext_container_empty_l;

// F1U-TNL-InfoToAdd-Item ::= SEQUENCE
struct f1_u_tnl_info_to_add_item_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  fixed_octstring<4, true>                    broadcast_f1_u_context_ref_e1;
  f1_u_tnl_info_to_add_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using f1_u_tnl_info_to_add_or_modify_item_ext_ies_container = protocol_ext_container_empty_l;

// F1U-TNL-InfoToAddOrModify-Item ::= SEQUENCE
struct f1_u_tnl_info_to_add_or_modify_item_s {
  bool                                                  ext                                          = false;
  bool                                                  bc_bearer_context_f1_u_tnl_infoat_du_present = false;
  bool                                                  ie_exts_present                              = false;
  fixed_octstring<4, true>                              broadcast_f1_u_context_ref_e1;
  bc_bearer_context_f1_u_tnl_infoat_du_c                bc_bearer_context_f1_u_tnl_infoat_du;
  f1_u_tnl_info_to_add_or_modify_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using f1_u_tnl_info_to_release_item_ext_ies_container = protocol_ext_container_empty_l;

// F1U-TNL-InfoToRelease-Item ::= SEQUENCE
struct f1_u_tnl_info_to_release_item_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  fixed_octstring<4, true>                        broadcast_f1_u_context_ref_e1;
  f1_u_tnl_info_to_release_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSNGUInformationAt5GC-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using mb_sn_gu_info_at5_gc_ext_ies_o = protocol_ies_empty_o;

using mb_sn_gu_info_at5_gc_multicast_ext_ies_container = protocol_ext_container_empty_l;

// MBSNGUInformationAt5GC-Multicast ::= SEQUENCE
struct mb_sn_gu_info_at5_gc_multicast_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  bounded_bitstring<1, 160, true, true>            ipmc_address;
  bounded_bitstring<1, 160, true, true>            ipsource_address;
  fixed_octstring<4, true>                         gtp_dl_teid;
  mb_sn_gu_info_at5_gc_multicast_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSNGUInformationAtNGRAN-Request ::= ENUMERATED
struct mb_sn_gu_info_at_ngran_request_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using mb_sn_gu_info_at_ngran_request_e = enumerated<mb_sn_gu_info_at_ngran_request_opts, true>;

// MBSNGUInformationAtNGRAN-Request-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mb_sn_gu_info_at_ngran_request_item_ext_ies_o = protocol_ext_empty_o;

// PDCP-StatusReportIndication ::= ENUMERATED
struct pdcp_status_report_ind_opts {
  enum options { dl, ul, both, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdcp_status_report_ind_e = enumerated<pdcp_status_report_ind_opts, true>;

// PDCPSNGapReport ::= ENUMERATED
struct pdcp_sn_gap_report_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdcp_sn_gap_report_e = enumerated<pdcp_sn_gap_report_opts, true>;

// PSIbasedDiscardTimer ::= ENUMERATED
struct psi_based_discard_timer_opts {
  enum options {
    ms0,
    ms2,
    ms4,
    ms6,
    ms8,
    ms10,
    ms12,
    ms14,
    ms18,
    ms22,
    ms26,
    ms30,
    ms40,
    ms50,
    ms75,
    ms100,
    // ...
    nulltype
  } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using psi_based_discard_timer_e = enumerated<psi_based_discard_timer_opts, true>;

// QoS-Flow-Mapping-Indication ::= ENUMERATED
struct qos_flow_map_ind_opts {
  enum options { ul, dl, /*...*/ nulltype } value;

  const char* to_string() const;
};
using qos_flow_map_ind_e = enumerated<qos_flow_map_ind_opts, true>;

// QoS-Flow-QoS-Parameter-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct qos_flow_qos_param_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        redundant_qos_flow_ind,
        tsc_traffic_characteristics,
        ecn_markingor_congestion_info_report_request,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    redundant_qos_flow_ind_e&                             redundant_qos_flow_ind();
    tsc_traffic_characteristics_s&                        tsc_traffic_characteristics();
    ecn_markingor_congestion_info_report_request_c&       ecn_markingor_congestion_info_report_request();
    const redundant_qos_flow_ind_e&                       redundant_qos_flow_ind() const;
    const tsc_traffic_characteristics_s&                  tsc_traffic_characteristics() const;
    const ecn_markingor_congestion_info_report_request_c& ecn_markingor_congestion_info_report_request() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct qos_flow_level_qos_params_ext_ies_container {
  bool                                  qos_monitoring_request_present            = false;
  bool                                  mcg_offered_gbr_qos_flow_info_present     = false;
  bool                                  qos_monitoring_report_freq_present        = false;
  bool                                  qos_monitoring_disabled_present           = false;
  bool                                  data_forwarding_source_ip_address_present = false;
  bool                                  pdu_set_qos_params_present                = false;
  qos_monitoring_request_e              qos_monitoring_request;
  gbr_qos_flow_info_s                   mcg_offered_gbr_qos_flow_info;
  uint16_t                              qos_monitoring_report_freq;
  qos_monitoring_disabled_e             qos_monitoring_disabled;
  bounded_bitstring<1, 160, true, true> data_forwarding_source_ip_address;
  pdu_set_qos_params_s                  pdu_set_qos_params;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoSFlowLevelQoSParameters ::= SEQUENCE
struct qos_flow_level_qos_params_s {
  struct reflective_qos_attribute_opts {
    enum options { subject_to, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using reflective_qos_attribute_e_ = enumerated<reflective_qos_attribute_opts, true>;
  struct add_qos_info_opts {
    enum options { more_likely, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using add_qos_info_e_ = enumerated<add_qos_info_opts, true>;
  struct reflective_qos_ind_opts {
    enum options { enabled, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using reflective_qos_ind_e_ = enumerated<reflective_qos_ind_opts, true>;

  // member variables
  bool                                        gbr_qos_flow_info_present        = false;
  bool                                        reflective_qos_attribute_present = false;
  bool                                        add_qos_info_present             = false;
  bool                                        paging_policy_idx_present        = false;
  bool                                        reflective_qos_ind_present       = false;
  bool                                        ie_exts_present                  = false;
  qos_characteristics_c                       qos_characteristics;
  ngran_alloc_and_retention_prio_s            ngra_nalloc_retention_prio;
  gbr_qos_flow_info_s                         gbr_qos_flow_info;
  reflective_qos_attribute_e_                 reflective_qos_attribute;
  add_qos_info_e_                             add_qos_info;
  uint8_t                                     paging_policy_idx = 1;
  reflective_qos_ind_e_                       reflective_qos_ind;
  qos_flow_level_qos_params_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using rohc_ext_ies_container = protocol_ext_container_empty_l;

// ROHC ::= SEQUENCE
struct rohc_s {
  struct continue_rohc_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using continue_rohc_e_ = enumerated<continue_rohc_opts, true>;

  // member variables
  bool                   continue_rohc_present = false;
  bool                   ie_exts_present       = false;
  uint16_t               max_c_id              = 0;
  uint16_t               rohc_profiles         = 0;
  continue_rohc_e_       continue_rohc;
  rohc_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ROHC-Parameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using rohc_params_ext_ies_o = protocol_ies_empty_o;

// T-Reordering ::= ENUMERATED
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
    // ...
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using t_reordering_e = enumerated<t_reordering_opts, true>;

// T-ReorderingTimer-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using t_reordering_timer_ext_ies_o = protocol_ext_empty_o;

// UDC-Parameters ::= SEQUENCE
struct udc_params_s {
  struct continue_udc_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using continue_udc_e_ = enumerated<continue_udc_opts, true>;

  // member variables
  bool                                           dictionary_present   = false;
  bool                                           continue_udc_present = false;
  buffer_size_e                                  buffer_size;
  dictionary_e                                   dictionary;
  continue_udc_e_                                continue_udc;
  protocol_ext_container_l<udc_params_ext_ies_o> ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ul_only_rohc_ext_ies_container = protocol_ext_container_empty_l;

// UplinkOnlyROHC ::= SEQUENCE
struct ul_only_rohc_s {
  struct continue_rohc_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using continue_rohc_e_ = enumerated<continue_rohc_opts, true>;

  // member variables
  bool                           continue_rohc_present = false;
  bool                           ie_exts_present       = false;
  uint16_t                       max_c_id              = 0;
  uint16_t                       rohc_profiles         = 0;
  continue_rohc_e_               continue_rohc;
  ul_only_rohc_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DiscardTimer ::= ENUMERATED
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
using discard_timer_e = enumerated<discard_timer_opts>;

// Duplication-Activation ::= ENUMERATED
struct dupl_activation_opts {
  enum options { active, inactive, /*...*/ nulltype } value;

  const char* to_string() const;
};
using dupl_activation_e = enumerated<dupl_activation_opts, true>;

// F1U-TNL-InfoToAdd-List ::= SEQUENCE (SIZE (1..512)) OF F1U-TNL-InfoToAdd-Item
using f1_u_tnl_info_to_add_list_l = dyn_array<f1_u_tnl_info_to_add_item_s>;

// F1U-TNL-InfoToAddOrModify-List ::= SEQUENCE (SIZE (1..512)) OF F1U-TNL-InfoToAddOrModify-Item
using f1_u_tnl_info_to_add_or_modify_list_l = dyn_array<f1_u_tnl_info_to_add_or_modify_item_s>;

// F1U-TNL-InfoToRelease-List ::= SEQUENCE (SIZE (1..512)) OF F1U-TNL-InfoToRelease-Item
using f1_u_tnl_info_to_release_list_l = dyn_array<f1_u_tnl_info_to_release_item_s>;

// LocationDependentMBSNGUInformationAt5GC-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using location_dependent_mb_sn_gu_info_at5_gc_item_ext_ies_o = protocol_ext_empty_o;

// MBSNGUInformationAt5GC ::= CHOICE
struct mb_sn_gu_info_at5_gc_c {
  struct types_opts {
    enum options { multicast, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  mb_sn_gu_info_at5_gc_c() = default;
  mb_sn_gu_info_at5_gc_c(const mb_sn_gu_info_at5_gc_c& other);
  mb_sn_gu_info_at5_gc_c& operator=(const mb_sn_gu_info_at5_gc_c& other);
  ~mb_sn_gu_info_at5_gc_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  mb_sn_gu_info_at5_gc_multicast_s& multicast()
  {
    assert_choice_type(types::multicast, type_, "MBSNGUInformationAt5GC");
    return c.get<mb_sn_gu_info_at5_gc_multicast_s>();
  }
  protocol_ie_single_container_s<mb_sn_gu_info_at5_gc_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "MBSNGUInformationAt5GC");
    return c.get<protocol_ie_single_container_s<mb_sn_gu_info_at5_gc_ext_ies_o>>();
  }
  const mb_sn_gu_info_at5_gc_multicast_s& multicast() const
  {
    assert_choice_type(types::multicast, type_, "MBSNGUInformationAt5GC");
    return c.get<mb_sn_gu_info_at5_gc_multicast_s>();
  }
  const protocol_ie_single_container_s<mb_sn_gu_info_at5_gc_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "MBSNGUInformationAt5GC");
    return c.get<protocol_ie_single_container_s<mb_sn_gu_info_at5_gc_ext_ies_o>>();
  }
  mb_sn_gu_info_at5_gc_multicast_s&                               set_multicast();
  protocol_ie_single_container_s<mb_sn_gu_info_at5_gc_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<mb_sn_gu_info_at5_gc_multicast_s, protocol_ie_single_container_s<mb_sn_gu_info_at5_gc_ext_ies_o>> c;

  void destroy_();
};

using mb_sn_gu_info_at_ngran_request_item_ext_ies_container = protocol_ext_container_empty_l;

// MBSNGUInformationAtNGRAN-Request-Item ::= SEQUENCE
struct mb_sn_gu_info_at_ngran_request_item_s {
  bool                                                  ext                 = false;
  bool                                                  ie_exts_present     = false;
  uint32_t                                              mbs_area_session_id = 0;
  mb_sn_gu_info_at_ngran_request_e                      mbs_ngu_info_at_ngran_request;
  mb_sn_gu_info_at_ngran_request_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OutOfOrderDelivery ::= ENUMERATED
struct out_of_order_delivery_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using out_of_order_delivery_e = enumerated<out_of_order_delivery_opts, true>;

// PDCP-Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdcp_cfg_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        pdcp_status_report_ind,
        add_pdcp_dupl_info,
        ehc_params,
        udc_params,
        discard_timer_extended,
        psi_based_discard_timer,
        pdcp_sn_gap_report,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pdcp_status_report_ind_e&        pdcp_status_report_ind();
    add_pdcp_dupl_info_e&            add_pdcp_dupl_info();
    ehc_params_s&                    ehc_params();
    udc_params_s&                    udc_params();
    discard_timer_extended_e&        discard_timer_extended();
    psi_based_discard_timer_e&       psi_based_discard_timer();
    pdcp_sn_gap_report_e&            pdcp_sn_gap_report();
    const pdcp_status_report_ind_e&  pdcp_status_report_ind() const;
    const add_pdcp_dupl_info_e&      add_pdcp_dupl_info() const;
    const ehc_params_s&              ehc_params() const;
    const udc_params_s&              udc_params() const;
    const discard_timer_extended_e&  discard_timer_extended() const;
    const psi_based_discard_timer_e& psi_based_discard_timer() const;
    const pdcp_sn_gap_report_e&      pdcp_sn_gap_report() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDCP-DataRecovery ::= ENUMERATED
struct pdcp_data_recovery_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdcp_data_recovery_e = enumerated<pdcp_data_recovery_opts, true>;

// PDCP-Duplication ::= ENUMERATED
struct pdcp_dupl_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdcp_dupl_e = enumerated<pdcp_dupl_opts, true>;

// PDCP-Reestablishment ::= ENUMERATED
struct pdcp_reest_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdcp_reest_e = enumerated<pdcp_reest_opts, true>;

// PDCP-SN-Size ::= ENUMERATED
struct pdcp_sn_size_opts {
  enum options { s_neg12, s_neg18, /*...*/ s_neg7, s_neg15, s_neg16, nulltype } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
using pdcp_sn_size_e = enumerated<pdcp_sn_size_opts, true, 3>;

struct qos_flow_qos_param_item_ext_ies_container {
  bool                                           redundant_qos_flow_ind_present                       = false;
  bool                                           tsc_traffic_characteristics_present                  = false;
  bool                                           ecn_markingor_congestion_info_report_request_present = false;
  redundant_qos_flow_ind_e                       redundant_qos_flow_ind;
  tsc_traffic_characteristics_s                  tsc_traffic_characteristics;
  ecn_markingor_congestion_info_report_request_c ecn_markingor_congestion_info_report_request;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoS-Flow-QoS-Parameter-Item ::= SEQUENCE
struct qos_flow_qos_param_item_s {
  bool                                      ext                      = false;
  bool                                      qos_flow_map_ind_present = false;
  bool                                      ie_exts_present          = false;
  uint8_t                                   qos_flow_id              = 0;
  qos_flow_level_qos_params_s               qos_flow_level_qos_params;
  qos_flow_map_ind_e                        qos_flow_map_ind;
  qos_flow_qos_param_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Mode ::= ENUMERATED
struct rlc_mode_opts {
  enum options {
    rlc_tm,
    rlc_am,
    rlc_um_bidirectional,
    rlc_um_unidirectional_ul,
    rlc_um_unidirectional_dl,
    /*...*/ nulltype
  } value;

  const char* to_string() const;
};
using rlc_mode_e = enumerated<rlc_mode_opts, true>;

// ROHC-Parameters ::= CHOICE
struct rohc_params_c {
  struct types_opts {
    enum options { rohc, ul_only_rohc, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  rohc_params_c() = default;
  rohc_params_c(const rohc_params_c& other);
  rohc_params_c& operator=(const rohc_params_c& other);
  ~rohc_params_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  rohc_s& rohc()
  {
    assert_choice_type(types::rohc, type_, "ROHC-Parameters");
    return c.get<rohc_s>();
  }
  ul_only_rohc_s& ul_only_rohc()
  {
    assert_choice_type(types::ul_only_rohc, type_, "ROHC-Parameters");
    return c.get<ul_only_rohc_s>();
  }
  protocol_ie_single_container_s<rohc_params_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "ROHC-Parameters");
    return c.get<protocol_ie_single_container_s<rohc_params_ext_ies_o>>();
  }
  const rohc_s& rohc() const
  {
    assert_choice_type(types::rohc, type_, "ROHC-Parameters");
    return c.get<rohc_s>();
  }
  const ul_only_rohc_s& ul_only_rohc() const
  {
    assert_choice_type(types::ul_only_rohc, type_, "ROHC-Parameters");
    return c.get<ul_only_rohc_s>();
  }
  const protocol_ie_single_container_s<rohc_params_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "ROHC-Parameters");
    return c.get<protocol_ie_single_container_s<rohc_params_ext_ies_o>>();
  }
  rohc_s&                                                set_rohc();
  ul_only_rohc_s&                                        set_ul_only_rohc();
  protocol_ie_single_container_s<rohc_params_ext_ies_o>& set_choice_ext();

private:
  types                                                                                          type_;
  choice_buffer_t<protocol_ie_single_container_s<rohc_params_ext_ies_o>, rohc_s, ul_only_rohc_s> c;

  void destroy_();
};

using t_reordering_timer_ext_ies_container = protocol_ext_container_empty_l;

// T-ReorderingTimer ::= SEQUENCE
struct t_reordering_timer_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  t_reordering_e                       t_reordering;
  t_reordering_timer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULDataSplitThreshold ::= ENUMERATED
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
    // ...
    nulltype
  } value;
  typedef int32_t number_type;

  const char* to_string() const;
  int32_t     to_number() const;
};
using ul_data_split_thres_e = enumerated<ul_data_split_thres_opts, true>;

// BCBearerContextNGU-TNLInfoatNGRAN-Request-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using bc_bearer_context_ngu_tnl_infoat_ngran_request_ext_ies_o = protocol_ies_empty_o;

// BCMRBModifyConfiguration-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct bcmrb_modify_cfg_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { f1_u_tnl_info_to_add_or_modify_list, f1_u_tnl_info_to_release_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    f1_u_tnl_info_to_add_or_modify_list_l&       f1_u_tnl_info_to_add_or_modify_list();
    f1_u_tnl_info_to_release_list_l&             f1_u_tnl_info_to_release_list();
    const f1_u_tnl_info_to_add_or_modify_list_l& f1_u_tnl_info_to_add_or_modify_list() const;
    const f1_u_tnl_info_to_release_list_l&       f1_u_tnl_info_to_release_list() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BCMRBSetupConfiguration-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct bcmrb_setup_cfg_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { f1_u_tnl_info_to_add_list, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::f1_u_tnl_info_to_add_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    f1_u_tnl_info_to_add_list_l&       f1_u_tnl_info_to_add_list() { return c; }
    const f1_u_tnl_info_to_add_list_l& f1_u_tnl_info_to_add_list() const { return c; }

  private:
    f1_u_tnl_info_to_add_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using location_dependent_mb_sn_gu_info_at5_gc_item_ext_ies_container = protocol_ext_container_empty_l;

// LocationDependentMBSNGUInformationAt5GC-Item ::= SEQUENCE
struct location_dependent_mb_sn_gu_info_at5_gc_item_s {
  bool                                                           ext                 = false;
  bool                                                           ie_exts_present     = false;
  uint32_t                                                       mbs_area_session_id = 0;
  mb_sn_gu_info_at5_gc_c                                         mbs_ngu_info_at5_gc;
  location_dependent_mb_sn_gu_info_at5_gc_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSNGUInformationAtNGRAN-Request-List ::= SEQUENCE (SIZE (1..256)) OF MBSNGUInformationAtNGRAN-Request-Item
using mb_sn_gu_info_at_ngran_request_list_l = dyn_array<mb_sn_gu_info_at_ngran_request_item_s>;

struct pdcp_cfg_ext_ies_container {
  bool                      pdcp_status_report_ind_present  = false;
  bool                      add_pdcp_dupl_info_present      = false;
  bool                      ehc_params_present              = false;
  bool                      udc_params_present              = false;
  bool                      discard_timer_extended_present  = false;
  bool                      psi_based_discard_timer_present = false;
  bool                      pdcp_sn_gap_report_present      = false;
  pdcp_status_report_ind_e  pdcp_status_report_ind;
  add_pdcp_dupl_info_e      add_pdcp_dupl_info;
  ehc_params_s              ehc_params;
  udc_params_s              udc_params;
  discard_timer_extended_e  discard_timer_extended;
  psi_based_discard_timer_e psi_based_discard_timer;
  pdcp_sn_gap_report_e      pdcp_sn_gap_report;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-Configuration ::= SEQUENCE
struct pdcp_cfg_s {
  bool                       ext                           = false;
  bool                       rohc_params_present           = false;
  bool                       t_reordering_timer_present    = false;
  bool                       discard_timer_present         = false;
  bool                       ul_data_split_thres_present   = false;
  bool                       pdcp_dupl_present             = false;
  bool                       pdcp_reest_present            = false;
  bool                       pdcp_data_recovery_present    = false;
  bool                       dupl_activation_present       = false;
  bool                       out_of_order_delivery_present = false;
  bool                       ie_exts_present               = false;
  pdcp_sn_size_e             pdcp_sn_size_ul;
  pdcp_sn_size_e             pdcp_sn_size_dl;
  rlc_mode_e                 rlc_mode;
  rohc_params_c              rohc_params;
  t_reordering_timer_s       t_reordering_timer;
  discard_timer_e            discard_timer;
  ul_data_split_thres_e      ul_data_split_thres;
  pdcp_dupl_e                pdcp_dupl;
  pdcp_reest_e               pdcp_reest;
  pdcp_data_recovery_e       pdcp_data_recovery;
  dupl_activation_e          dupl_activation;
  out_of_order_delivery_e    out_of_order_delivery;
  pdcp_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoS-Flow-QoS-Parameter-List ::= SEQUENCE (SIZE (1..64)) OF QoS-Flow-QoS-Parameter-Item
using qos_flow_qos_param_list_l = dyn_array<qos_flow_qos_param_item_s>;

// BCBearerContextNGU-TNLInfoat5GC-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using bc_bearer_context_ngu_tnl_infoat5_gc_ext_ies_o = protocol_ies_empty_o;

// BCBearerContextNGU-TNLInfoatNGRAN-Request ::= CHOICE
struct bc_bearer_context_ngu_tnl_infoat_ngran_request_c {
  struct types_opts {
    enum options { locationindependent, locationdependent, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  bc_bearer_context_ngu_tnl_infoat_ngran_request_c() = default;
  bc_bearer_context_ngu_tnl_infoat_ngran_request_c(const bc_bearer_context_ngu_tnl_infoat_ngran_request_c& other);
  bc_bearer_context_ngu_tnl_infoat_ngran_request_c&
  operator=(const bc_bearer_context_ngu_tnl_infoat_ngran_request_c& other);
  ~bc_bearer_context_ngu_tnl_infoat_ngran_request_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  mb_sn_gu_info_at_ngran_request_e& locationindependent()
  {
    assert_choice_type(types::locationindependent, type_, "BCBearerContextNGU-TNLInfoatNGRAN-Request");
    return c.get<mb_sn_gu_info_at_ngran_request_e>();
  }
  mb_sn_gu_info_at_ngran_request_list_l& locationdependent()
  {
    assert_choice_type(types::locationdependent, type_, "BCBearerContextNGU-TNLInfoatNGRAN-Request");
    return c.get<mb_sn_gu_info_at_ngran_request_list_l>();
  }
  protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_request_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "BCBearerContextNGU-TNLInfoatNGRAN-Request");
    return c.get<protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_request_ext_ies_o>>();
  }
  const mb_sn_gu_info_at_ngran_request_e& locationindependent() const
  {
    assert_choice_type(types::locationindependent, type_, "BCBearerContextNGU-TNLInfoatNGRAN-Request");
    return c.get<mb_sn_gu_info_at_ngran_request_e>();
  }
  const mb_sn_gu_info_at_ngran_request_list_l& locationdependent() const
  {
    assert_choice_type(types::locationdependent, type_, "BCBearerContextNGU-TNLInfoatNGRAN-Request");
    return c.get<mb_sn_gu_info_at_ngran_request_list_l>();
  }
  const protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_request_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "BCBearerContextNGU-TNLInfoatNGRAN-Request");
    return c.get<protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_request_ext_ies_o>>();
  }
  mb_sn_gu_info_at_ngran_request_e&                                                         set_locationindependent();
  mb_sn_gu_info_at_ngran_request_list_l&                                                    set_locationdependent();
  protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_request_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<mb_sn_gu_info_at_ngran_request_list_l,
                  protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_request_ext_ies_o>>
      c;

  void destroy_();
};

struct bcmrb_modify_cfg_item_ext_ies_container {
  bool                                  f1_u_tnl_info_to_add_or_modify_list_present = false;
  bool                                  f1_u_tnl_info_to_release_list_present       = false;
  f1_u_tnl_info_to_add_or_modify_list_l f1_u_tnl_info_to_add_or_modify_list;
  f1_u_tnl_info_to_release_list_l       f1_u_tnl_info_to_release_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCMRBModifyConfiguration-Item ::= SEQUENCE
struct bcmrb_modify_cfg_item_s {
  bool                                    ext                                          = false;
  bool                                    bc_bearer_context_f1_u_tnl_infoat_du_present = false;
  bool                                    mbs_pdcp_cfg_present                         = false;
  bool                                    qos_flow_level_qos_params_present            = false;
  bool                                    ie_exts_present                              = false;
  uint16_t                                mrb_id                                       = 1;
  bc_bearer_context_f1_u_tnl_infoat_du_c  bc_bearer_context_f1_u_tnl_infoat_du;
  pdcp_cfg_s                              mbs_pdcp_cfg;
  qos_flow_qos_param_list_l               qos_flow_qos_param_list;
  qos_flow_level_qos_params_s             qos_flow_level_qos_params;
  bcmrb_modify_cfg_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCMRBSetupConfiguration-Item ::= SEQUENCE
struct bcmrb_setup_cfg_item_s {
  bool                                                     ext                               = false;
  bool                                                     qos_flow_level_qos_params_present = false;
  uint16_t                                                 mrb_id                            = 1;
  pdcp_cfg_s                                               mbs_pdcp_cfg;
  qos_flow_qos_param_list_l                                qos_flow_qos_param_list;
  qos_flow_level_qos_params_s                              qos_flow_level_qos_params;
  protocol_ext_container_l<bcmrb_setup_cfg_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationDependentMBSNGUInformationAt5GC ::= SEQUENCE (SIZE (1..256)) OF LocationDependentMBSNGUInformationAt5GC-Item
using location_dependent_mb_sn_gu_info_at5_gc_l = dyn_array<location_dependent_mb_sn_gu_info_at5_gc_item_s>;

// BCBearerContextNGU-TNLInfoat5GC ::= CHOICE
struct bc_bearer_context_ngu_tnl_infoat5_gc_c {
  struct types_opts {
    enum options { locationindependent, locationdependent, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  bc_bearer_context_ngu_tnl_infoat5_gc_c() = default;
  bc_bearer_context_ngu_tnl_infoat5_gc_c(const bc_bearer_context_ngu_tnl_infoat5_gc_c& other);
  bc_bearer_context_ngu_tnl_infoat5_gc_c& operator=(const bc_bearer_context_ngu_tnl_infoat5_gc_c& other);
  ~bc_bearer_context_ngu_tnl_infoat5_gc_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  mb_sn_gu_info_at5_gc_c& locationindependent()
  {
    assert_choice_type(types::locationindependent, type_, "BCBearerContextNGU-TNLInfoat5GC");
    return c.get<mb_sn_gu_info_at5_gc_c>();
  }
  location_dependent_mb_sn_gu_info_at5_gc_l& locationdependent()
  {
    assert_choice_type(types::locationdependent, type_, "BCBearerContextNGU-TNLInfoat5GC");
    return c.get<location_dependent_mb_sn_gu_info_at5_gc_l>();
  }
  protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat5_gc_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "BCBearerContextNGU-TNLInfoat5GC");
    return c.get<protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat5_gc_ext_ies_o>>();
  }
  const mb_sn_gu_info_at5_gc_c& locationindependent() const
  {
    assert_choice_type(types::locationindependent, type_, "BCBearerContextNGU-TNLInfoat5GC");
    return c.get<mb_sn_gu_info_at5_gc_c>();
  }
  const location_dependent_mb_sn_gu_info_at5_gc_l& locationdependent() const
  {
    assert_choice_type(types::locationdependent, type_, "BCBearerContextNGU-TNLInfoat5GC");
    return c.get<location_dependent_mb_sn_gu_info_at5_gc_l>();
  }
  const protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat5_gc_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "BCBearerContextNGU-TNLInfoat5GC");
    return c.get<protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat5_gc_ext_ies_o>>();
  }
  mb_sn_gu_info_at5_gc_c&                                                         set_locationindependent();
  location_dependent_mb_sn_gu_info_at5_gc_l&                                      set_locationdependent();
  protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat5_gc_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<location_dependent_mb_sn_gu_info_at5_gc_l,
                  mb_sn_gu_info_at5_gc_c,
                  protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat5_gc_ext_ies_o>>
      c;

  void destroy_();
};

// BCBearerContextToModify-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct bc_bearer_context_to_modify_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { bc_bearer_context_ngu_tnl_infoat_ngran_request, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::bc_bearer_context_ngu_tnl_infoat_ngran_request; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bc_bearer_context_ngu_tnl_infoat_ngran_request_c& bc_bearer_context_ngu_tnl_infoat_ngran_request() { return c; }
    const bc_bearer_context_ngu_tnl_infoat_ngran_request_c& bc_bearer_context_ngu_tnl_infoat_ngran_request() const
    {
      return c;
    }

  private:
    bc_bearer_context_ngu_tnl_infoat_ngran_request_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BCMRBModifyConfiguration ::= SEQUENCE (SIZE (1..32)) OF BCMRBModifyConfiguration-Item
using bcmrb_modify_cfg_l = dyn_array<bcmrb_modify_cfg_item_s>;

// BCMRBRemoveConfiguration ::= SEQUENCE (SIZE (1..32)) OF INTEGER (1..512,...)
using bcmrb_rem_cfg_l = bounded_array<uint16_t, 32>;

// BCMRBSetupConfiguration ::= SEQUENCE (SIZE (1..32)) OF BCMRBSetupConfiguration-Item
using bcmrb_setup_cfg_l = dyn_array<bcmrb_setup_cfg_item_s>;

// BCBearerContextToModify ::= SEQUENCE
struct bc_bearer_context_to_modify_s {
  bool                                                            ext                                          = false;
  bool                                                            bc_bearer_context_ngu_tnl_infoat5_gc_present = false;
  bc_bearer_context_ngu_tnl_infoat5_gc_c                          bc_bearer_context_ngu_tnl_infoat5_gc;
  bcmrb_setup_cfg_l                                               bc_mrb_to_setup_list;
  bcmrb_modify_cfg_l                                              bc_mrb_to_modify_list;
  bcmrb_rem_cfg_l                                                 bc_mrb_to_rem_list;
  protocol_ext_container_l<bc_bearer_context_to_modify_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCBearerContextToModifyRequired-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using bc_bearer_context_to_modify_required_ext_ies_o = protocol_ext_empty_o;

using bc_bearer_context_to_modify_required_ext_ies_container = protocol_ext_container_empty_l;

// BCBearerContextToModifyRequired ::= SEQUENCE
struct bc_bearer_context_to_modify_required_s {
  bool                                                   ext             = false;
  bool                                                   ie_exts_present = false;
  bcmrb_rem_cfg_l                                        bc_mrb_to_rem_list;
  bc_bearer_context_to_modify_required_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ECNMarkingorCongestionInformationReportingStatus ::= ENUMERATED
struct ecn_markingor_congestion_info_report_status_opts {
  enum options { active, not_active, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ecn_markingor_congestion_info_report_status_e =
    enumerated<ecn_markingor_congestion_info_report_status_opts, true>;

// F1U-TNL-InfoAddedOrModified-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using f1_u_tnl_info_added_or_modified_item_ext_ies_o = protocol_ext_empty_o;

using f1_u_tnl_info_added_or_modified_item_ext_ies_container = protocol_ext_container_empty_l;

// F1U-TNL-InfoAddedOrModified-Item ::= SEQUENCE
struct f1_u_tnl_info_added_or_modified_item_s {
  bool                                                   ext                                          = false;
  bool                                                   bc_bearer_context_f1_u_tnl_infoat_cu_present = false;
  bool                                                   ie_exts_present                              = false;
  fixed_octstring<4, true>                               broadcast_f1_u_context_ref_e1;
  bc_bearer_context_f1_u_tnl_infoat_cu_c                 bc_bearer_context_f1_u_tnl_infoat_cu;
  f1_u_tnl_info_added_or_modified_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSNGUInformationAtNGRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using mb_sn_gu_info_at_ngran_ext_ies_o = protocol_ies_empty_o;

// QoS-Flow-Failed-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using qos_flow_failed_item_ext_ies_o = protocol_ext_empty_o;

// QoS-Flow-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct qos_flow_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        qos_flow_map_ind,
        data_forwarding_source_ip_address,
        ecn_markingor_congestion_info_report_status,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_flow_map_ind_e&                                  qos_flow_map_ind();
    bounded_bitstring<1, 160, true, true>&               data_forwarding_source_ip_address();
    ecn_markingor_congestion_info_report_status_e&       ecn_markingor_congestion_info_report_status();
    const qos_flow_map_ind_e&                            qos_flow_map_ind() const;
    const bounded_bitstring<1, 160, true, true>&         data_forwarding_source_ip_address() const;
    const ecn_markingor_congestion_info_report_status_e& ecn_markingor_congestion_info_report_status() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// F1U-TNL-InfoAddedOrModified-List ::= SEQUENCE (SIZE (1..512)) OF F1U-TNL-InfoAddedOrModified-Item
using f1_u_tnl_info_added_or_modified_list_l = dyn_array<f1_u_tnl_info_added_or_modified_item_s>;

// LocationDependentMBSNGUInformationAtNGRAN-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using location_dependent_mb_sn_gu_info_at_ngran_item_ext_ies_o = protocol_ext_empty_o;

// MBSNGUInformationAtNGRAN ::= CHOICE
struct mb_sn_gu_info_at_ngran_c {
  struct types_opts {
    enum options { unicast, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  mb_sn_gu_info_at_ngran_c() = default;
  mb_sn_gu_info_at_ngran_c(const mb_sn_gu_info_at_ngran_c& other);
  mb_sn_gu_info_at_ngran_c& operator=(const mb_sn_gu_info_at_ngran_c& other);
  ~mb_sn_gu_info_at_ngran_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  up_tnl_info_c& unicast()
  {
    assert_choice_type(types::unicast, type_, "MBSNGUInformationAtNGRAN");
    return c.get<up_tnl_info_c>();
  }
  protocol_ie_single_container_s<mb_sn_gu_info_at_ngran_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "MBSNGUInformationAtNGRAN");
    return c.get<protocol_ie_single_container_s<mb_sn_gu_info_at_ngran_ext_ies_o>>();
  }
  const up_tnl_info_c& unicast() const
  {
    assert_choice_type(types::unicast, type_, "MBSNGUInformationAtNGRAN");
    return c.get<up_tnl_info_c>();
  }
  const protocol_ie_single_container_s<mb_sn_gu_info_at_ngran_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "MBSNGUInformationAtNGRAN");
    return c.get<protocol_ie_single_container_s<mb_sn_gu_info_at_ngran_ext_ies_o>>();
  }
  up_tnl_info_c&                                                    set_unicast();
  protocol_ie_single_container_s<mb_sn_gu_info_at_ngran_ext_ies_o>& set_choice_ext();

private:
  types                                                                                            type_;
  choice_buffer_t<protocol_ie_single_container_s<mb_sn_gu_info_at_ngran_ext_ies_o>, up_tnl_info_c> c;

  void destroy_();
};

using qos_flow_failed_item_ext_ies_container = protocol_ext_container_empty_l;

// QoS-Flow-Failed-Item ::= SEQUENCE
struct qos_flow_failed_item_s {
  bool                                   ext             = false;
  bool                                   ie_exts_present = false;
  uint8_t                                qos_flow_id     = 0;
  cause_c                                cause;
  qos_flow_failed_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct qos_flow_item_ext_ies_container {
  bool                                          qos_flow_map_ind_present                            = false;
  bool                                          data_forwarding_source_ip_address_present           = false;
  bool                                          ecn_markingor_congestion_info_report_status_present = false;
  qos_flow_map_ind_e                            qos_flow_map_ind;
  bounded_bitstring<1, 160, true, true>         data_forwarding_source_ip_address;
  ecn_markingor_congestion_info_report_status_e ecn_markingor_congestion_info_report_status;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoS-Flow-Item ::= SEQUENCE
struct qos_flow_item_s {
  bool                            ext             = false;
  bool                            ie_exts_present = false;
  uint8_t                         qos_flow_id     = 0;
  qos_flow_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCMRBFailedList-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using bcmrb_failed_list_item_ext_ies_o = protocol_ext_empty_o;

// BCMRBSetupModifyResponseList-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct bcmrb_setup_modify_resp_list_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { f1_u_tnl_info_added_or_modified_list, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::f1_u_tnl_info_added_or_modified_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    f1_u_tnl_info_added_or_modified_list_l&       f1_u_tnl_info_added_or_modified_list() { return c; }
    const f1_u_tnl_info_added_or_modified_list_l& f1_u_tnl_info_added_or_modified_list() const { return c; }

  private:
    f1_u_tnl_info_added_or_modified_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using location_dependent_mb_sn_gu_info_at_ngran_item_ext_ies_container = protocol_ext_container_empty_l;

// LocationDependentMBSNGUInformationAtNGRAN-Item ::= SEQUENCE
struct location_dependent_mb_sn_gu_info_at_ngran_item_s {
  bool                                                             ext                 = false;
  bool                                                             ie_exts_present     = false;
  uint32_t                                                         mbs_area_session_id = 0;
  mb_sn_gu_info_at_ngran_c                                         mbs_ngu_info_at_ngran;
  location_dependent_mb_sn_gu_info_at_ngran_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoS-Flow-Failed-List ::= SEQUENCE (SIZE (1..64)) OF QoS-Flow-Failed-Item
using qos_flow_failed_list_l = dyn_array<qos_flow_failed_item_s>;

// QoS-Flow-List ::= SEQUENCE (SIZE (1..64)) OF QoS-Flow-Item
using qos_flow_list_l = dyn_array<qos_flow_item_s>;

// BCBearerContextNGU-TNLInfoatNGRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using bc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o = protocol_ies_empty_o;

using bcmrb_failed_list_item_ext_ies_container = protocol_ext_container_empty_l;

// BCMRBFailedList-Item ::= SEQUENCE
struct bcmrb_failed_list_item_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  uint16_t                                 mrb_id          = 1;
  cause_c                                  cause;
  bcmrb_failed_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCMRBSetupModifyResponseList-Item ::= SEQUENCE
struct bcmrb_setup_modify_resp_list_item_s {
  bool                                   ext                                          = false;
  bool                                   bc_bearer_context_f1_u_tnl_infoat_cu_present = false;
  uint16_t                               mrb_id                                       = 1;
  qos_flow_list_l                        qosflow_setup;
  qos_flow_failed_list_l                 qosflow_failed;
  bc_bearer_context_f1_u_tnl_infoat_cu_c bc_bearer_context_f1_u_tnl_infoat_cu;
  protocol_ext_container_l<bcmrb_setup_modify_resp_list_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationDependentMBSNGUInformationAtNGRAN ::= SEQUENCE (SIZE (1..256)) OF
// LocationDependentMBSNGUInformationAtNGRAN-Item
using location_dependent_mb_sn_gu_info_at_ngran_l = dyn_array<location_dependent_mb_sn_gu_info_at_ngran_item_s>;

// BCBearerContextNGU-TNLInfoatNGRAN ::= CHOICE
struct bc_bearer_context_ngu_tnl_infoat_ngran_c {
  struct types_opts {
    enum options { locationindependent, locationdependent, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  bc_bearer_context_ngu_tnl_infoat_ngran_c() = default;
  bc_bearer_context_ngu_tnl_infoat_ngran_c(const bc_bearer_context_ngu_tnl_infoat_ngran_c& other);
  bc_bearer_context_ngu_tnl_infoat_ngran_c& operator=(const bc_bearer_context_ngu_tnl_infoat_ngran_c& other);
  ~bc_bearer_context_ngu_tnl_infoat_ngran_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  mb_sn_gu_info_at_ngran_c& locationindependent()
  {
    assert_choice_type(types::locationindependent, type_, "BCBearerContextNGU-TNLInfoatNGRAN");
    return c.get<mb_sn_gu_info_at_ngran_c>();
  }
  location_dependent_mb_sn_gu_info_at_ngran_l& locationdependent()
  {
    assert_choice_type(types::locationdependent, type_, "BCBearerContextNGU-TNLInfoatNGRAN");
    return c.get<location_dependent_mb_sn_gu_info_at_ngran_l>();
  }
  protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "BCBearerContextNGU-TNLInfoatNGRAN");
    return c.get<protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>>();
  }
  const mb_sn_gu_info_at_ngran_c& locationindependent() const
  {
    assert_choice_type(types::locationindependent, type_, "BCBearerContextNGU-TNLInfoatNGRAN");
    return c.get<mb_sn_gu_info_at_ngran_c>();
  }
  const location_dependent_mb_sn_gu_info_at_ngran_l& locationdependent() const
  {
    assert_choice_type(types::locationdependent, type_, "BCBearerContextNGU-TNLInfoatNGRAN");
    return c.get<location_dependent_mb_sn_gu_info_at_ngran_l>();
  }
  const protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "BCBearerContextNGU-TNLInfoatNGRAN");
    return c.get<protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>>();
  }
  mb_sn_gu_info_at_ngran_c&                                                         set_locationindependent();
  location_dependent_mb_sn_gu_info_at_ngran_l&                                      set_locationdependent();
  protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<location_dependent_mb_sn_gu_info_at_ngran_l,
                  mb_sn_gu_info_at_ngran_c,
                  protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>>
      c;

  void destroy_();
};

// BCBearerContextToModifyResponse-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using bc_bearer_context_to_modify_resp_ext_ies_o = protocol_ext_empty_o;

// BCMRBFailedList ::= SEQUENCE (SIZE (1..32)) OF BCMRBFailedList-Item
using bcmrb_failed_list_l = dyn_array<bcmrb_failed_list_item_s>;

// BCMRBSetupModifyResponseList ::= SEQUENCE (SIZE (1..32)) OF BCMRBSetupModifyResponseList-Item
using bcmrb_setup_modify_resp_list_l = dyn_array<bcmrb_setup_modify_resp_list_item_s>;

using bc_bearer_context_to_modify_resp_ext_ies_container = protocol_ext_container_empty_l;

// BCBearerContextToModifyResponse ::= SEQUENCE
struct bc_bearer_context_to_modify_resp_s {
  bool                                               ext                                            = false;
  bool                                               bc_bearer_context_ngu_tnl_infoat_ngran_present = false;
  bool                                               ie_exts_present                                = false;
  bc_bearer_context_ngu_tnl_infoat_ngran_c           bc_bearer_context_ngu_tnl_infoat_ngran;
  bcmrb_setup_modify_resp_list_l                     bc_mrb_setup_modify_resp_list;
  bcmrb_failed_list_l                                bc_mrb_failed_list;
  bcmrb_setup_cfg_l                                  available_bcmrb_cfg;
  bc_bearer_context_to_modify_resp_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-ServiceAreaTAIList-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mbs_service_area_tai_list_item_ext_ies_o = protocol_ext_empty_o;

// NR-CGI-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using nr_cgi_ext_ies_o = protocol_ext_empty_o;

using mbs_service_area_tai_list_item_ext_ies_container = protocol_ext_container_empty_l;

// MBS-ServiceAreaTAIList-Item ::= SEQUENCE
struct mbs_service_area_tai_list_item_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  fixed_octstring<3, true>                         plmn_id;
  fixed_octstring<3, true>                         five_gs_tac;
  mbs_service_area_tai_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using nr_cgi_ext_ies_container = protocol_ext_container_empty_l;

// NR-CGI ::= SEQUENCE
struct nr_cgi_s {
  bool                             ie_exts_present = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<36, false, true> nr_cell_id;
  nr_cgi_ext_ies_container         ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-ServiceAreaCellList ::= SEQUENCE (SIZE (1..512)) OF NR-CGI
using mbs_service_area_cell_list_l = dyn_array<nr_cgi_s>;

// MBS-ServiceAreaInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mbs_service_area_info_ext_ies_o = protocol_ext_empty_o;

// MBS-ServiceAreaTAIList ::= SEQUENCE (SIZE (1..512)) OF MBS-ServiceAreaTAIList-Item
using mbs_service_area_tai_list_l = dyn_array<mbs_service_area_tai_list_item_s>;

using mbs_service_area_info_ext_ies_container = protocol_ext_container_empty_l;

// MBS-ServiceAreaInformation ::= SEQUENCE
struct mbs_service_area_info_s {
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  mbs_service_area_cell_list_l            mbs_service_area_cell_list;
  mbs_service_area_tai_list_l             mbs_service_area_tai_list;
  mbs_service_area_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-ServiceAreaInformationItem-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mbs_service_area_info_item_ext_ies_o = protocol_ext_empty_o;

using mbs_service_area_info_item_ext_ies_container = protocol_ext_container_empty_l;

// MBS-ServiceAreaInformationItem ::= SEQUENCE
struct mbs_service_area_info_item_s {
  bool                                         ext                 = false;
  bool                                         ie_exts_present     = false;
  uint32_t                                     mbs_area_session_id = 0;
  mbs_service_area_info_s                      mbs_service_area_info;
  mbs_service_area_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SNSSAI-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using snssai_ext_ies_o = protocol_ext_empty_o;

// BCBearerContextToSetup-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using bc_bearer_context_to_setup_ext_ies_o = protocol_ext_empty_o;

// GlobalMBSSessionID-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using global_mbs_session_id_ext_ies_o = protocol_ext_empty_o;

// MBS-ServiceArea-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mbs_service_area_ext_ies_o = protocol_ext_empty_o;

// MBS-ServiceAreaInformationList ::= SEQUENCE (SIZE (1..256)) OF MBS-ServiceAreaInformationItem
using mbs_service_area_info_list_l = dyn_array<mbs_service_area_info_item_s>;

// RequestedAction4AvailNGUTermination ::= ENUMERATED
struct requested_action4_avail_ngu_termination_opts {
  enum options {
    apply_available_cfg,
    apply_requested_cfg,
    /*...*/ apply_available_cfg_if_same_as_requested,
    nulltype
  } value;

  const char* to_string() const;
};
using requested_action4_avail_ngu_termination_e = enumerated<requested_action4_avail_ngu_termination_opts, true, 1>;

using snssai_ext_ies_container = protocol_ext_container_empty_l;

// SNSSAI ::= SEQUENCE
struct snssai_s {
  bool                     ext             = false;
  bool                     sd_present      = false;
  bool                     ie_exts_present = false;
  fixed_octstring<1, true> sst;
  fixed_octstring<3, true> sd;
  snssai_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using bc_bearer_context_to_setup_ext_ies_container = protocol_ext_container_empty_l;

// BCBearerContextToSetup ::= SEQUENCE
struct bc_bearer_context_to_setup_s {
  bool                                         ext                                          = false;
  bool                                         bc_bearer_context_ngu_tnl_infoat5_gc_present = false;
  bool                                         requested_action_present                     = false;
  bool                                         ie_exts_present                              = false;
  snssai_s                                     snssai;
  bc_bearer_context_ngu_tnl_infoat5_gc_c       bc_bearer_context_ngu_tnl_infoat5_gc;
  bcmrb_setup_cfg_l                            bc_mrb_to_setup_list;
  requested_action4_avail_ngu_termination_e    requested_action;
  bc_bearer_context_to_setup_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using global_mbs_session_id_ext_ies_container = protocol_ext_container_empty_l;

// GlobalMBSSessionID ::= SEQUENCE
struct global_mbs_session_id_s {
  bool                                    ext             = false;
  bool                                    nid_present     = false;
  bool                                    ie_exts_present = false;
  fixed_octstring<6, true>                tmgi;
  fixed_bitstring<44, false, true>        nid;
  global_mbs_session_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mbs_service_area_ext_ies_container = protocol_ext_container_empty_l;

// MBS-ServiceArea ::= SEQUENCE
struct mbs_service_area_s {
  bool                               ext             = false;
  bool                               ie_exts_present = false;
  mbs_service_area_info_list_l       mbs_service_area_info_list;
  mbs_service_area_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1U-TNL-InfoAdded-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using f1_u_tnl_info_added_item_ext_ies_o = protocol_ext_empty_o;

using f1_u_tnl_info_added_item_ext_ies_container = protocol_ext_container_empty_l;

// F1U-TNL-InfoAdded-Item ::= SEQUENCE
struct f1_u_tnl_info_added_item_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  fixed_octstring<4, true>                   broadcast_f1_u_context_ref_e1;
  bc_bearer_context_f1_u_tnl_infoat_cu_c     bc_bearer_context_f1_u_tnl_infoat_cu;
  f1_u_tnl_info_added_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1U-TNL-InfoAdded-List ::= SEQUENCE (SIZE (1..512)) OF F1U-TNL-InfoAdded-Item
using f1_u_tnl_info_added_list_l = dyn_array<f1_u_tnl_info_added_item_s>;

// BCMRBSetupResponseList-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct bcmrb_setup_resp_list_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { f1_u_tnl_info_added_list, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::f1_u_tnl_info_added_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    f1_u_tnl_info_added_list_l&       f1_u_tnl_info_added_list() { return c; }
    const f1_u_tnl_info_added_list_l& f1_u_tnl_info_added_list() const { return c; }

  private:
    f1_u_tnl_info_added_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BCMRBSetupResponseList-Item ::= SEQUENCE
struct bcmrb_setup_resp_list_item_s {
  bool                                                           ext    = false;
  uint16_t                                                       mrb_id = 1;
  qos_flow_list_l                                                qosflow_setup;
  qos_flow_failed_list_l                                         qosflow_failed;
  bc_bearer_context_f1_u_tnl_infoat_cu_c                         bc_bearer_context_f1_u_tnl_infoat_cu;
  protocol_ext_container_l<bcmrb_setup_resp_list_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCBearerContextToSetupResponse-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using bc_bearer_context_to_setup_resp_ext_ies_o = protocol_ext_empty_o;

// BCMRBSetupResponseList ::= SEQUENCE (SIZE (1..32)) OF BCMRBSetupResponseList-Item
using bcmrb_setup_resp_list_l = dyn_array<bcmrb_setup_resp_list_item_s>;

using bc_bearer_context_to_setup_resp_ext_ies_container = protocol_ext_container_empty_l;

// BCBearerContextToSetupResponse ::= SEQUENCE
struct bc_bearer_context_to_setup_resp_s {
  bool                                              ext                                            = false;
  bool                                              bc_bearer_context_ngu_tnl_infoat_ngran_present = false;
  bool                                              ie_exts_present                                = false;
  bc_bearer_context_ngu_tnl_infoat_ngran_c          bc_bearer_context_ngu_tnl_infoat_ngran;
  bcmrb_setup_resp_list_l                           bc_mrb_setup_resp_list;
  bcmrb_failed_list_l                               bc_mrb_failed_list;
  bcmrb_setup_cfg_l                                 available_bcmrb_cfg;
  bc_bearer_context_to_setup_resp_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cell-Group-Information-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct cell_group_info_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { nof_tunnels, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::nof_tunnels; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&       nof_tunnels() { return c; }
    const uint8_t& nof_tunnels() const { return c; }

  private:
    uint8_t c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DL-TX-Stop ::= ENUMERATED
struct dl_tx_stop_opts {
  enum options { stop, resume, /*...*/ nulltype } value;

  const char* to_string() const;
};
using dl_tx_stop_e = enumerated<dl_tx_stop_opts, true>;

// RAT-Type ::= ENUMERATED
struct rat_type_opts {
  enum options { e_utra, nr, /*...*/ nulltype } value;

  const char* to_string() const;
};
using rat_type_e = enumerated<rat_type_opts, true>;

// UL-Configuration ::= ENUMERATED
struct ul_cfg_opts {
  enum options { no_data, shared, only, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ul_cfg_e = enumerated<ul_cfg_opts, true>;

// Cell-Group-Information-Item ::= SEQUENCE
struct cell_group_info_item_s {
  bool                                                     ext                = false;
  bool                                                     ul_cfg_present     = false;
  bool                                                     dl_tx_stop_present = false;
  bool                                                     rat_type_present   = false;
  uint8_t                                                  cell_group_id      = 0;
  ul_cfg_e                                                 ul_cfg;
  dl_tx_stop_e                                             dl_tx_stop;
  rat_type_e                                               rat_type;
  protocol_ext_container_l<cell_group_info_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cell-Group-Information ::= SEQUENCE (SIZE (1..4)) OF Cell-Group-Information-Item
using cell_group_info_l = dyn_array<cell_group_info_item_s>;

// DRB-Confirm-Modified-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_confirm_modified_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

using drb_confirm_modified_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Confirm-Modified-Item-NG-RAN ::= SEQUENCE
struct drb_confirm_modified_item_ng_ran_s {
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  uint8_t                                            drb_id          = 1;
  cell_group_info_l                                  cell_group_info;
  drb_confirm_modified_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Confirm-Modified-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_confirm_modified_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-Confirm-Modified-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Confirm-Modified-Item-NG-RAN
using drb_confirm_modified_list_ng_ran_l = dyn_array<drb_confirm_modified_item_ng_ran_s>;

// PDU-Session-Resource-Confirm-Modified-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdu_session_res_confirm_modified_item_ext_ies_o = protocol_ext_empty_o;

using drb_confirm_modified_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Confirm-Modified-Item-EUTRAN ::= SEQUENCE
struct drb_confirm_modified_item_eutran_s {
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  uint8_t                                            drb_id          = 1;
  cell_group_info_l                                  cell_group_info;
  drb_confirm_modified_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_confirm_modified_item_ext_ies_container = protocol_ext_container_empty_l;

// PDU-Session-Resource-Confirm-Modified-Item ::= SEQUENCE
struct pdu_session_res_confirm_modified_item_s {
  bool                                                    ext             = false;
  bool                                                    ie_exts_present = false;
  uint16_t                                                pdu_session_id  = 0;
  drb_confirm_modified_list_ng_ran_l                      drb_confirm_modified_list_ng_ran;
  pdu_session_res_confirm_modified_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Confirm-Modified-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Confirm-Modified-Item-EUTRAN
using drb_confirm_modified_list_eutran_l = dyn_array<drb_confirm_modified_item_eutran_s>;

// PDU-Session-Resource-Confirm-Modified-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Confirm-Modified-Item
using pdu_session_res_confirm_modified_list_l = dyn_array<pdu_session_res_confirm_modified_item_s>;

// DataForwardingtoNG-RANQoSFlowInformationList-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using data_forwardingto_ng_ran_qos_flow_info_list_item_ext_ies_o = protocol_ext_empty_o;

// PDCP-Count-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdcp_count_ext_ies_o = protocol_ext_empty_o;

// DLDiscarding-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using dl_discarding_ext_ies_o = protocol_ext_empty_o;

using data_forwardingto_ng_ran_qos_flow_info_list_item_ext_ies_container = protocol_ext_container_empty_l;

// DataForwardingtoNG-RANQoSFlowInformationList-Item ::= SEQUENCE
struct data_forwardingto_ng_ran_qos_flow_info_list_item_s {
  bool                                                               ext             = false;
  bool                                                               ie_exts_present = false;
  uint8_t                                                            qos_flow_id     = 0;
  data_forwardingto_ng_ran_qos_flow_info_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FirstDLCount-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using first_dl_count_ext_ies_o = protocol_ext_empty_o;

// IndirectPathIndication ::= ENUMERATED
struct indirect_path_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using indirect_path_ind_e = enumerated<indirect_path_ind_opts, true>;

using pdcp_count_ext_ies_container = protocol_ext_container_empty_l;

// PDCP-Count ::= SEQUENCE
struct pdcp_count_s {
  bool                         ext             = false;
  bool                         ie_exts_present = false;
  uint32_t                     pdcp_sn         = 0;
  uint64_t                     hfn             = 0;
  pdcp_count_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoS-Flow-Mapping-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using qos_flow_map_item_ext_ies_o = protocol_ext_empty_o;

// QoS-Flows-to-be-forwarded-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using qos_flows_to_be_forwarded_item_ext_ies_o = protocol_ext_empty_o;

// QoS-Mapping-Information ::= SEQUENCE
struct qos_map_info_s {
  bool                             ext                = false;
  bool                             dscp_present       = false;
  bool                             flow_label_present = false;
  fixed_bitstring<6, false, true>  dscp;
  fixed_bitstring<20, false, true> flow_label;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DAPSRequestInfo-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using daps_request_info_ext_ies_o = protocol_ext_empty_o;

using dl_discarding_ext_ies_container = protocol_ext_container_empty_l;

// DLDiscarding ::= SEQUENCE
struct dl_discarding_s {
  bool                            ie_exts_present = false;
  pdcp_count_s                    dl_discarding_count_val;
  dl_discarding_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBBStatusTransfer-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_b_status_transfer_ext_ies_o = protocol_ext_empty_o;

// DataForwardingtoNG-RANQoSFlowInformationList ::= SEQUENCE (SIZE (1..64)) OF
// DataForwardingtoNG-RANQoSFlowInformationList-Item
using data_forwardingto_ng_ran_qos_flow_info_list_l = dyn_array<data_forwardingto_ng_ran_qos_flow_info_list_item_s>;

// EarlyForwardingCOUNTInfo-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using early_forwarding_count_info_ext_ies_o = protocol_ies_empty_o;

using first_dl_count_ext_ies_container = protocol_ext_container_empty_l;

// FirstDLCount ::= SEQUENCE
struct first_dl_count_s {
  bool                             ie_exts_present = false;
  pdcp_count_s                     first_dl_count_val;
  first_dl_count_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSetbasedHandlingIndicator ::= ENUMERATED
struct pdu_setbased_handling_ind_opts {
  enum options { supported, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdu_setbased_handling_ind_e = enumerated<pdu_setbased_handling_ind_opts, true>;

using qos_flow_map_item_ext_ies_container = protocol_ext_container_empty_l;

// QoS-Flow-Mapping-Item ::= SEQUENCE
struct qos_flow_map_item_s {
  bool                                ext                      = false;
  bool                                qos_flow_map_ind_present = false;
  bool                                ie_exts_present          = false;
  uint8_t                             qos_flow_id              = 0;
  qos_flow_map_ind_e                  qos_flow_map_ind;
  qos_flow_map_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using qos_flows_to_be_forwarded_item_ext_ies_container = protocol_ext_container_empty_l;

// QoS-Flows-to-be-forwarded-Item ::= SEQUENCE
struct qos_flows_to_be_forwarded_item_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  uint8_t                                          qos_flow_id     = 0;
  qos_flows_to_be_forwarded_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UP-Parameters-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct up_params_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { qos_map_info, indirect_path_ind, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_map_info_s&            qos_map_info();
    indirect_path_ind_e&       indirect_path_ind();
    const qos_map_info_s&      qos_map_info() const;
    const indirect_path_ind_e& indirect_path_ind() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using daps_request_info_ext_ies_container = protocol_ext_container_empty_l;

// DAPSRequestInfo ::= SEQUENCE
struct daps_request_info_s {
  struct daps_ind_opts {
    enum options { daps_ho_required, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using daps_ind_e_ = enumerated<daps_ind_opts, true>;

  // member variables
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  daps_ind_e_                         daps_ind;
  daps_request_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_b_status_transfer_ext_ies_container = protocol_ext_container_empty_l;

// DRBBStatusTransfer ::= SEQUENCE
struct drb_b_status_transfer_s {
  bool                                      ext                               = false;
  bool                                      receive_statusof_pdcp_sdu_present = false;
  bool                                      ie_ext_present                    = false;
  bounded_bitstring<1, 131072, false, true> receive_statusof_pdcp_sdu;
  pdcp_count_s                              count_value;
  drb_b_status_transfer_ext_ies_container   ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Data-Forwarding-Information-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct data_forwarding_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { data_forwardingto_ng_ran_qos_flow_info_list, pdu_setbased_handling_ind, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    data_forwardingto_ng_ran_qos_flow_info_list_l&       data_forwardingto_ng_ran_qos_flow_info_list();
    pdu_setbased_handling_ind_e&                         pdu_setbased_handling_ind();
    const data_forwardingto_ng_ran_qos_flow_info_list_l& data_forwardingto_ng_ran_qos_flow_info_list() const;
    const pdu_setbased_handling_ind_e&                   pdu_setbased_handling_ind() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Data-Forwarding-Information-Request-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using data_forwarding_info_request_ext_ies_o = protocol_ext_empty_o;

// Data-Forwarding-Request ::= ENUMERATED
struct data_forwarding_request_opts {
  enum options { ul, dl, both, /*...*/ nulltype } value;

  const char* to_string() const;
};
using data_forwarding_request_e = enumerated<data_forwarding_request_opts, true>;

// DataForwardingtoE-UTRANInformationListItem-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using data_forwardingto_e_utran_info_list_item_ext_ies_o = protocol_ext_empty_o;

// DefaultDRB ::= ENUMERATED
struct default_drb_opts {
  enum options { true_value, false_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using default_drb_e = enumerated<default_drb_opts, true>;

// EarlyDataForwardingIndicator ::= ENUMERATED
struct early_data_forwarding_ind_opts {
  enum options { stop, /*...*/ nulltype } value;

  const char* to_string() const;
};
using early_data_forwarding_ind_e = enumerated<early_data_forwarding_ind_opts, true>;

// EarlyForwardingCOUNTInfo ::= CHOICE
struct early_forwarding_count_info_c {
  struct types_opts {
    enum options { first_dl_count, dl_discarding_count, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  early_forwarding_count_info_c() = default;
  early_forwarding_count_info_c(const early_forwarding_count_info_c& other);
  early_forwarding_count_info_c& operator=(const early_forwarding_count_info_c& other);
  ~early_forwarding_count_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  first_dl_count_s& first_dl_count()
  {
    assert_choice_type(types::first_dl_count, type_, "EarlyForwardingCOUNTInfo");
    return c.get<first_dl_count_s>();
  }
  dl_discarding_s& dl_discarding_count()
  {
    assert_choice_type(types::dl_discarding_count, type_, "EarlyForwardingCOUNTInfo");
    return c.get<dl_discarding_s>();
  }
  protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "EarlyForwardingCOUNTInfo");
    return c.get<protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>>();
  }
  const first_dl_count_s& first_dl_count() const
  {
    assert_choice_type(types::first_dl_count, type_, "EarlyForwardingCOUNTInfo");
    return c.get<first_dl_count_s>();
  }
  const dl_discarding_s& dl_discarding_count() const
  {
    assert_choice_type(types::dl_discarding_count, type_, "EarlyForwardingCOUNTInfo");
    return c.get<dl_discarding_s>();
  }
  const protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "EarlyForwardingCOUNTInfo");
    return c.get<protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>>();
  }
  first_dl_count_s&                                                      set_first_dl_count();
  dl_discarding_s&                                                       set_dl_discarding_count();
  protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<dl_discarding_s,
                  first_dl_count_s,
                  protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>>
      c;

  void destroy_();
};

// EarlyForwardingCOUNTReq ::= ENUMERATED
struct early_forwarding_count_req_opts {
  enum options { first_dl_count, dl_discarding, /*...*/ nulltype } value;

  const char* to_string() const;
};
using early_forwarding_count_req_e = enumerated<early_forwarding_count_req_opts, true>;

// IgnoreMappingRuleIndication ::= ENUMERATED
struct ignore_map_rule_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ignore_map_rule_ind_e = enumerated<ignore_map_rule_ind_opts, true>;

// MaxIPrate ::= ENUMERATED
struct max_ip_rate_opts {
  enum options { bitrate64kbs, max_ue_rate, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using max_ip_rate_e = enumerated<max_ip_rate_opts, true>;

// MaximumIPdatarate-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using max_ip_datarate_ext_ies_o = protocol_ext_empty_o;

// PDCP-COUNT-Reset ::= ENUMERATED
struct pdcp_count_reset_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdcp_count_reset_e = enumerated<pdcp_count_reset_opts, true>;

// PDCP-SN-Status-Information-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdcp_sn_status_info_ext_ies_o = protocol_ext_empty_o;

// QoS-Flow-Mapping-List ::= SEQUENCE (SIZE (1..64)) OF QoS-Flow-Mapping-Item
using qos_flow_map_list_l = dyn_array<qos_flow_map_item_s>;

// QoS-Flows-DRB-Remapping ::= ENUMERATED
struct qos_flows_drb_remap_opts {
  enum options { upd, source_cfg, /*...*/ nulltype } value;

  const char* to_string() const;
};
using qos_flows_drb_remap_e = enumerated<qos_flows_drb_remap_opts, true>;

// QoS-Flows-to-be-forwarded-List ::= SEQUENCE (SIZE (1..64)) OF QoS-Flows-to-be-forwarded-Item
using qos_flows_to_be_forwarded_list_l = dyn_array<qos_flows_to_be_forwarded_item_s>;

// SDAP-Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using sdap_cfg_ext_ies_o = protocol_ext_empty_o;

// SDAP-Header-DL ::= ENUMERATED
struct sdap_hdr_dl_opts {
  enum options { present, absent, /*...*/ nulltype } value;

  const char* to_string() const;
};
using sdap_hdr_dl_e = enumerated<sdap_hdr_dl_opts, true>;

// SDAP-Header-UL ::= ENUMERATED
struct sdap_hdr_ul_opts {
  enum options { present, absent, /*...*/ nulltype } value;

  const char* to_string() const;
};
using sdap_hdr_ul_e = enumerated<sdap_hdr_ul_opts, true>;

// SDTindicatorMod ::= ENUMERATED
struct sd_tind_mod_opts {
  enum options { true_value, false_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using sd_tind_mod_e = enumerated<sd_tind_mod_opts, true>;

// SDTindicatorSetup ::= ENUMERATED
struct sd_tind_setup_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using sd_tind_setup_e = enumerated<sd_tind_setup_opts, true>;

// SpecialTriggeringPurpose ::= ENUMERATED
struct special_trigger_purpose_opts {
  enum options { indirect_data_forwarding, /*...*/ nulltype } value;

  const char* to_string() const;
};
using special_trigger_purpose_e = enumerated<special_trigger_purpose_opts, true>;

struct up_params_item_ext_ies_container {
  bool                qos_map_info_present      = false;
  bool                indirect_path_ind_present = false;
  qos_map_info_s      qos_map_info;
  indirect_path_ind_e indirect_path_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UP-Parameters-Item ::= SEQUENCE
struct up_params_item_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  up_tnl_info_c                    up_tnl_info;
  uint8_t                          cell_group_id = 0;
  up_params_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ConfidentialityProtectionIndication ::= ENUMERATED
struct confidentiality_protection_ind_opts {
  enum options { required, preferred, not_needed, /*...*/ nulltype } value;

  const char* to_string() const;
};
using confidentiality_protection_ind_e = enumerated<confidentiality_protection_ind_opts, true>;

// DRB-To-Modify-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct drb_to_modify_item_ng_ran_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        old_qos_flow_map_ul_endmarkerexpected,
        drb_qos,
        early_forwarding_count_req,
        early_forwarding_count_info,
        daps_request_info,
        early_data_forwarding_ind,
        sd_tind_mod,
        pdcp_count_reset,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_flow_list_l&                     old_qos_flow_map_ul_endmarkerexpected();
    qos_flow_level_qos_params_s&         drb_qos();
    early_forwarding_count_req_e&        early_forwarding_count_req();
    early_forwarding_count_info_c&       early_forwarding_count_info();
    daps_request_info_s&                 daps_request_info();
    early_data_forwarding_ind_e&         early_data_forwarding_ind();
    sd_tind_mod_e&                       sd_tind_mod();
    pdcp_count_reset_e&                  pdcp_count_reset();
    const qos_flow_list_l&               old_qos_flow_map_ul_endmarkerexpected() const;
    const qos_flow_level_qos_params_s&   drb_qos() const;
    const early_forwarding_count_req_e&  early_forwarding_count_req() const;
    const early_forwarding_count_info_c& early_forwarding_count_info() const;
    const daps_request_info_s&           daps_request_info() const;
    const early_data_forwarding_ind_e&   early_data_forwarding_ind() const;
    const sd_tind_mod_e&                 sd_tind_mod() const;
    const pdcp_count_reset_e&            pdcp_count_reset() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-To-Remove-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_to_rem_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

// DRB-To-Setup-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct drb_to_setup_item_ng_ran_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        drb_qos,
        daps_request_info,
        ignore_map_rule_ind,
        qos_flows_drb_remap,
        sd_tind_setup,
        special_trigger_purpose,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_flow_level_qos_params_s&       drb_qos();
    daps_request_info_s&               daps_request_info();
    ignore_map_rule_ind_e&             ignore_map_rule_ind();
    qos_flows_drb_remap_e&             qos_flows_drb_remap();
    sd_tind_setup_e&                   sd_tind_setup();
    special_trigger_purpose_e&         special_trigger_purpose();
    const qos_flow_level_qos_params_s& drb_qos() const;
    const daps_request_info_s&         daps_request_info() const;
    const ignore_map_rule_ind_e&       ignore_map_rule_ind() const;
    const qos_flows_drb_remap_e&       qos_flows_drb_remap() const;
    const sd_tind_setup_e&             sd_tind_setup() const;
    const special_trigger_purpose_e&   special_trigger_purpose() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-To-Setup-Mod-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct drb_to_setup_mod_item_ng_ran_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        drb_qos,
        ignore_map_rule_ind,
        daps_request_info,
        sd_tind_setup,
        special_trigger_purpose,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_flow_level_qos_params_s&       drb_qos();
    ignore_map_rule_ind_e&             ignore_map_rule_ind();
    daps_request_info_s&               daps_request_info();
    sd_tind_setup_e&                   sd_tind_setup();
    special_trigger_purpose_e&         special_trigger_purpose();
    const qos_flow_level_qos_params_s& drb_qos() const;
    const ignore_map_rule_ind_e&       ignore_map_rule_ind() const;
    const daps_request_info_s&         daps_request_info() const;
    const sd_tind_setup_e&             sd_tind_setup() const;
    const special_trigger_purpose_e&   special_trigger_purpose() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct data_forwarding_info_ext_ies_container {
  bool                                          data_forwardingto_ng_ran_qos_flow_info_list_present = false;
  bool                                          pdu_setbased_handling_ind_present                   = false;
  data_forwardingto_ng_ran_qos_flow_info_list_l data_forwardingto_ng_ran_qos_flow_info_list;
  pdu_setbased_handling_ind_e                   pdu_setbased_handling_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Data-Forwarding-Information ::= SEQUENCE
struct data_forwarding_info_s {
  bool                                   ext                        = false;
  bool                                   ul_data_forwarding_present = false;
  bool                                   dl_data_forwarding_present = false;
  bool                                   ie_exts_present            = false;
  up_tnl_info_c                          ul_data_forwarding;
  up_tnl_info_c                          dl_data_forwarding;
  data_forwarding_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using data_forwarding_info_request_ext_ies_container = protocol_ext_container_empty_l;

// Data-Forwarding-Information-Request ::= SEQUENCE
struct data_forwarding_info_request_s {
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  data_forwarding_request_e                      data_forwarding_request;
  qos_flow_map_list_l                            qos_flows_forwarded_on_fwd_tunnels;
  data_forwarding_info_request_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using data_forwardingto_e_utran_info_list_item_ext_ies_container = protocol_ext_container_empty_l;

// DataForwardingtoE-UTRANInformationListItem ::= SEQUENCE
struct data_forwardingto_e_utran_info_list_item_s {
  bool                                                       ext             = false;
  bool                                                       ie_exts_present = false;
  up_tnl_info_c                                              data_forwarding_tunnel_info;
  qos_flows_to_be_forwarded_list_l                           qos_flows_to_be_forwarded_list;
  data_forwardingto_e_utran_info_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRANAllocationAndRetentionPriority-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using eutran_alloc_and_retention_prio_ext_ies_o = protocol_ext_empty_o;

// GBR-QosInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gbr_qos_info_ext_ies_o = protocol_ext_empty_o;

// IntegrityProtectionIndication ::= ENUMERATED
struct integrity_protection_ind_opts {
  enum options { required, preferred, not_needed, /*...*/ nulltype } value;

  const char* to_string() const;
};
using integrity_protection_ind_e = enumerated<integrity_protection_ind_opts, true>;

using max_ip_datarate_ext_ies_container = protocol_ext_container_empty_l;

// MaximumIPdatarate ::= SEQUENCE
struct max_ip_datarate_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  max_ip_rate_e                     max_ip_rate;
  max_ip_datarate_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdcp_sn_status_info_ext_ies_container = protocol_ext_container_empty_l;

// PDCP-SN-Status-Information ::= SEQUENCE
struct pdcp_sn_status_info_s {
  bool                                  ext            = false;
  bool                                  ie_ext_present = false;
  drb_b_status_transfer_s               pdcp_status_transfer_ul;
  pdcp_count_s                          pdcp_status_transfer_dl;
  pdcp_sn_status_info_ext_ies_container ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-SN-Status-Request ::= ENUMERATED
struct pdcp_sn_status_request_opts {
  enum options { requested, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdcp_sn_status_request_e = enumerated<pdcp_sn_status_request_opts, true>;

using sdap_cfg_ext_ies_container = protocol_ext_container_empty_l;

// SDAP-Configuration ::= SEQUENCE
struct sdap_cfg_s {
  bool                       ext             = false;
  bool                       ie_exts_present = false;
  default_drb_e              default_drb;
  sdap_hdr_ul_e              sdap_hdr_ul;
  sdap_hdr_dl_e              sdap_hdr_dl;
  sdap_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityIndication-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using security_ind_ext_ies_o = protocol_ext_empty_o;

// UP-Parameters ::= SEQUENCE (SIZE (1..8)) OF UP-Parameters-Item
using up_params_l = dyn_array<up_params_item_s>;

// UserPlaneFailureIndication-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using user_plane_fail_ind_ext_ies_o = protocol_ext_empty_o;

// UserPlaneFailureType ::= ENUMERATED
struct user_plane_fail_type_opts {
  enum options { gtp_u_error_ind_rx, up_path_fail, /*...*/ nulltype } value;

  const char* to_string() const;
};
using user_plane_fail_type_e = enumerated<user_plane_fail_type_opts, true>;

struct drb_to_modify_item_ng_ran_ext_ies_container {
  bool                          old_qos_flow_map_ul_endmarkerexpected_present = false;
  bool                          drb_qos_present                               = false;
  bool                          early_forwarding_count_req_present            = false;
  bool                          early_forwarding_count_info_present           = false;
  bool                          daps_request_info_present                     = false;
  bool                          early_data_forwarding_ind_present             = false;
  bool                          sd_tind_mod_present                           = false;
  bool                          pdcp_count_reset_present                      = false;
  qos_flow_list_l               old_qos_flow_map_ul_endmarkerexpected;
  qos_flow_level_qos_params_s   drb_qos;
  early_forwarding_count_req_e  early_forwarding_count_req;
  early_forwarding_count_info_c early_forwarding_count_info;
  daps_request_info_s           daps_request_info;
  early_data_forwarding_ind_e   early_data_forwarding_ind;
  sd_tind_mod_e                 sd_tind_mod;
  pdcp_count_reset_e            pdcp_count_reset;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-To-Modify-Item-NG-RAN ::= SEQUENCE
struct drb_to_modify_item_ng_ran_s {
  bool                                        ext                              = false;
  bool                                        sdap_cfg_present                 = false;
  bool                                        pdcp_cfg_present                 = false;
  bool                                        drb_data_forwarding_info_present = false;
  bool                                        pdcp_sn_status_request_present   = false;
  bool                                        pdcp_sn_status_info_present      = false;
  bool                                        drb_inactivity_timer_present     = false;
  bool                                        ie_exts_present                  = false;
  uint8_t                                     drb_id                           = 1;
  sdap_cfg_s                                  sdap_cfg;
  pdcp_cfg_s                                  pdcp_cfg;
  data_forwarding_info_s                      drb_data_forwarding_info;
  pdcp_sn_status_request_e                    pdcp_sn_status_request;
  pdcp_sn_status_info_s                       pdcp_sn_status_info;
  up_params_l                                 dl_up_params;
  cell_group_info_l                           cell_group_to_add;
  cell_group_info_l                           cell_group_to_modify;
  cell_group_info_l                           cell_group_to_rem;
  qos_flow_qos_param_list_l                   flow_map_info;
  uint16_t                                    drb_inactivity_timer = 1;
  drb_to_modify_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_to_rem_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-To-Remove-Item-NG-RAN ::= SEQUENCE
struct drb_to_rem_item_ng_ran_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  uint8_t                                  drb_id          = 1;
  drb_to_rem_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct drb_to_setup_item_ng_ran_ext_ies_container {
  bool                        drb_qos_present                 = false;
  bool                        daps_request_info_present       = false;
  bool                        ignore_map_rule_ind_present     = false;
  bool                        qos_flows_drb_remap_present     = false;
  bool                        sd_tind_setup_present           = false;
  bool                        special_trigger_purpose_present = false;
  qos_flow_level_qos_params_s drb_qos;
  daps_request_info_s         daps_request_info;
  ignore_map_rule_ind_e       ignore_map_rule_ind;
  qos_flows_drb_remap_e       qos_flows_drb_remap;
  sd_tind_setup_e             sd_tind_setup;
  special_trigger_purpose_e   special_trigger_purpose;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-To-Setup-Item-NG-RAN ::= SEQUENCE
struct drb_to_setup_item_ng_ran_s {
  bool                                       ext                                      = false;
  bool                                       drb_data_forwarding_info_request_present = false;
  bool                                       drb_inactivity_timer_present             = false;
  bool                                       pdcp_sn_status_info_present              = false;
  bool                                       ie_exts_present                          = false;
  uint8_t                                    drb_id                                   = 1;
  sdap_cfg_s                                 sdap_cfg;
  pdcp_cfg_s                                 pdcp_cfg;
  cell_group_info_l                          cell_group_info;
  qos_flow_qos_param_list_l                  qos_flow_info_to_be_setup;
  data_forwarding_info_request_s             drb_data_forwarding_info_request;
  uint16_t                                   drb_inactivity_timer = 1;
  pdcp_sn_status_info_s                      pdcp_sn_status_info;
  drb_to_setup_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct drb_to_setup_mod_item_ng_ran_ext_ies_container {
  bool                        drb_qos_present                 = false;
  bool                        ignore_map_rule_ind_present     = false;
  bool                        daps_request_info_present       = false;
  bool                        sd_tind_setup_present           = false;
  bool                        special_trigger_purpose_present = false;
  qos_flow_level_qos_params_s drb_qos;
  ignore_map_rule_ind_e       ignore_map_rule_ind;
  daps_request_info_s         daps_request_info;
  sd_tind_setup_e             sd_tind_setup;
  special_trigger_purpose_e   special_trigger_purpose;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-To-Setup-Mod-Item-NG-RAN ::= SEQUENCE
struct drb_to_setup_mod_item_ng_ran_s {
  bool                                           ext                                      = false;
  bool                                           drb_data_forwarding_info_request_present = false;
  bool                                           drb_inactivity_timer_present             = false;
  bool                                           pdcp_sn_status_info_present              = false;
  bool                                           ie_exts_present                          = false;
  uint8_t                                        drb_id                                   = 1;
  sdap_cfg_s                                     sdap_cfg;
  pdcp_cfg_s                                     pdcp_cfg;
  cell_group_info_l                              cell_group_info;
  qos_flow_qos_param_list_l                      flow_map_info;
  data_forwarding_info_request_s                 drb_data_forwarding_info_request;
  uint16_t                                       drb_inactivity_timer = 1;
  pdcp_sn_status_info_s                          pdcp_sn_status_info;
  drb_to_setup_mod_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DataForwardingtoE-UTRANInformationList ::= SEQUENCE (SIZE (1..256)) OF DataForwardingtoE-UTRANInformationListItem
using data_forwardingto_e_utran_info_list_l = dyn_array<data_forwardingto_e_utran_info_list_item_s>;

// EUTRAN-QoS-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using eutran_qos_ext_ies_o = protocol_ext_empty_o;

using eutran_alloc_and_retention_prio_ext_ies_container = protocol_ext_container_empty_l;

// EUTRANAllocationAndRetentionPriority ::= SEQUENCE
struct eutran_alloc_and_retention_prio_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  uint8_t                                           prio_level      = 0;
  pre_emption_cap_e                                 pre_emption_cap;
  pre_emption_vulnerability_e                       pre_emption_vulnerability;
  eutran_alloc_and_retention_prio_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gbr_qos_info_ext_ies_container = protocol_ext_container_empty_l;

// GBR-QosInformation ::= SEQUENCE
struct gbr_qos_info_s {
  bool                           ext                        = false;
  bool                           ie_exts_present            = false;
  uint64_t                       erab_max_bitrate_dl        = 0;
  uint64_t                       erab_max_bitrate_ul        = 0;
  uint64_t                       erab_guaranteed_bitrate_dl = 0;
  uint64_t                       erab_guaranteed_bitrate_ul = 0;
  gbr_qos_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using security_ind_ext_ies_container = protocol_ext_container_empty_l;

// SecurityIndication ::= SEQUENCE
struct security_ind_s {
  bool                             ext                     = false;
  bool                             max_ip_datarate_present = false;
  bool                             ie_exts_present         = false;
  integrity_protection_ind_e       integrity_protection_ind;
  confidentiality_protection_ind_e confidentiality_protection_ind;
  max_ip_datarate_s                max_ip_datarate;
  security_ind_ext_ies_container   ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UserPlaneErrorIndicator ::= ENUMERATED
struct user_plane_error_ind_opts {
  enum options { gtp_u_error_ind_rx, /*...*/ nulltype } value;

  const char* to_string() const;
};
using user_plane_error_ind_e = enumerated<user_plane_error_ind_opts, true>;

using user_plane_fail_ind_ext_ies_container = protocol_ext_container_empty_l;

// UserPlaneFailureIndication ::= SEQUENCE
struct user_plane_fail_ind_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  user_plane_fail_type_e                user_plane_fail_type;
  up_tnl_info_c                         ng_dl_up_tnl_info;
  up_tnl_info_c                         ng_ul_up_tnl_info;
  user_plane_fail_ind_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-To-Modify-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_to_modify_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-To-Modify-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-To-Modify-Item-NG-RAN
using drb_to_modify_list_ng_ran_l = dyn_array<drb_to_modify_item_ng_ran_s>;

// DRB-To-Remove-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_to_rem_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-To-Remove-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-To-Remove-Item-NG-RAN
using drb_to_rem_list_ng_ran_l = dyn_array<drb_to_rem_item_ng_ran_s>;

// DRB-To-Setup-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-To-Setup-Item-NG-RAN
using drb_to_setup_list_ng_ran_l = dyn_array<drb_to_setup_item_ng_ran_s>;

// DRB-To-Setup-Mod-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct drb_to_setup_mod_item_eutran_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { security_ind, data_forwarding_source_ip_address, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    security_ind_s&                              security_ind();
    bounded_bitstring<1, 160, true, true>&       data_forwarding_source_ip_address();
    const security_ind_s&                        security_ind() const;
    const bounded_bitstring<1, 160, true, true>& data_forwarding_source_ip_address() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-To-Setup-Mod-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-To-Setup-Mod-Item-NG-RAN
using drb_to_setup_mod_list_ng_ran_l = dyn_array<drb_to_setup_mod_item_ng_ran_s>;

using eutran_qos_ext_ies_container = protocol_ext_container_empty_l;

// EUTRAN-QoS ::= SEQUENCE
struct eutran_qos_s {
  bool                              ext                  = false;
  bool                              gbr_qos_info_present = false;
  bool                              ie_exts_present      = false;
  uint16_t                          qci                  = 0;
  eutran_alloc_and_retention_prio_s eutran_alloc_and_retention_prio;
  gbr_qos_info_s                    gbr_qos_info;
  eutran_qos_ext_ies_container      ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-To-Modify-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdu_session_res_to_modify_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        snssai,
        common_network_instance,
        redundant_n_g_ul_up_tnl_info,
        redundant_common_network_instance,
        data_forwardingto_e_utran_info_list,
        security_ind_modify,
        secondary_pdu_session_data_forwarding_info,
        user_plane_fail_ind,
        special_trigger_purpose,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    snssai_s&                                    snssai();
    unbounded_octstring<true>&                   common_network_instance();
    up_tnl_info_c&                               redundant_n_g_ul_up_tnl_info();
    unbounded_octstring<true>&                   redundant_common_network_instance();
    data_forwardingto_e_utran_info_list_l&       data_forwardingto_e_utran_info_list();
    security_ind_s&                              security_ind_modify();
    data_forwarding_info_s&                      secondary_pdu_session_data_forwarding_info();
    user_plane_fail_ind_s&                       user_plane_fail_ind();
    special_trigger_purpose_e&                   special_trigger_purpose();
    const snssai_s&                              snssai() const;
    const unbounded_octstring<true>&             common_network_instance() const;
    const up_tnl_info_c&                         redundant_n_g_ul_up_tnl_info() const;
    const unbounded_octstring<true>&             redundant_common_network_instance() const;
    const data_forwardingto_e_utran_info_list_l& data_forwardingto_e_utran_info_list() const;
    const security_ind_s&                        security_ind_modify() const;
    const data_forwarding_info_s&                secondary_pdu_session_data_forwarding_info() const;
    const user_plane_fail_ind_s&                 user_plane_fail_ind() const;
    const special_trigger_purpose_e&             special_trigger_purpose() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDU-Session-Resource-To-Remove-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdu_session_res_to_rem_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { cause, user_plane_error_ind, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&                      cause();
    user_plane_error_ind_e&       user_plane_error_ind();
    const cause_c&                cause() const;
    const user_plane_error_ind_e& user_plane_error_ind() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDU-Session-Resource-To-Setup-Mod-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdu_session_res_to_setup_mod_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        network_instance,
        common_network_instance,
        redundant_n_g_ul_up_tnl_info,
        redundant_common_network_instance,
        special_trigger_purpose,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                        network_instance();
    unbounded_octstring<true>&       common_network_instance();
    up_tnl_info_c&                   redundant_n_g_ul_up_tnl_info();
    unbounded_octstring<true>&       redundant_common_network_instance();
    special_trigger_purpose_e&       special_trigger_purpose();
    const uint16_t&                  network_instance() const;
    const unbounded_octstring<true>& common_network_instance() const;
    const up_tnl_info_c&             redundant_n_g_ul_up_tnl_info() const;
    const unbounded_octstring<true>& redundant_common_network_instance() const;
    const special_trigger_purpose_e& special_trigger_purpose() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDU-Session-Type ::= ENUMERATED
struct pdu_session_type_opts {
  enum options { ipv4, ipv6, ipv4v6, ethernet, unstructured, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdu_session_type_e = enumerated<pdu_session_type_opts, true>;

using drb_to_modify_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-To-Modify-Item-EUTRAN ::= SEQUENCE
struct drb_to_modify_item_eutran_s {
  bool                                        ext                            = false;
  bool                                        pdcp_cfg_present               = false;
  bool                                        eutran_qos_present             = false;
  bool                                        s1_ul_up_tnl_info_present      = false;
  bool                                        data_forwarding_info_present   = false;
  bool                                        pdcp_sn_status_request_present = false;
  bool                                        pdcp_sn_status_info_present    = false;
  bool                                        drb_inactivity_timer_present   = false;
  bool                                        ie_exts_present                = false;
  uint8_t                                     drb_id                         = 1;
  pdcp_cfg_s                                  pdcp_cfg;
  eutran_qos_s                                eutran_qos;
  up_tnl_info_c                               s1_ul_up_tnl_info;
  data_forwarding_info_s                      data_forwarding_info;
  pdcp_sn_status_request_e                    pdcp_sn_status_request;
  pdcp_sn_status_info_s                       pdcp_sn_status_info;
  up_params_l                                 dl_up_params;
  cell_group_info_l                           cell_group_to_add;
  cell_group_info_l                           cell_group_to_modify;
  cell_group_info_l                           cell_group_to_rem;
  uint16_t                                    drb_inactivity_timer = 1;
  drb_to_modify_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_to_rem_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-To-Remove-Item-EUTRAN ::= SEQUENCE
struct drb_to_rem_item_eutran_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  uint8_t                                  drb_id          = 1;
  drb_to_rem_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct drb_to_setup_mod_item_eutran_ext_ies_container {
  bool                                  security_ind_present                      = false;
  bool                                  data_forwarding_source_ip_address_present = false;
  security_ind_s                        security_ind;
  bounded_bitstring<1, 160, true, true> data_forwarding_source_ip_address;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-To-Setup-Mod-Item-EUTRAN ::= SEQUENCE
struct drb_to_setup_mod_item_eutran_s {
  bool                                           ext                                  = false;
  bool                                           data_forwarding_info_request_present = false;
  bool                                           drb_inactivity_timer_present         = false;
  bool                                           ie_exts_present                      = false;
  uint8_t                                        drb_id                               = 1;
  pdcp_cfg_s                                     pdcp_cfg;
  eutran_qos_s                                   eutran_qos;
  up_tnl_info_c                                  s1_ul_up_tnl_info;
  data_forwarding_info_request_s                 data_forwarding_info_request;
  cell_group_info_l                              cell_group_info;
  up_params_l                                    dl_up_params;
  uint16_t                                       drb_inactivity_timer = 1;
  drb_to_setup_mod_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct pdu_session_res_to_modify_item_ext_ies_container {
  bool                                  snssai_present                                     = false;
  bool                                  common_network_instance_present                    = false;
  bool                                  redundant_n_g_ul_up_tnl_info_present               = false;
  bool                                  redundant_common_network_instance_present          = false;
  bool                                  data_forwardingto_e_utran_info_list_present        = false;
  bool                                  security_ind_modify_present                        = false;
  bool                                  secondary_pdu_session_data_forwarding_info_present = false;
  bool                                  user_plane_fail_ind_present                        = false;
  bool                                  special_trigger_purpose_present                    = false;
  snssai_s                              snssai;
  unbounded_octstring<true>             common_network_instance;
  up_tnl_info_c                         redundant_n_g_ul_up_tnl_info;
  unbounded_octstring<true>             redundant_common_network_instance;
  data_forwardingto_e_utran_info_list_l data_forwardingto_e_utran_info_list;
  security_ind_s                        security_ind_modify;
  data_forwarding_info_s                secondary_pdu_session_data_forwarding_info;
  user_plane_fail_ind_s                 user_plane_fail_ind;
  special_trigger_purpose_e             special_trigger_purpose;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-To-Modify-Item ::= SEQUENCE
struct pdu_session_res_to_modify_item_s {
  bool                                             ext                                              = false;
  bool                                             security_ind_present                             = false;
  bool                                             pdu_session_res_dl_ambr_present                  = false;
  bool                                             ng_ul_up_tnl_info_present                        = false;
  bool                                             pdu_session_data_forwarding_info_request_present = false;
  bool                                             pdu_session_data_forwarding_info_present         = false;
  bool                                             pdu_session_inactivity_timer_present             = false;
  bool                                             network_instance_present                         = false;
  bool                                             ie_exts_present                                  = false;
  uint16_t                                         pdu_session_id                                   = 0;
  security_ind_s                                   security_ind;
  uint64_t                                         pdu_session_res_dl_ambr = 0;
  up_tnl_info_c                                    ng_ul_up_tnl_info;
  data_forwarding_info_request_s                   pdu_session_data_forwarding_info_request;
  data_forwarding_info_s                           pdu_session_data_forwarding_info;
  uint16_t                                         pdu_session_inactivity_timer = 1;
  uint16_t                                         network_instance             = 1;
  drb_to_setup_list_ng_ran_l                       drb_to_setup_list_ng_ran;
  drb_to_modify_list_ng_ran_l                      drb_to_modify_list_ng_ran;
  drb_to_rem_list_ng_ran_l                         drb_to_rem_list_ng_ran;
  pdu_session_res_to_modify_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct pdu_session_res_to_rem_item_ext_ies_container {
  bool                   cause_present                = false;
  bool                   user_plane_error_ind_present = false;
  cause_c                cause;
  user_plane_error_ind_e user_plane_error_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-To-Remove-Item ::= SEQUENCE
struct pdu_session_res_to_rem_item_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  uint16_t                                      pdu_session_id  = 0;
  pdu_session_res_to_rem_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct pdu_session_res_to_setup_mod_item_ext_ies_container {
  bool                      network_instance_present                  = false;
  bool                      common_network_instance_present           = false;
  bool                      redundant_n_g_ul_up_tnl_info_present      = false;
  bool                      redundant_common_network_instance_present = false;
  bool                      special_trigger_purpose_present           = false;
  uint16_t                  network_instance;
  unbounded_octstring<true> common_network_instance;
  up_tnl_info_c             redundant_n_g_ul_up_tnl_info;
  unbounded_octstring<true> redundant_common_network_instance;
  special_trigger_purpose_e special_trigger_purpose;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-To-Setup-Mod-Item ::= SEQUENCE
struct pdu_session_res_to_setup_mod_item_s {
  bool                                                ext                                              = false;
  bool                                                pdu_session_res_ambr_present                     = false;
  bool                                                pdu_session_data_forwarding_info_request_present = false;
  bool                                                pdu_session_inactivity_timer_present             = false;
  bool                                                ie_exts_present                                  = false;
  uint16_t                                            pdu_session_id                                   = 0;
  pdu_session_type_e                                  pdu_session_type;
  snssai_s                                            snssai;
  security_ind_s                                      security_ind;
  uint64_t                                            pdu_session_res_ambr = 0;
  up_tnl_info_c                                       ng_ul_up_tnl_info;
  data_forwarding_info_request_s                      pdu_session_data_forwarding_info_request;
  uint16_t                                            pdu_session_inactivity_timer = 1;
  drb_to_setup_mod_list_ng_ran_l                      drb_to_setup_mod_list_ng_ran;
  pdu_session_res_to_setup_mod_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CipheringAlgorithm ::= ENUMERATED
struct ciphering_algorithm_opts {
  enum options { nea0, c_neg128_nea1, c_neg128_nea2, c_neg128_nea3, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ciphering_algorithm_e = enumerated<ciphering_algorithm_opts, true>;

// DRB-To-Modify-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-To-Modify-Item-EUTRAN
using drb_to_modify_list_eutran_l = dyn_array<drb_to_modify_item_eutran_s>;

// DRB-To-Remove-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-To-Remove-Item-EUTRAN
using drb_to_rem_list_eutran_l = dyn_array<drb_to_rem_item_eutran_s>;

// DRB-To-Setup-Mod-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-To-Setup-Mod-Item-EUTRAN
using drb_to_setup_mod_list_eutran_l = dyn_array<drb_to_setup_mod_item_eutran_s>;

// IntegrityProtectionAlgorithm ::= ENUMERATED
struct integrity_protection_algorithm_opts {
  enum options { nia0, i_neg128_nia1, i_neg128_nia2, i_neg128_nia3, /*...*/ nulltype } value;

  const char* to_string() const;
};
using integrity_protection_algorithm_e = enumerated<integrity_protection_algorithm_opts, true>;

// PDU-Session-Resource-To-Modify-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-To-Modify-Item
using pdu_session_res_to_modify_list_l = dyn_array<pdu_session_res_to_modify_item_s>;

// PDU-Session-Resource-To-Remove-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-To-Remove-Item
using pdu_session_res_to_rem_list_l = dyn_array<pdu_session_res_to_rem_item_s>;

// PDU-Session-Resource-To-Setup-Mod-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-To-Setup-Mod-Item
using pdu_session_res_to_setup_mod_list_l = dyn_array<pdu_session_res_to_setup_mod_item_s>;

// SecurityAlgorithm-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using security_algorithm_ext_ies_o = protocol_ext_empty_o;

// UESliceMaximumBitRateItem-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ue_slice_max_bit_rate_item_ext_ies_o = protocol_ext_empty_o;

// UPSecuritykey-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using up_securitykey_ext_ies_o = protocol_ext_empty_o;

using security_algorithm_ext_ies_container = protocol_ext_container_empty_l;

// SecurityAlgorithm ::= SEQUENCE
struct security_algorithm_s {
  bool                                 ext                                    = false;
  bool                                 integrity_protection_algorithm_present = false;
  bool                                 ie_exts_present                        = false;
  ciphering_algorithm_e                ciphering_algorithm;
  integrity_protection_algorithm_e     integrity_protection_algorithm;
  security_algorithm_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using security_info_ext_ies_o = protocol_ext_empty_o;

using ue_slice_max_bit_rate_item_ext_ies_container = protocol_ext_container_empty_l;

// UESliceMaximumBitRateItem ::= SEQUENCE
struct ue_slice_max_bit_rate_item_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  snssai_s                                     snssai;
  uint64_t                                     ue_slice_max_bit_rate_dl = 0;
  ue_slice_max_bit_rate_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using up_securitykey_ext_ies_container = protocol_ext_container_empty_l;

// UPSecuritykey ::= SEQUENCE
struct up_securitykey_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  unbounded_octstring<true>        encryption_key;
  unbounded_octstring<true>        integrity_protection_key;
  up_securitykey_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ActivityNotificationLevel ::= ENUMERATED
struct activity_notif_level_opts {
  enum options { drb, pdu_session, ue, /*...*/ nulltype } value;

  const char* to_string() const;
};
using activity_notif_level_e = enumerated<activity_notif_level_opts, true>;

// BearerContextStatusChange ::= ENUMERATED
struct bearer_context_status_change_opts {
  enum options { suspend, resume, /*...*/ resumefor_sdt, nulltype } value;

  const char* to_string() const;
};
using bearer_context_status_change_e = enumerated<bearer_context_status_change_opts, true, 1>;

// DataDiscardRequired ::= ENUMERATED
struct data_discard_required_opts {
  enum options { required, /*...*/ nulltype } value;

  const char* to_string() const;
};
using data_discard_required_e = enumerated<data_discard_required_opts, true>;

// InactivityInformationRequest ::= ENUMERATED
struct inactivity_info_request_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using inactivity_info_request_e = enumerated<inactivity_info_request_opts, true>;

// MDTPLMNModificationList ::= SEQUENCE (SIZE (0..16)) OF OCTET STRING (SIZE (3))
using mdt_plmn_mod_list_l = bounded_array<fixed_octstring<3, true>, 16>;

// MDTPollutedMeasurementIndicator ::= ENUMERATED
struct mdt_polluted_meas_ind_opts {
  enum options { id_c, no_id_c, /*...*/ nulltype } value;

  const char* to_string() const;
};
using mdt_polluted_meas_ind_e = enumerated<mdt_polluted_meas_ind_opts, true>;

// MT-SDT-Information-Request ::= ENUMERATED
struct mt_sdt_info_request_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using mt_sdt_info_request_e = enumerated<mt_sdt_info_request_opts, true>;

// New-UL-TNL-Information-Required ::= ENUMERATED
struct new_ul_tnl_info_required_opts {
  enum options { required, /*...*/ nulltype } value;

  const char* to_string() const;
};
using new_ul_tnl_info_required_e = enumerated<new_ul_tnl_info_required_opts, true>;

// SCGActivationStatus ::= ENUMERATED
struct scg_activation_status_opts {
  enum options { scg_activ, scg_deactiv, /*...*/ nulltype } value;

  const char* to_string() const;
};
using scg_activation_status_e = enumerated<scg_activation_status_opts, true>;

// SDTContinueROHC ::= ENUMERATED
struct sdt_continue_rohc_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using sdt_continue_rohc_e = enumerated<sdt_continue_rohc_opts, true>;

using security_info_ext_ies_container = protocol_ext_container_empty_l;

// SecurityInformation ::= SEQUENCE
struct security_info_s {
  bool                            ext             = false;
  bool                            ie_exts_present = false;
  security_algorithm_s            security_algorithm;
  up_securitykey_s                up_securitykey;
  security_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UESliceMaximumBitRateList ::= SEQUENCE (SIZE (1..8)) OF UESliceMaximumBitRateItem
using ue_slice_max_bit_rate_list_l = dyn_array<ue_slice_max_bit_rate_item_s>;

// GNB-CU-UP-CellGroupRelatedConfiguration-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gnb_cu_up_cell_group_related_cfg_item_ext_ies_o = protocol_ext_empty_o;

using gnb_cu_up_cell_group_related_cfg_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-UP-CellGroupRelatedConfiguration-Item ::= SEQUENCE
struct gnb_cu_up_cell_group_related_cfg_item_s {
  bool                                                    ul_cfg_present  = false;
  bool                                                    ie_exts_present = false;
  uint8_t                                                 cell_group_id   = 0;
  up_tnl_info_c                                           up_tnl_info;
  ul_cfg_e                                                ul_cfg;
  gnb_cu_up_cell_group_related_cfg_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Required-To-Modify-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_required_to_modify_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

// DRB-Required-To-Remove-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_required_to_rem_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

// GNB-CU-UP-CellGroupRelatedConfiguration ::= SEQUENCE (SIZE (1..8)) OF GNB-CU-UP-CellGroupRelatedConfiguration-Item
using gnb_cu_up_cell_group_related_cfg_l = dyn_array<gnb_cu_up_cell_group_related_cfg_item_s>;

using drb_required_to_modify_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Required-To-Modify-Item-NG-RAN ::= SEQUENCE
struct drb_required_to_modify_item_ng_ran_s {
  bool                                                 ext             = false;
  bool                                                 cause_present   = false;
  bool                                                 ie_exts_present = false;
  uint8_t                                              drb_id          = 1;
  gnb_cu_up_cell_group_related_cfg_l                   gnb_cu_up_cell_group_related_cfg;
  qos_flow_list_l                                      flow_to_rem;
  cause_c                                              cause;
  drb_required_to_modify_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_required_to_rem_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Required-To-Remove-Item-NG-RAN ::= SEQUENCE
struct drb_required_to_rem_item_ng_ran_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  uint8_t                                           drb_id          = 1;
  cause_c                                           cause;
  drb_required_to_rem_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Required-To-Modify-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_required_to_modify_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-Required-To-Modify-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Required-To-Modify-Item-NG-RAN
using drb_required_to_modify_list_ng_ran_l = dyn_array<drb_required_to_modify_item_ng_ran_s>;

// DRB-Required-To-Remove-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_required_to_rem_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-Required-To-Remove-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Required-To-Remove-Item-NG-RAN
using drb_required_to_rem_list_ng_ran_l = dyn_array<drb_required_to_rem_item_ng_ran_s>;

// PDU-Session-Resource-Required-To-Modify-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdu_session_res_required_to_modify_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { redundant_n_g_dl_up_tnl_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::redundant_n_g_dl_up_tnl_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    up_tnl_info_c&       redundant_n_g_dl_up_tnl_info() { return c; }
    const up_tnl_info_c& redundant_n_g_dl_up_tnl_info() const { return c; }

  private:
    up_tnl_info_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using drb_required_to_modify_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Required-To-Modify-Item-EUTRAN ::= SEQUENCE
struct drb_required_to_modify_item_eutran_s {
  bool                                                 ext                       = false;
  bool                                                 s1_dl_up_tnl_info_present = false;
  bool                                                 cause_present             = false;
  bool                                                 ie_exts_present           = false;
  uint8_t                                              drb_id                    = 1;
  up_tnl_info_c                                        s1_dl_up_tnl_info;
  gnb_cu_up_cell_group_related_cfg_l                   gnb_cu_up_cell_group_related_cfg;
  cause_c                                              cause;
  drb_required_to_modify_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_required_to_rem_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Required-To-Remove-Item-EUTRAN ::= SEQUENCE
struct drb_required_to_rem_item_eutran_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  uint8_t                                           drb_id          = 1;
  cause_c                                           cause;
  drb_required_to_rem_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-Required-To-Modify-Item ::= SEQUENCE
struct pdu_session_res_required_to_modify_item_s {
  bool                                                                        ext                       = false;
  bool                                                                        ng_dl_up_tnl_info_present = false;
  uint16_t                                                                    pdu_session_id            = 0;
  up_tnl_info_c                                                               ng_dl_up_tnl_info;
  drb_required_to_modify_list_ng_ran_l                                        drb_required_to_modify_list_ng_ran;
  drb_required_to_rem_list_ng_ran_l                                           drb_required_to_rem_list_ng_ran;
  protocol_ext_container_l<pdu_session_res_required_to_modify_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Required-To-Modify-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Required-To-Modify-Item-EUTRAN
using drb_required_to_modify_list_eutran_l = dyn_array<drb_required_to_modify_item_eutran_s>;

// DRB-Required-To-Remove-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Required-To-Remove-Item-EUTRAN
using drb_required_to_rem_list_eutran_l = dyn_array<drb_required_to_rem_item_eutran_s>;

// PDU-Session-Resource-Required-To-Modify-List ::= SEQUENCE (SIZE (1..256)) OF
// PDU-Session-Resource-Required-To-Modify-Item
using pdu_session_res_required_to_modify_list_l = dyn_array<pdu_session_res_required_to_modify_item_s>;

// ConfidentialityProtectionResult ::= ENUMERATED
struct confidentiality_protection_result_opts {
  enum options { performed, not_performed, /*...*/ nulltype } value;

  const char* to_string() const;
};
using confidentiality_protection_result_e = enumerated<confidentiality_protection_result_opts, true>;

// DRB-Failed-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_failed_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

// DRB-Failed-Mod-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_failed_mod_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

// DRB-Failed-To-Modify-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_failed_to_modify_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

// DRB-Modified-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct drb_modified_item_ng_ran_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { early_forwarding_count_info, old_qos_flow_map_ul_endmarkerexpected, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    early_forwarding_count_info_c&       early_forwarding_count_info();
    qos_flow_list_l&                     old_qos_flow_map_ul_endmarkerexpected();
    const early_forwarding_count_info_c& early_forwarding_count_info() const;
    const qos_flow_list_l&               old_qos_flow_map_ul_endmarkerexpected() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-Setup-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_setup_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

// DRB-Setup-Mod-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_setup_mod_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

// IntegrityProtectionResult ::= ENUMERATED
struct integrity_protection_result_opts {
  enum options { performed, not_performed, /*...*/ nulltype } value;

  const char* to_string() const;
};
using integrity_protection_result_e = enumerated<integrity_protection_result_opts, true>;

// SecurityResult-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using security_result_ext_ies_o = protocol_ext_empty_o;

using drb_failed_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Failed-Item-NG-RAN ::= SEQUENCE
struct drb_failed_item_ng_ran_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  uint8_t                                  drb_id          = 1;
  cause_c                                  cause;
  drb_failed_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_failed_mod_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Failed-Mod-Item-NG-RAN ::= SEQUENCE
struct drb_failed_mod_item_ng_ran_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  uint8_t                                      drb_id          = 1;
  cause_c                                      cause;
  drb_failed_mod_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_failed_to_modify_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Failed-To-Modify-Item-NG-RAN ::= SEQUENCE
struct drb_failed_to_modify_item_ng_ran_s {
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  uint8_t                                            drb_id          = 1;
  cause_c                                            cause;
  drb_failed_to_modify_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct drb_modified_item_ng_ran_ext_ies_container {
  bool                          early_forwarding_count_info_present           = false;
  bool                          old_qos_flow_map_ul_endmarkerexpected_present = false;
  early_forwarding_count_info_c early_forwarding_count_info;
  qos_flow_list_l               old_qos_flow_map_ul_endmarkerexpected;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Modified-Item-NG-RAN ::= SEQUENCE
struct drb_modified_item_ng_ran_s {
  bool                                       ext                         = false;
  bool                                       pdcp_sn_status_info_present = false;
  bool                                       ie_exts_present             = false;
  uint8_t                                    drb_id                      = 1;
  up_params_l                                ul_up_transport_params;
  pdcp_sn_status_info_s                      pdcp_sn_status_info;
  qos_flow_list_l                            flow_setup_list;
  qos_flow_failed_list_l                     flow_failed_list;
  drb_modified_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_setup_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Setup-Item-NG-RAN ::= SEQUENCE
struct drb_setup_item_ng_ran_s {
  bool                                    ext                                   = false;
  bool                                    drb_data_forwarding_info_resp_present = false;
  bool                                    ie_exts_present                       = false;
  uint8_t                                 drb_id                                = 1;
  data_forwarding_info_s                  drb_data_forwarding_info_resp;
  up_params_l                             ul_up_transport_params;
  qos_flow_list_l                         flow_setup_list;
  qos_flow_failed_list_l                  flow_failed_list;
  drb_setup_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_setup_mod_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Setup-Mod-Item-NG-RAN ::= SEQUENCE
struct drb_setup_mod_item_ng_ran_s {
  bool                                        ext                                   = false;
  bool                                        drb_data_forwarding_info_resp_present = false;
  bool                                        ie_exts_present                       = false;
  uint8_t                                     drb_id                                = 1;
  data_forwarding_info_s                      drb_data_forwarding_info_resp;
  up_params_l                                 ul_up_transport_params;
  qos_flow_list_l                             flow_setup_list;
  qos_flow_failed_list_l                      flow_failed_list;
  drb_setup_mod_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoS-Flow-Removed-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using qos_flow_remd_item_ext_ies_o = protocol_ext_empty_o;

using security_result_ext_ies_container = protocol_ext_container_empty_l;

// SecurityResult ::= SEQUENCE
struct security_result_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  integrity_protection_result_e       integrity_protection_result;
  confidentiality_protection_result_e confidentiality_protection_result;
  security_result_ext_ies_container   ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Failed-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Failed-Item-NG-RAN
using drb_failed_list_ng_ran_l = dyn_array<drb_failed_item_ng_ran_s>;

// DRB-Failed-Mod-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_failed_mod_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-Failed-Mod-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Failed-Mod-Item-NG-RAN
using drb_failed_mod_list_ng_ran_l = dyn_array<drb_failed_mod_item_ng_ran_s>;

// DRB-Failed-To-Modify-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_failed_to_modify_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-Failed-To-Modify-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Failed-To-Modify-Item-NG-RAN
using drb_failed_to_modify_list_ng_ran_l = dyn_array<drb_failed_to_modify_item_ng_ran_s>;

// DRB-Modified-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_modified_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-Modified-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Modified-Item-NG-RAN
using drb_modified_list_ng_ran_l = dyn_array<drb_modified_item_ng_ran_s>;

// DRB-Removed-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_remd_item_ext_ies_o = protocol_ext_empty_o;

// DRB-Setup-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Setup-Item-NG-RAN
using drb_setup_list_ng_ran_l = dyn_array<drb_setup_item_ng_ran_s>;

// DRB-Setup-Mod-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct drb_setup_mod_item_eutran_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { security_result, data_forwarding_source_ip_address, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    security_result_s&                           security_result();
    bounded_bitstring<1, 160, true, true>&       data_forwarding_source_ip_address();
    const security_result_s&                     security_result() const;
    const bounded_bitstring<1, 160, true, true>& data_forwarding_source_ip_address() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-Setup-Mod-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Setup-Mod-Item-NG-RAN
using drb_setup_mod_list_ng_ran_l = dyn_array<drb_setup_mod_item_ng_ran_s>;

// PDU-Session-Resource-Failed-Mod-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdu_session_res_failed_mod_item_ext_ies_o = protocol_ext_empty_o;

// PDU-Session-Resource-Failed-To-Modify-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdu_session_res_failed_to_modify_item_ext_ies_o = protocol_ext_empty_o;

// PDU-Session-Resource-Modified-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdu_session_res_modified_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { redundant_n_g_dl_up_tnl_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::redundant_n_g_dl_up_tnl_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    up_tnl_info_c&       redundant_n_g_dl_up_tnl_info() { return c; }
    const up_tnl_info_c& redundant_n_g_dl_up_tnl_info() const { return c; }

  private:
    up_tnl_info_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDU-Session-Resource-Setup-Mod-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdu_session_res_setup_mod_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { redundant_n_g_dl_up_tnl_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::redundant_n_g_dl_up_tnl_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    up_tnl_info_c&       redundant_n_g_dl_up_tnl_info() { return c; }
    const up_tnl_info_c& redundant_n_g_dl_up_tnl_info() const { return c; }

  private:
    up_tnl_info_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using qos_flow_remd_item_ext_ies_container = protocol_ext_container_empty_l;

// QoS-Flow-Removed-Item ::= SEQUENCE
struct qos_flow_remd_item_s {
  struct qos_flow_released_in_session_opts {
    enum options { released_in_session, not_released_in_session, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using qos_flow_released_in_session_e_ = enumerated<qos_flow_released_in_session_opts, true>;

  // member variables
  bool                                 ext                                       = false;
  bool                                 qos_flow_released_in_session_present      = false;
  bool                                 qos_flow_accumulated_session_time_present = false;
  bool                                 ie_exts_present                           = false;
  uint8_t                              qos_flow_id                               = 0;
  qos_flow_released_in_session_e_      qos_flow_released_in_session;
  fixed_octstring<5, true>             qos_flow_accumulated_session_time;
  qos_flow_remd_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_failed_mod_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Failed-Mod-Item-EUTRAN ::= SEQUENCE
struct drb_failed_mod_item_eutran_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  uint8_t                                      drb_id          = 1;
  cause_c                                      cause;
  drb_failed_mod_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_failed_to_modify_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Failed-To-Modify-Item-EUTRAN ::= SEQUENCE
struct drb_failed_to_modify_item_eutran_s {
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  uint8_t                                            drb_id          = 1;
  cause_c                                            cause;
  drb_failed_to_modify_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_modified_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Modified-Item-EUTRAN ::= SEQUENCE
struct drb_modified_item_eutran_s {
  bool                                       ext                         = false;
  bool                                       s1_dl_up_tnl_info_present   = false;
  bool                                       pdcp_sn_status_info_present = false;
  bool                                       ie_exts_present             = false;
  uint8_t                                    drb_id                      = 1;
  up_tnl_info_c                              s1_dl_up_tnl_info;
  pdcp_sn_status_info_s                      pdcp_sn_status_info;
  up_params_l                                ul_up_transport_params;
  drb_modified_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_remd_item_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Removed-Item ::= SEQUENCE
struct drb_remd_item_s {
  struct drb_released_in_session_opts {
    enum options { released_in_session, not_released_in_session, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using drb_released_in_session_e_ = enumerated<drb_released_in_session_opts, true>;
  using qos_flow_remd_list_l_      = dyn_array<qos_flow_remd_item_s>;

  // member variables
  bool                            ext                                  = false;
  bool                            drb_released_in_session_present      = false;
  bool                            drb_accumulated_session_time_present = false;
  bool                            ie_exts_present                      = false;
  uint8_t                         drb_id                               = 1;
  drb_released_in_session_e_      drb_released_in_session;
  fixed_octstring<5, true>        drb_accumulated_session_time;
  qos_flow_remd_list_l_           qos_flow_remd_list;
  drb_remd_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct drb_setup_mod_item_eutran_ext_ies_container {
  bool                                  security_result_present                   = false;
  bool                                  data_forwarding_source_ip_address_present = false;
  security_result_s                     security_result;
  bounded_bitstring<1, 160, true, true> data_forwarding_source_ip_address;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Setup-Mod-Item-EUTRAN ::= SEQUENCE
struct drb_setup_mod_item_eutran_s {
  bool                                        ext                               = false;
  bool                                        data_forwarding_info_resp_present = false;
  bool                                        ie_exts_present                   = false;
  uint8_t                                     drb_id                            = 1;
  up_tnl_info_c                               s1_dl_up_tnl_info;
  data_forwarding_info_s                      data_forwarding_info_resp;
  up_params_l                                 ul_up_transport_params;
  drb_setup_mod_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_failed_mod_item_ext_ies_container = protocol_ext_container_empty_l;

// PDU-Session-Resource-Failed-Mod-Item ::= SEQUENCE
struct pdu_session_res_failed_mod_item_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  uint16_t                                          pdu_session_id  = 0;
  cause_c                                           cause;
  pdu_session_res_failed_mod_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_failed_to_modify_item_ext_ies_container = protocol_ext_container_empty_l;

// PDU-Session-Resource-Failed-To-Modify-Item ::= SEQUENCE
struct pdu_session_res_failed_to_modify_item_s {
  bool                                                    ext             = false;
  bool                                                    ie_exts_present = false;
  uint16_t                                                pdu_session_id  = 0;
  cause_c                                                 cause;
  pdu_session_res_failed_to_modify_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-Modified-Item ::= SEQUENCE
struct pdu_session_res_modified_item_s {
  bool                               ext                                           = false;
  bool                               ng_dl_up_tnl_info_present                     = false;
  bool                               security_result_present                       = false;
  bool                               pdu_session_data_forwarding_info_resp_present = false;
  uint16_t                           pdu_session_id                                = 0;
  up_tnl_info_c                      ng_dl_up_tnl_info;
  security_result_s                  security_result;
  data_forwarding_info_s             pdu_session_data_forwarding_info_resp;
  drb_setup_list_ng_ran_l            drb_setup_list_ng_ran;
  drb_failed_list_ng_ran_l           drb_failed_list_ng_ran;
  drb_modified_list_ng_ran_l         drb_modified_list_ng_ran;
  drb_failed_to_modify_list_ng_ran_l drb_failed_to_modify_list_ng_ran;
  protocol_ext_container_l<pdu_session_res_modified_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-Setup-Mod-Item ::= SEQUENCE
struct pdu_session_res_setup_mod_item_s {
  bool                         ext                                           = false;
  bool                         security_result_present                       = false;
  bool                         pdu_session_data_forwarding_info_resp_present = false;
  uint16_t                     pdu_session_id                                = 0;
  security_result_s            security_result;
  up_tnl_info_c                ng_dl_up_tnl_info;
  data_forwarding_info_s       pdu_session_data_forwarding_info_resp;
  drb_setup_mod_list_ng_ran_l  drb_setup_mod_list_ng_ran;
  drb_failed_mod_list_ng_ran_l drb_failed_mod_list_ng_ran;
  protocol_ext_container_l<pdu_session_res_setup_mod_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Failed-Mod-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Failed-Mod-Item-EUTRAN
using drb_failed_mod_list_eutran_l = dyn_array<drb_failed_mod_item_eutran_s>;

// DRB-Failed-To-Modify-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Failed-To-Modify-Item-EUTRAN
using drb_failed_to_modify_list_eutran_l = dyn_array<drb_failed_to_modify_item_eutran_s>;

// DRB-Modified-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Modified-Item-EUTRAN
using drb_modified_list_eutran_l = dyn_array<drb_modified_item_eutran_s>;

// DRB-Setup-Mod-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Setup-Mod-Item-EUTRAN
using drb_setup_mod_list_eutran_l = dyn_array<drb_setup_mod_item_eutran_s>;

// PDU-Session-Resource-Failed-Mod-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Failed-Mod-Item
using pdu_session_res_failed_mod_list_l = dyn_array<pdu_session_res_failed_mod_item_s>;

// PDU-Session-Resource-Failed-To-Modify-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Failed-To-Modify-Item
using pdu_session_res_failed_to_modify_list_l = dyn_array<pdu_session_res_failed_to_modify_item_s>;

// PDU-Session-Resource-Modified-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Modified-Item
using pdu_session_res_modified_list_l = dyn_array<pdu_session_res_modified_item_s>;

// PDU-Session-Resource-Setup-Mod-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Setup-Mod-Item
using pdu_session_res_setup_mod_list_l = dyn_array<pdu_session_res_setup_mod_item_s>;

// RetainabilityMeasurementsInfo ::= SEQUENCE (SIZE (1..32)) OF DRB-Removed-Item
using retainability_meass_info_l = dyn_array<drb_remd_item_s>;

// DRB-Status-ItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_status_item_ext_ies_o = protocol_ext_empty_o;

using drb_status_item_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Status-Item ::= SEQUENCE
struct drb_status_item_s {
  bool                              ext                   = false;
  bool                              pdcp_dl_count_present = false;
  bool                              pdcp_ul_count_present = false;
  bool                              ie_exts_present       = false;
  uint8_t                           drb_id                = 1;
  pdcp_count_s                      pdcp_dl_count;
  pdcp_count_s                      pdcp_ul_count;
  drb_status_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// M4ReportAmount ::= ENUMERATED
struct m4_report_amount_opts {
  enum options { r1, r2, r4, r8, r16, r32, r64, infinity, /*...*/ nulltype } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
using m4_report_amount_e = enumerated<m4_report_amount_opts, true>;

// M6ReportAmount ::= ENUMERATED
struct m6_report_amount_opts {
  enum options { r1, r2, r4, r8, r16, r32, r64, infinity, /*...*/ nulltype } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
using m6_report_amount_e = enumerated<m6_report_amount_opts, true>;

// M7ReportAmount ::= ENUMERATED
struct m7_report_amount_opts {
  enum options { r1, r2, r4, r8, r16, r32, r64, infinity, /*...*/ nulltype } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
using m7_report_amount_e = enumerated<m7_report_amount_opts, true>;

// Links-to-log ::= ENUMERATED
struct links_to_log_opts {
  enum options { ul, dl, both_ul_and_dl, /*...*/ nulltype } value;

  const char* to_string() const;
};
using links_to_log_e = enumerated<links_to_log_opts, true>;

// M4Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct m4_cfg_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { m4_report_amount, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::m4_report_amount; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    m4_report_amount_e&       m4_report_amount() { return c; }
    const m4_report_amount_e& m4_report_amount() const { return c; }

  private:
    m4_report_amount_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// M4period ::= ENUMERATED
struct m4period_opts {
  enum options { ms1024, ms2048, ms5120, ms10240, min1, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using m4period_e = enumerated<m4period_opts, true>;

// M6Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct m6_cfg_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { m6_report_amount, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::m6_report_amount; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    m6_report_amount_e&       m6_report_amount() { return c; }
    const m6_report_amount_e& m6_report_amount() const { return c; }

  private:
    m6_report_amount_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// M6report-Interval ::= ENUMERATED
struct m6report_interv_opts {
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
    // ...
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using m6report_interv_e = enumerated<m6report_interv_opts, true>;

// M7Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct m7_cfg_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { m7_report_amount, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::m7_report_amount; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    m7_report_amount_e&       m7_report_amount() { return c; }
    const m7_report_amount_e& m7_report_amount() const { return c; }

  private:
    m7_report_amount_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RSN ::= ENUMERATED
struct r_sn_opts {
  enum options { v1, v2, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using r_sn_e = enumerated<r_sn_opts, true>;

// RedundantPDUSessionInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct redundant_pdu_session_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { pdu_session_pair_id, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::pdu_session_pair_id; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&       pdu_session_pair_id() { return c; }
    const uint16_t& pdu_session_pair_id() const { return c; }

  private:
    uint16_t c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ImmediateMDT-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using immediate_mdt_ext_ies_o = protocol_ext_empty_o;

// M4Configuration ::= SEQUENCE
struct m4_cfg_s {
  bool                                       ext = false;
  m4period_e                                 m4period;
  links_to_log_e                             m4_links_to_log;
  protocol_ext_container_l<m4_cfg_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// M6Configuration ::= SEQUENCE
struct m6_cfg_s {
  bool                                       ext = false;
  m6report_interv_e                          m6report_interv;
  links_to_log_e                             m6_links_to_log;
  protocol_ext_container_l<m6_cfg_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// M7Configuration ::= SEQUENCE
struct m7_cfg_s {
  bool                                       ext      = false;
  uint8_t                                    m7period = 1;
  links_to_log_e                             m7_links_to_log;
  protocol_ext_container_l<m7_cfg_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RedundantPDUSessionInformation ::= SEQUENCE
struct redundant_pdu_session_info_s {
  bool                                                           ext = false;
  r_sn_e                                                         rsn;
  protocol_ext_container_l<redundant_pdu_session_info_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-To-Setup-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct drb_to_setup_item_eutran_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { data_forwarding_source_ip_address, security_ind, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bounded_bitstring<1, 160, true, true>&       data_forwarding_source_ip_address();
    security_ind_s&                              security_ind();
    const bounded_bitstring<1, 160, true, true>& data_forwarding_source_ip_address() const;
    const security_ind_s&                        security_ind() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using immediate_mdt_ext_ies_container = protocol_ext_container_empty_l;

// ImmediateMDT ::= SEQUENCE
struct immediate_mdt_s {
  bool                            ext                = false;
  bool                            meas_four_present  = false;
  bool                            meas_six_present   = false;
  bool                            meas_seven_present = false;
  bool                            ie_exts_present    = false;
  fixed_bitstring<8, false, true> meass_to_activ;
  m4_cfg_s                        meas_four;
  m6_cfg_s                        meas_six;
  m7_cfg_s                        meas_seven;
  immediate_mdt_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MDTMode-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using mdt_mode_ext_ies_o = protocol_ies_empty_o;

// PDU-Session-Resource-To-Setup-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdu_session_res_to_setup_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        common_network_instance,
        redundant_n_g_ul_up_tnl_info,
        redundant_common_network_instance,
        redundant_pdu_session_info,
        special_trigger_purpose,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    unbounded_octstring<true>&          common_network_instance();
    up_tnl_info_c&                      redundant_n_g_ul_up_tnl_info();
    unbounded_octstring<true>&          redundant_common_network_instance();
    redundant_pdu_session_info_s&       redundant_pdu_session_info();
    special_trigger_purpose_e&          special_trigger_purpose();
    const unbounded_octstring<true>&    common_network_instance() const;
    const up_tnl_info_c&                redundant_n_g_ul_up_tnl_info() const;
    const unbounded_octstring<true>&    redundant_common_network_instance() const;
    const redundant_pdu_session_info_s& redundant_pdu_session_info() const;
    const special_trigger_purpose_e&    special_trigger_purpose() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct drb_to_setup_item_eutran_ext_ies_container {
  bool                                  data_forwarding_source_ip_address_present = false;
  bool                                  security_ind_present                      = false;
  bounded_bitstring<1, 160, true, true> data_forwarding_source_ip_address;
  security_ind_s                        security_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-To-Setup-Item-EUTRAN ::= SEQUENCE
struct drb_to_setup_item_eutran_s {
  bool                                       ext                                          = false;
  bool                                       data_forwarding_info_request_present         = false;
  bool                                       drb_inactivity_timer_present                 = false;
  bool                                       existing_allocated_s1_dl_up_tnl_info_present = false;
  bool                                       ie_exts_present                              = false;
  uint8_t                                    drb_id                                       = 1;
  pdcp_cfg_s                                 pdcp_cfg;
  eutran_qos_s                               eutran_qos;
  up_tnl_info_c                              s1_ul_up_tnl_info;
  data_forwarding_info_request_s             data_forwarding_info_request;
  cell_group_info_l                          cell_group_info;
  up_params_l                                dl_up_params;
  uint16_t                                   drb_inactivity_timer = 1;
  up_tnl_info_c                              existing_allocated_s1_dl_up_tnl_info;
  drb_to_setup_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MDT-Activation ::= ENUMERATED
struct mdt_activation_opts {
  enum options { immediate_mdt_only, immediate_mdt_and_trace, /*...*/ nulltype } value;

  const char* to_string() const;
};
using mdt_activation_e = enumerated<mdt_activation_opts, true>;

// MDT-Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mdt_cfg_ext_ies_o = protocol_ext_empty_o;

// MDTMode ::= CHOICE
struct mdt_mode_c {
  struct types_opts {
    enum options { immediate_mdt, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  mdt_mode_c() = default;
  mdt_mode_c(const mdt_mode_c& other);
  mdt_mode_c& operator=(const mdt_mode_c& other);
  ~mdt_mode_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  immediate_mdt_s& immediate_mdt()
  {
    assert_choice_type(types::immediate_mdt, type_, "MDTMode");
    return c.get<immediate_mdt_s>();
  }
  protocol_ie_single_container_s<mdt_mode_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "MDTMode");
    return c.get<protocol_ie_single_container_s<mdt_mode_ext_ies_o>>();
  }
  const immediate_mdt_s& immediate_mdt() const
  {
    assert_choice_type(types::immediate_mdt, type_, "MDTMode");
    return c.get<immediate_mdt_s>();
  }
  const protocol_ie_single_container_s<mdt_mode_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "MDTMode");
    return c.get<protocol_ie_single_container_s<mdt_mode_ext_ies_o>>();
  }
  immediate_mdt_s&                                    set_immediate_mdt();
  protocol_ie_single_container_s<mdt_mode_ext_ies_o>& set_choice_ext();

private:
  types                                                                                type_;
  choice_buffer_t<immediate_mdt_s, protocol_ie_single_container_s<mdt_mode_ext_ies_o>> c;

  void destroy_();
};

struct pdu_session_res_to_setup_item_ext_ies_container {
  bool                         common_network_instance_present           = false;
  bool                         redundant_n_g_ul_up_tnl_info_present      = false;
  bool                         redundant_common_network_instance_present = false;
  bool                         redundant_pdu_session_info_present        = false;
  bool                         special_trigger_purpose_present           = false;
  unbounded_octstring<true>    common_network_instance;
  up_tnl_info_c                redundant_n_g_ul_up_tnl_info;
  unbounded_octstring<true>    redundant_common_network_instance;
  redundant_pdu_session_info_s redundant_pdu_session_info;
  special_trigger_purpose_e    special_trigger_purpose;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-To-Setup-Item ::= SEQUENCE
struct pdu_session_res_to_setup_item_s {
  bool                                            ext                                              = false;
  bool                                            pdu_session_res_dl_ambr_present                  = false;
  bool                                            pdu_session_data_forwarding_info_request_present = false;
  bool                                            pdu_session_inactivity_timer_present             = false;
  bool                                            existing_allocated_ng_dl_up_tnl_info_present     = false;
  bool                                            network_instance_present                         = false;
  bool                                            ie_exts_present                                  = false;
  uint16_t                                        pdu_session_id                                   = 0;
  pdu_session_type_e                              pdu_session_type;
  snssai_s                                        snssai;
  security_ind_s                                  security_ind;
  uint64_t                                        pdu_session_res_dl_ambr = 0;
  up_tnl_info_c                                   ng_ul_up_tnl_info;
  data_forwarding_info_request_s                  pdu_session_data_forwarding_info_request;
  uint16_t                                        pdu_session_inactivity_timer = 1;
  up_tnl_info_c                                   existing_allocated_ng_dl_up_tnl_info;
  uint16_t                                        network_instance = 1;
  drb_to_setup_list_ng_ran_l                      drb_to_setup_list_ng_ran;
  pdu_session_res_to_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-To-Setup-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-To-Setup-Item-EUTRAN
using drb_to_setup_list_eutran_l = dyn_array<drb_to_setup_item_eutran_s>;

using mdt_cfg_ext_ies_container = protocol_ext_container_empty_l;

// MDT-Configuration ::= SEQUENCE
struct mdt_cfg_s {
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  mdt_activation_e          mdt_activation;
  mdt_mode_c                mdt_mode;
  mdt_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPNContextInfo-SNPN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using npn_context_info_sn_pn_ext_ies_o = protocol_ext_empty_o;

// PDU-Session-Resource-To-Setup-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-To-Setup-Item
using pdu_session_res_to_setup_list_l = dyn_array<pdu_session_res_to_setup_item_s>;

// NPNContextInfo-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using npn_context_info_ext_ies_o = protocol_ies_empty_o;

using npn_context_info_sn_pn_ext_ies_container = protocol_ext_container_empty_l;

// NPNContextInfo-SNPN ::= SEQUENCE
struct npn_context_info_sn_pn_s {
  bool                                     ie_exts_present = false;
  fixed_bitstring<44, false, true>         nid;
  npn_context_info_sn_pn_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TraceActivation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct trace_activation_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { mdt_cfg, trace_collection_entity_uri, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mdt_cfg_s&                                                   mdt_cfg();
    visible_string<0, MAX_ASN_STRING_LENGTH, false, true>&       trace_collection_entity_uri();
    const mdt_cfg_s&                                             mdt_cfg() const;
    const visible_string<0, MAX_ASN_STRING_LENGTH, false, true>& trace_collection_entity_uri() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TraceDepth ::= ENUMERATED
struct trace_depth_opts {
  enum options {
    minimum,
    medium,
    max,
    minimum_without_vendor_specific_ext,
    medium_without_vendor_specific_ext,
    max_without_vendor_specific_ext,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
using trace_depth_e = enumerated<trace_depth_opts, true>;

// AdditionalHandoverInfo ::= ENUMERATED
struct add_ho_info_opts {
  enum options { discard_pdpc_sn, /*...*/ nulltype } value;

  const char* to_string() const;
};
using add_ho_info_e = enumerated<add_ho_info_opts, true>;

// CHOInitiation ::= ENUMERATED
struct cho_initiation_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using cho_initiation_e = enumerated<cho_initiation_opts, true>;

// DirectForwardingPathAvailability ::= ENUMERATED
struct direct_forwarding_path_availability_opts {
  enum options { inter_sys_direct_path_available, /*...*/ intra_sys_direct_path_available, nulltype } value;

  const char* to_string() const;
};
using direct_forwarding_path_availability_e = enumerated<direct_forwarding_path_availability_opts, true, 1>;

// MDTPLMNList ::= SEQUENCE (SIZE (1..16)) OF OCTET STRING (SIZE (3))
using mdt_plmn_list_l = bounded_array<fixed_octstring<3, true>, 16>;

// NPNContextInfo ::= CHOICE
struct npn_context_info_c {
  struct types_opts {
    enum options { sn_pn, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  npn_context_info_c() = default;
  npn_context_info_c(const npn_context_info_c& other);
  npn_context_info_c& operator=(const npn_context_info_c& other);
  ~npn_context_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  npn_context_info_sn_pn_s& sn_pn()
  {
    assert_choice_type(types::sn_pn, type_, "NPNContextInfo");
    return c.get<npn_context_info_sn_pn_s>();
  }
  protocol_ie_single_container_s<npn_context_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "NPNContextInfo");
    return c.get<protocol_ie_single_container_s<npn_context_info_ext_ies_o>>();
  }
  const npn_context_info_sn_pn_s& sn_pn() const
  {
    assert_choice_type(types::sn_pn, type_, "NPNContextInfo");
    return c.get<npn_context_info_sn_pn_s>();
  }
  const protocol_ie_single_container_s<npn_context_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "NPNContextInfo");
    return c.get<protocol_ie_single_container_s<npn_context_info_ext_ies_o>>();
  }
  npn_context_info_sn_pn_s&                                   set_sn_pn();
  protocol_ie_single_container_s<npn_context_info_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                 type_;
  choice_buffer_t<npn_context_info_sn_pn_s, protocol_ie_single_container_s<npn_context_info_ext_ies_o>> c;

  void destroy_();
};

struct trace_activation_ext_ies_container {
  bool                                                  mdt_cfg_present                     = false;
  bool                                                  trace_collection_entity_uri_present = false;
  mdt_cfg_s                                             mdt_cfg;
  visible_string<0, MAX_ASN_STRING_LENGTH, false, true> trace_collection_entity_uri;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TraceActivation ::= SEQUENCE
struct trace_activation_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  fixed_octstring<8, true>              trace_id;
  fixed_bitstring<8, false, true>       interfaces_to_trace;
  trace_depth_e                         trace_depth;
  bounded_bitstring<1, 160, true, true> trace_collection_entity_ip_address;
  trace_activation_ext_ies_container    ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Failed-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_failed_item_eutran_ext_ies_o = protocol_ext_empty_o;

// DRB-Setup-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct drb_setup_item_eutran_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { data_forwarding_source_ip_address, security_result, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bounded_bitstring<1, 160, true, true>&       data_forwarding_source_ip_address();
    security_result_s&                           security_result();
    const bounded_bitstring<1, 160, true, true>& data_forwarding_source_ip_address() const;
    const security_result_s&                     security_result() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDU-Session-Resource-Failed-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdu_session_res_failed_item_ext_ies_o = protocol_ext_empty_o;

// PDU-Session-Resource-Setup-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct pdu_session_res_setup_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { redundant_n_g_dl_up_tnl_info, redundant_pdu_session_info_used, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    up_tnl_info_c&                      redundant_n_g_dl_up_tnl_info();
    redundant_pdu_session_info_s&       redundant_pdu_session_info_used();
    const up_tnl_info_c&                redundant_n_g_dl_up_tnl_info() const;
    const redundant_pdu_session_info_s& redundant_pdu_session_info_used() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using drb_failed_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Failed-Item-EUTRAN ::= SEQUENCE
struct drb_failed_item_eutran_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  uint8_t                                  drb_id          = 1;
  cause_c                                  cause;
  drb_failed_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct drb_setup_item_eutran_ext_ies_container {
  bool                                  data_forwarding_source_ip_address_present = false;
  bool                                  security_result_present                   = false;
  bounded_bitstring<1, 160, true, true> data_forwarding_source_ip_address;
  security_result_s                     security_result;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Setup-Item-EUTRAN ::= SEQUENCE
struct drb_setup_item_eutran_s {
  struct s1_dl_up_unchanged_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using s1_dl_up_unchanged_e_ = enumerated<s1_dl_up_unchanged_opts, true>;

  // member variables
  bool                                    ext                               = false;
  bool                                    data_forwarding_info_resp_present = false;
  bool                                    s1_dl_up_unchanged_present        = false;
  bool                                    ie_exts_present                   = false;
  uint8_t                                 drb_id                            = 1;
  up_tnl_info_c                           s1_dl_up_tnl_info;
  data_forwarding_info_s                  data_forwarding_info_resp;
  up_params_l                             ul_up_transport_params;
  s1_dl_up_unchanged_e_                   s1_dl_up_unchanged;
  drb_setup_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_failed_item_ext_ies_container = protocol_ext_container_empty_l;

// PDU-Session-Resource-Failed-Item ::= SEQUENCE
struct pdu_session_res_failed_item_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  uint16_t                                      pdu_session_id  = 0;
  cause_c                                       cause;
  pdu_session_res_failed_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct pdu_session_res_setup_item_ext_ies_container {
  bool                         redundant_n_g_dl_up_tnl_info_present    = false;
  bool                         redundant_pdu_session_info_used_present = false;
  up_tnl_info_c                redundant_n_g_dl_up_tnl_info;
  redundant_pdu_session_info_s redundant_pdu_session_info_used;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-Setup-Item ::= SEQUENCE
struct pdu_session_res_setup_item_s {
  struct ng_dl_up_unchanged_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ng_dl_up_unchanged_e_ = enumerated<ng_dl_up_unchanged_opts, true>;

  // member variables
  bool                                         ext                                           = false;
  bool                                         security_result_present                       = false;
  bool                                         pdu_session_data_forwarding_info_resp_present = false;
  bool                                         ng_dl_up_unchanged_present                    = false;
  bool                                         ie_exts_present                               = false;
  uint16_t                                     pdu_session_id                                = 0;
  security_result_s                            security_result;
  up_tnl_info_c                                ng_dl_up_tnl_info;
  data_forwarding_info_s                       pdu_session_data_forwarding_info_resp;
  ng_dl_up_unchanged_e_                        ng_dl_up_unchanged;
  drb_setup_list_ng_ran_l                      drb_setup_list_ng_ran;
  drb_failed_list_ng_ran_l                     drb_failed_list_ng_ran;
  pdu_session_res_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Failed-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Failed-Item-EUTRAN
using drb_failed_list_eutran_l = dyn_array<drb_failed_item_eutran_s>;

// DRB-Setup-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRB-Setup-Item-EUTRAN
using drb_setup_list_eutran_l = dyn_array<drb_setup_item_eutran_s>;

// PDU-Session-Resource-Failed-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Failed-Item
using pdu_session_res_failed_list_l = dyn_array<pdu_session_res_failed_item_s>;

// PDU-Session-Resource-Setup-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Setup-Item
using pdu_session_res_setup_list_l = dyn_array<pdu_session_res_setup_item_s>;

// Endpoint-IP-address-and-port-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using endpoint_ip_address_and_port_ext_ies_o = protocol_ext_empty_o;

using endpoint_ip_address_and_port_ext_ies_container = protocol_ext_container_empty_l;

// Endpoint-IP-address-and-port ::= SEQUENCE
struct endpoint_ip_address_and_port_s {
  bool                                           ie_exts_present = false;
  bounded_bitstring<1, 160, true, true>          endpoint_ip_address;
  fixed_bitstring<16, false, true>               port_num;
  endpoint_ip_address_and_port_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CP-TNL-Information-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
struct cp_tnl_info_ext_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { endpoint_ip_address_and_port, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::endpoint_ip_address_and_port; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    endpoint_ip_address_and_port_s&       endpoint_ip_address_and_port() { return c; }
    const endpoint_ip_address_and_port_s& endpoint_ip_address_and_port() const { return c; }

  private:
    endpoint_ip_address_and_port_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// CP-TNL-Information ::= CHOICE
struct cp_tnl_info_c {
  struct types_opts {
    enum options { endpoint_ip_address, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  cp_tnl_info_c() = default;
  cp_tnl_info_c(const cp_tnl_info_c& other);
  cp_tnl_info_c& operator=(const cp_tnl_info_c& other);
  ~cp_tnl_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  bounded_bitstring<1, 160, true, true>& endpoint_ip_address()
  {
    assert_choice_type(types::endpoint_ip_address, type_, "CP-TNL-Information");
    return c.get<bounded_bitstring<1, 160, true, true>>();
  }
  protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "CP-TNL-Information");
    return c.get<protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>>();
  }
  const bounded_bitstring<1, 160, true, true>& endpoint_ip_address() const
  {
    assert_choice_type(types::endpoint_ip_address, type_, "CP-TNL-Information");
    return c.get<bounded_bitstring<1, 160, true, true>>();
  }
  const protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "CP-TNL-Information");
    return c.get<protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>>();
  }
  bounded_bitstring<1, 160, true, true>&                 set_endpoint_ip_address();
  protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                         type_;
  choice_buffer_t<bounded_bitstring<1, 160, true, true>, protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>> c;

  void destroy_();
};

// PrivacyIndicator ::= ENUMERATED
struct privacy_ind_opts {
  enum options { immediate_mdt, logged_mdt, /*...*/ nulltype } value;

  const char* to_string() const;
};
using privacy_ind_e = enumerated<privacy_ind_opts, true>;

// PDU-Session-To-Notify-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdu_session_to_notify_item_ext_ies_o = protocol_ext_empty_o;

// MT-SDT-Information-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mt_sdt_info_ext_ies_o = protocol_ext_empty_o;

using pdu_session_to_notify_item_ext_ies_container = protocol_ext_container_empty_l;

// PDU-Session-To-Notify-Item ::= SEQUENCE
struct pdu_session_to_notify_item_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  uint16_t                                     pdu_session_id  = 0;
  qos_flow_list_l                              qos_flow_list;
  pdu_session_to_notify_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mt_sdt_info_ext_ies_container = protocol_ext_container_empty_l;

// MT-SDT-Information ::= SEQUENCE
struct mt_sdt_info_s {
  bool                          ie_exts_present  = false;
  uint32_t                      mt_sdt_data_size = 1;
  mt_sdt_info_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-To-Notify-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-To-Notify-Item
using pdu_session_to_notify_list_l = dyn_array<pdu_session_to_notify_item_s>;

// SDT-data-size-threshold-Crossed ::= ENUMERATED
struct sdt_data_size_thres_crossed_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using sdt_data_size_thres_crossed_e = enumerated<sdt_data_size_thres_crossed_opts, true>;

// DLUPTNLAddressToUpdateItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using dl_up_tnl_address_to_upd_item_ext_ies_o = protocol_ext_empty_o;

using dl_up_tnl_address_to_upd_item_ext_ies_container = protocol_ext_container_empty_l;

// DLUPTNLAddressToUpdateItem ::= SEQUENCE
struct dl_up_tnl_address_to_upd_item_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  bounded_bitstring<1, 160, true, true>           old_tnl_adress;
  bounded_bitstring<1, 160, true, true>           new_tnl_adress;
  dl_up_tnl_address_to_upd_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Measurement-Results-Information-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_meas_results_info_item_ext_ies_o = protocol_ext_empty_o;

using drb_meas_results_info_item_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Measurement-Results-Information-Item ::= SEQUENCE
struct drb_meas_results_info_item_s {
  bool                                         ext                  = false;
  bool                                         ul_d1_result_present = false;
  bool                                         ie_exts_present      = false;
  uint8_t                                      drb_id               = 1;
  uint16_t                                     ul_d1_result         = 0;
  drb_meas_results_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Measurement-Results-Information-List ::= SEQUENCE (SIZE (1..32)) OF DRB-Measurement-Results-Information-Item
using drb_meas_results_info_list_l = dyn_array<drb_meas_results_info_item_s>;

// DRB-Usage-Report-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drb_usage_report_item_ext_ies_o = protocol_ext_empty_o;

using drb_usage_report_item_ext_ies_container = protocol_ext_container_empty_l;

// DRB-Usage-Report-Item ::= SEQUENCE
struct drb_usage_report_item_s {
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  fixed_octstring<4, true>                start_time_stamp;
  fixed_octstring<4, true>                end_time_stamp;
  uint64_t                                usage_count_ul = 0;
  uint64_t                                usage_count_dl = 0;
  drb_usage_report_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-Usage-Report-List ::= SEQUENCE (SIZE (1..2)) OF DRB-Usage-Report-Item
using drb_usage_report_list_l = dyn_array<drb_usage_report_item_s>;

// DRBs-Subject-To-Counter-Check-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drbs_subject_to_counter_check_item_eutran_ext_ies_o = protocol_ext_empty_o;

using drbs_subject_to_counter_check_item_eutran_ext_ies_container = protocol_ext_container_empty_l;

// DRBs-Subject-To-Counter-Check-Item-EUTRAN ::= SEQUENCE
struct drbs_subject_to_counter_check_item_eutran_s {
  bool                                                        ext             = false;
  bool                                                        ie_exts_present = false;
  uint8_t                                                     drb_id          = 1;
  pdcp_count_s                                                pdcp_ul_count;
  pdcp_count_s                                                pdcp_dl_count;
  drbs_subject_to_counter_check_item_eutran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-Subject-To-Counter-Check-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drbs_subject_to_counter_check_item_ng_ran_ext_ies_o = protocol_ext_empty_o;

using drbs_subject_to_counter_check_item_ng_ran_ext_ies_container = protocol_ext_container_empty_l;

// DRBs-Subject-To-Counter-Check-Item-NG-RAN ::= SEQUENCE
struct drbs_subject_to_counter_check_item_ng_ran_s {
  bool                                                        ext             = false;
  bool                                                        ie_exts_present = false;
  uint16_t                                                    pdu_session_id  = 0;
  uint8_t                                                     drb_id          = 1;
  pdcp_count_s                                                pdcp_ul_count;
  pdcp_count_s                                                pdcp_dl_count;
  drbs_subject_to_counter_check_item_ng_ran_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-Subject-To-Counter-Check-List-EUTRAN ::= SEQUENCE (SIZE (1..32)) OF DRBs-Subject-To-Counter-Check-Item-EUTRAN
using drbs_subject_to_counter_check_list_eutran_l = dyn_array<drbs_subject_to_counter_check_item_eutran_s>;

// DRBs-Subject-To-Counter-Check-List-NG-RAN ::= SEQUENCE (SIZE (1..32)) OF DRBs-Subject-To-Counter-Check-Item-NG-RAN
using drbs_subject_to_counter_check_list_ng_ran_l = dyn_array<drbs_subject_to_counter_check_item_ng_ran_s>;

// DRBs-Subject-To-Early-Forwarding-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using drbs_subject_to_early_forwarding_item_ext_ies_o = protocol_ext_empty_o;

using drbs_subject_to_early_forwarding_item_ext_ies_container = protocol_ext_container_empty_l;

// DRBs-Subject-To-Early-Forwarding-Item ::= SEQUENCE
struct drbs_subject_to_early_forwarding_item_s {
  bool                                                    ext             = false;
  bool                                                    ie_exts_present = false;
  uint8_t                                                 drb_id          = 1;
  pdcp_count_s                                            dl_count_value;
  drbs_subject_to_early_forwarding_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBs-Subject-To-Early-Forwarding-List ::= SEQUENCE (SIZE (1..32)) OF DRBs-Subject-To-Early-Forwarding-Item
using drbs_subject_to_early_forwarding_list_l = dyn_array<drbs_subject_to_early_forwarding_item_s>;

// Data-Usage-Report-ItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using data_usage_report_item_ext_ies_o = protocol_ext_empty_o;

using data_usage_report_item_ext_ies_container = protocol_ext_container_empty_l;

// Data-Usage-Report-Item ::= SEQUENCE
struct data_usage_report_item_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  uint8_t                                  drb_id          = 1;
  rat_type_e                               rat_type;
  drb_usage_report_list_l                  drb_usage_report_list;
  data_usage_report_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Data-Usage-Report-List ::= SEQUENCE (SIZE (1..32)) OF Data-Usage-Report-Item
using data_usage_report_list_l = dyn_array<data_usage_report_item_s>;

// MRDC-Data-Usage-Report-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mrdc_data_usage_report_item_ext_ies_o = protocol_ext_empty_o;

// Data-Usage-per-PDU-Session-Report-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using data_usage_per_pdu_session_report_ext_ies_o = protocol_ext_empty_o;

using mrdc_data_usage_report_item_ext_ies_container = protocol_ext_container_empty_l;

// MRDC-Data-Usage-Report-Item ::= SEQUENCE
struct mrdc_data_usage_report_item_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  fixed_octstring<4, true>                      start_time_stamp;
  fixed_octstring<4, true>                      end_time_stamp;
  uint64_t                                      usage_count_ul = 0;
  uint64_t                                      usage_count_dl = 0;
  mrdc_data_usage_report_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using data_usage_per_pdu_session_report_ext_ies_container = protocol_ext_container_empty_l;

// Data-Usage-per-PDU-Session-Report ::= SEQUENCE
struct data_usage_per_pdu_session_report_s {
  struct secondary_rat_type_opts {
    enum options { nr, e_utra, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using secondary_rat_type_e_            = enumerated<secondary_rat_type_opts, true>;
  using pdu_session_timed_report_list_l_ = dyn_array<mrdc_data_usage_report_item_s>;

  // member variables
  bool                                                ext             = false;
  bool                                                ie_exts_present = false;
  secondary_rat_type_e_                               secondary_rat_type;
  pdu_session_timed_report_list_l_                    pdu_session_timed_report_list;
  data_usage_per_pdu_session_report_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Data-Usage-per-QoS-Flow-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using data_usage_per_qos_flow_item_ext_ies_o = protocol_ext_empty_o;

using data_usage_per_qos_flow_item_ext_ies_container = protocol_ext_container_empty_l;

// Data-Usage-per-QoS-Flow-Item ::= SEQUENCE
struct data_usage_per_qos_flow_item_s {
  struct secondary_rat_type_opts {
    enum options { nr, e_utra, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using secondary_rat_type_e_         = enumerated<secondary_rat_type_opts, true>;
  using qos_flow_timed_report_list_l_ = dyn_array<mrdc_data_usage_report_item_s>;

  // member variables
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  uint8_t                                        qos_flow_id     = 0;
  secondary_rat_type_e_                          secondary_rat_type;
  qos_flow_timed_report_list_l_                  qos_flow_timed_report_list;
  data_usage_per_qos_flow_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Data-Usage-per-QoS-Flow-List ::= SEQUENCE (SIZE (1..64)) OF Data-Usage-per-QoS-Flow-Item
using data_usage_per_qos_flow_list_l = dyn_array<data_usage_per_qos_flow_item_s>;

// MRB-ProgressInformationSNs-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using mrb_progress_info_sn_s_ext_ies_o = protocol_ies_empty_o;

// ECGI-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ecgi_ext_ies_o = protocol_ext_empty_o;

// MBSSessionAssociatedInformation-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mbs_session_associated_info_item_ext_ies_o = protocol_ext_empty_o;

// MRB-ProgressInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mrb_progress_info_ext_ies_o = protocol_ext_empty_o;

// MRB-ProgressInformationSNs ::= CHOICE
struct mrb_progress_info_sn_s_c {
  struct types_opts {
    enum options { pdcp_sn12, pdcp_sn18, choice_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  mrb_progress_info_sn_s_c() = default;
  mrb_progress_info_sn_s_c(const mrb_progress_info_sn_s_c& other);
  mrb_progress_info_sn_s_c& operator=(const mrb_progress_info_sn_s_c& other);
  ~mrb_progress_info_sn_s_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& pdcp_sn12()
  {
    assert_choice_type(types::pdcp_sn12, type_, "MRB-ProgressInformationSNs");
    return c.get<uint16_t>();
  }
  uint32_t& pdcp_sn18()
  {
    assert_choice_type(types::pdcp_sn18, type_, "MRB-ProgressInformationSNs");
    return c.get<uint32_t>();
  }
  protocol_ie_single_container_s<mrb_progress_info_sn_s_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "MRB-ProgressInformationSNs");
    return c.get<protocol_ie_single_container_s<mrb_progress_info_sn_s_ext_ies_o>>();
  }
  const uint16_t& pdcp_sn12() const
  {
    assert_choice_type(types::pdcp_sn12, type_, "MRB-ProgressInformationSNs");
    return c.get<uint16_t>();
  }
  const uint32_t& pdcp_sn18() const
  {
    assert_choice_type(types::pdcp_sn18, type_, "MRB-ProgressInformationSNs");
    return c.get<uint32_t>();
  }
  const protocol_ie_single_container_s<mrb_progress_info_sn_s_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "MRB-ProgressInformationSNs");
    return c.get<protocol_ie_single_container_s<mrb_progress_info_sn_s_ext_ies_o>>();
  }
  uint16_t&                                                         set_pdcp_sn12();
  uint32_t&                                                         set_pdcp_sn18();
  protocol_ie_single_container_s<mrb_progress_info_sn_s_ext_ies_o>& set_choice_ext();

private:
  types                                                                             type_;
  choice_buffer_t<protocol_ie_single_container_s<mrb_progress_info_sn_s_ext_ies_o>> c;

  void destroy_();
};

// MRB-ProgressInformationType ::= ENUMERATED
struct mrb_progress_info_type_opts {
  enum options { oldest_available, last_delivered, /*...*/ nulltype } value;

  const char* to_string() const;
};
using mrb_progress_info_type_e = enumerated<mrb_progress_info_type_opts, true>;

using ecgi_ext_ies_container = protocol_ext_container_empty_l;

// ECGI ::= SEQUENCE
struct ecgi_s {
  bool                             ie_exts_present = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<28, false, true> eutran_cell_id;
  ecgi_ext_ies_container           ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ECGI-Support-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ecgi_support_item_ext_ies_o = protocol_ext_empty_o;

// EUTRAN-QoS-Support-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using eutran_qos_support_item_ext_ies_o = protocol_ext_empty_o;

// Extended-NR-CGI-Support-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using extended_nr_cgi_support_item_ext_ies_o = protocol_ext_empty_o;

// GTPTLA-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gtp_tla_item_ext_ies_o = protocol_ext_empty_o;

// MBSMulticastF1UContextDescriptor-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mbs_multicast_f1_u_context_descriptor_ext_ies_o = protocol_ext_empty_o;

using mbs_session_associated_info_item_ext_ies_container = protocol_ext_container_empty_l;

// MBSSessionAssociatedInformation-Item ::= SEQUENCE
struct mbs_session_associated_info_item_s {
  bool                                               ext                            = false;
  bool                                               ie_exts_present                = false;
  uint8_t                                            mbs_qos_flow_id                = 0;
  uint8_t                                            associated_unicast_qos_flow_id = 0;
  mbs_session_associated_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mrb_progress_info_ext_ies_container = protocol_ext_container_empty_l;

// MRB-ProgressInformation ::= SEQUENCE
struct mrb_progress_info_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  mrb_progress_info_sn_s_c            mrb_progress_info_sn_s;
  mrb_progress_info_type_e            mrb_progress_info_type;
  mrb_progress_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRBForwardingResourceIndication-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mrb_forwarding_res_ind_item_ext_ies_o = protocol_ext_empty_o;

// MRBForwardingResourceRequest-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mrb_forwarding_res_request_item_ext_ies_o = protocol_ext_empty_o;

// MRBForwardingResourceResponse-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mrb_forwarding_res_resp_item_ext_ies_o = protocol_ext_empty_o;

// NG-RAN-QoS-Support-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ng_ran_qos_support_item_ext_ies_o = protocol_ext_empty_o;

// NPNSupportInfo-SNPN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using npn_support_info_sn_pn_ext_ies_o = protocol_ext_empty_o;

// Slice-Support-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using slice_support_item_ext_ies_o = protocol_ext_empty_o;

using ecgi_support_item_ext_ies_container = protocol_ext_container_empty_l;

// ECGI-Support-Item ::= SEQUENCE
struct ecgi_support_item_s {
  bool                                ie_exts_present = false;
  ecgi_s                              ecgi;
  ecgi_support_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using eutran_qos_support_item_ext_ies_container = protocol_ext_container_empty_l;

// EUTRAN-QoS-Support-Item ::= SEQUENCE
struct eutran_qos_support_item_s {
  bool                                      ie_exts_present = false;
  eutran_qos_s                              eutran_qos;
  eutran_qos_support_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using extended_nr_cgi_support_item_ext_ies_container = protocol_ext_container_empty_l;

// Extended-NR-CGI-Support-Item ::= SEQUENCE
struct extended_nr_cgi_support_item_s {
  bool                                           ie_exts_present = false;
  nr_cgi_s                                       nr_cgi;
  extended_nr_cgi_support_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gtp_tla_item_ext_ies_container = protocol_ext_container_empty_l;

// GTPTLA-Item ::= SEQUENCE
struct gtp_tla_item_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  bounded_bitstring<1, 160, true, true> gtp_transport_layer_addresses;
  gtp_tla_item_ext_ies_container        ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mbs_multicast_f1_u_context_descriptor_ext_ies_container = protocol_ext_container_empty_l;

// MBSMulticastF1UContextDescriptor ::= SEQUENCE
struct mbs_multicast_f1_u_context_descriptor_s {
  struct mc_f1_u_ctxtusage_opts {
    enum options { ptm, ptp, ptp_retx, ptp_forwarding, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using mc_f1_u_ctxtusage_e_ = enumerated<mc_f1_u_ctxtusage_opts, true>;

  // member variables
  bool                                                    ext                      = false;
  bool                                                    mbs_area_session_present = false;
  bool                                                    ie_exts_present          = false;
  fixed_octstring<4, true>                                multicast_f1_u_context_ref_e1;
  mc_f1_u_ctxtusage_e_                                    mc_f1_u_ctxtusage;
  uint32_t                                                mbs_area_session = 0;
  mbs_multicast_f1_u_context_descriptor_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSSessionAssociatedInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mbs_session_associated_info_ext_ies_o = protocol_ext_empty_o;

// MBSSessionAssociatedInformationList ::= SEQUENCE (SIZE (1..64)) OF MBSSessionAssociatedInformation-Item
using mbs_session_associated_info_list_l = dyn_array<mbs_session_associated_info_item_s>;

// MCBearerContextF1UTNLInfoatDU-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mc_bearer_context_f1_u_tnl_infoat_du_ext_ies_o = protocol_ext_empty_o;

using mrb_forwarding_res_ind_item_ext_ies_container = protocol_ext_container_empty_l;

// MRBForwardingResourceIndication-Item ::= SEQUENCE
struct mrb_forwarding_res_ind_item_s {
  bool                                          ext                            = false;
  bool                                          mrb_progress_info_present      = false;
  bool                                          mrb_forwarding_address_present = false;
  bool                                          ie_exts_present                = false;
  uint16_t                                      mrb_id                         = 1;
  mrb_progress_info_s                           mrb_progress_info;
  up_tnl_info_c                                 mrb_forwarding_address;
  mrb_forwarding_res_ind_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mrb_forwarding_res_request_item_ext_ies_container = protocol_ext_container_empty_l;

// MRBForwardingResourceRequest-Item ::= SEQUENCE
struct mrb_forwarding_res_request_item_s {
  struct mrb_forwarding_address_request_opts {
    enum options { request, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using mrb_forwarding_address_request_e_ = enumerated<mrb_forwarding_address_request_opts, true>;

  // member variables
  bool                                              ext                                    = false;
  bool                                              mrb_progress_request_type_present      = false;
  bool                                              mrb_forwarding_address_request_present = false;
  bool                                              ie_exts_present                        = false;
  uint16_t                                          mrb_id                                 = 1;
  mrb_progress_info_type_e                          mrb_progress_request_type;
  mrb_forwarding_address_request_e_                 mrb_forwarding_address_request;
  mrb_forwarding_res_request_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mrb_forwarding_res_resp_item_ext_ies_container = protocol_ext_container_empty_l;

// MRBForwardingResourceResponse-Item ::= SEQUENCE
struct mrb_forwarding_res_resp_item_s {
  bool                                           ext                            = false;
  bool                                           mrb_progress_info_present      = false;
  bool                                           mrb_forwarding_address_present = false;
  bool                                           ie_exts_present                = false;
  uint16_t                                       mrb_id                         = 1;
  mrb_progress_info_s                            mrb_progress_info;
  up_tnl_info_c                                  mrb_forwarding_address;
  mrb_forwarding_res_resp_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ng_ran_qos_support_item_ext_ies_container = protocol_ext_container_empty_l;

// NG-RAN-QoS-Support-Item ::= SEQUENCE
struct ng_ran_qos_support_item_s {
  bool                                      ie_exts_present = false;
  non_dyn_5qi_descriptor_s                  non_dyn_5qi_descriptor;
  ng_ran_qos_support_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPNSupportInfo-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using npn_support_info_ext_ies_o = protocol_ies_empty_o;

using npn_support_info_sn_pn_ext_ies_container = protocol_ext_container_empty_l;

// NPNSupportInfo-SNPN ::= SEQUENCE
struct npn_support_info_sn_pn_s {
  bool                                     ie_exts_present = false;
  fixed_bitstring<44, false, true>         nid;
  npn_support_info_sn_pn_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-CGI-Support-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using nr_cgi_support_item_ext_ies_o = protocol_ext_empty_o;

using slice_support_item_ext_ies_container = protocol_ext_container_empty_l;

// Slice-Support-Item ::= SEQUENCE
struct slice_support_item_s {
  bool                                 ie_exts_present = false;
  snssai_s                             snssai;
  slice_support_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-associatedLogicalE1-ConnectionItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ue_associated_lc_e1_conn_item_ext_ies_o = protocol_ext_empty_o;

// ECGI-Support-List ::= SEQUENCE (SIZE (1..512)) OF ECGI-Support-Item
using ecgi_support_list_l = dyn_array<ecgi_support_item_s>;

// EUTRAN-QoS-Support-List ::= SEQUENCE (SIZE (1..256)) OF EUTRAN-QoS-Support-Item
using eutran_qos_support_list_l = dyn_array<eutran_qos_support_item_s>;

// Extended-NR-CGI-Support-List ::= SEQUENCE (SIZE (1..16384)) OF Extended-NR-CGI-Support-Item
using extended_nr_cgi_support_list_l = dyn_array<extended_nr_cgi_support_item_s>;

// ExtendedSliceSupportList ::= SEQUENCE (SIZE (1..65535)) OF Slice-Support-Item
using extended_slice_support_list_l = dyn_array<slice_support_item_s>;

// GTPTLAs ::= SEQUENCE (SIZE (1..16)) OF GTPTLA-Item
using gtp_tlas_l = dyn_array<gtp_tla_item_s>;

// MBS-PDCP-COUNT-Req ::= ENUMERATED
struct mbs_pdcp_count_req_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using mbs_pdcp_count_req_e = enumerated<mbs_pdcp_count_req_opts, true>;

// MBS-Support-Info-ToAdd-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mbs_support_info_to_add_item_ext_ies_o = protocol_ext_empty_o;

// MBS-Support-Info-ToRemove-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mbs_support_info_to_rem_item_ext_ies_o = protocol_ext_empty_o;

// MBSSessionAssociatedInfoNonSupportToSupport-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mbs_session_associated_info_non_support_to_support_ext_ies_o = protocol_ext_empty_o;

using mbs_session_associated_info_ext_ies_container = protocol_ext_container_empty_l;

// MBSSessionAssociatedInformation ::= SEQUENCE
struct mbs_session_associated_info_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  mbs_session_associated_info_list_l            mbs_session_associated_info_list;
  up_tnl_info_c                                 mbs_session_forwarding_address;
  mbs_session_associated_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mc_bearer_context_f1_u_tnl_infoat_du_ext_ies_container = protocol_ext_container_empty_l;

// MCBearerContextF1UTNLInfoatDU ::= SEQUENCE
struct mc_bearer_context_f1_u_tnl_infoat_du_s {
  bool                                                   ext             = false;
  bool                                                   ie_exts_present = false;
  up_tnl_info_c                                          mbs_f1_u_infoat_du;
  mbs_multicast_f1_u_context_descriptor_s                mbs_multicast_f1_u_context_descriptor;
  mc_bearer_context_f1_u_tnl_infoat_du_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MCForwardingResourceIndication-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mc_forwarding_res_ind_ext_ies_o = protocol_ext_empty_o;

// MCForwardingResourceRelease-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mc_forwarding_res_release_ext_ies_o = protocol_ext_empty_o;

// MCForwardingResourceReleaseIndication-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mc_forwarding_res_release_ind_ext_ies_o = protocol_ext_empty_o;

// MCForwardingResourceRequest-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mc_forwarding_res_request_ext_ies_o = protocol_ext_empty_o;

// MCForwardingResourceResponse-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mc_forwarding_res_resp_ext_ies_o = protocol_ext_empty_o;

// MCMRBFailedList-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mcmrb_failed_list_item_ext_ies_o = protocol_ext_empty_o;

// MCMRBModifyConfirmList-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mcmrb_modify_confirm_list_item_ext_ies_o = protocol_ext_empty_o;

// MCMRBModifyRequiredConfiguration-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mcmrb_modify_required_cfg_item_ext_ies_o = protocol_ext_empty_o;

// MCMRBSetupConfiguration-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mcmrb_setup_cfg_item_ext_ies_o = protocol_ext_empty_o;

// MCMRBSetupModifyConfiguration-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mcmrb_setup_modify_cfg_item_ext_ies_o = protocol_ext_empty_o;

// MCMRBSetupModifyResponseList-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mcmrb_setup_modify_resp_list_item_ext_ies_o = protocol_ext_empty_o;

// MCMRBSetupResponseList-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mcmrb_setup_resp_list_item_ext_ies_o = protocol_ext_empty_o;

// MRBForwardingResourceIndicationList ::= SEQUENCE (SIZE (1..64)) OF MRBForwardingResourceIndication-Item
using mrb_forwarding_res_ind_list_l = dyn_array<mrb_forwarding_res_ind_item_s>;

// MRBForwardingResourceRequestList ::= SEQUENCE (SIZE (1..64)) OF MRBForwardingResourceRequest-Item
using mrb_forwarding_res_request_list_l = dyn_array<mrb_forwarding_res_request_item_s>;

// MRBForwardingResourceResponseList ::= SEQUENCE (SIZE (1..64)) OF MRBForwardingResourceResponse-Item
using mrb_forwarding_res_resp_list_l = dyn_array<mrb_forwarding_res_resp_item_s>;

// MRDC-Usage-Information-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mrdc_usage_info_ext_ies_o = protocol_ext_empty_o;

// NG-RAN-QoS-Support-List ::= SEQUENCE (SIZE (1..256)) OF NG-RAN-QoS-Support-Item
using ng_ran_qos_support_list_l = dyn_array<ng_ran_qos_support_item_s>;

// NPNSupportInfo ::= CHOICE
struct npn_support_info_c {
  struct types_opts {
    enum options { sn_pn, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  npn_support_info_c() = default;
  npn_support_info_c(const npn_support_info_c& other);
  npn_support_info_c& operator=(const npn_support_info_c& other);
  ~npn_support_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  npn_support_info_sn_pn_s& sn_pn()
  {
    assert_choice_type(types::sn_pn, type_, "NPNSupportInfo");
    return c.get<npn_support_info_sn_pn_s>();
  }
  protocol_ie_single_container_s<npn_support_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "NPNSupportInfo");
    return c.get<protocol_ie_single_container_s<npn_support_info_ext_ies_o>>();
  }
  const npn_support_info_sn_pn_s& sn_pn() const
  {
    assert_choice_type(types::sn_pn, type_, "NPNSupportInfo");
    return c.get<npn_support_info_sn_pn_s>();
  }
  const protocol_ie_single_container_s<npn_support_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "NPNSupportInfo");
    return c.get<protocol_ie_single_container_s<npn_support_info_ext_ies_o>>();
  }
  npn_support_info_sn_pn_s&                                   set_sn_pn();
  protocol_ie_single_container_s<npn_support_info_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                 type_;
  choice_buffer_t<npn_support_info_sn_pn_s, protocol_ie_single_container_s<npn_support_info_ext_ies_o>> c;

  void destroy_();
};

using nr_cgi_support_item_ext_ies_container = protocol_ext_container_empty_l;

// NR-CGI-Support-Item ::= SEQUENCE
struct nr_cgi_support_item_s {
  bool                                  ie_exts_present = false;
  nr_cgi_s                              nr_cgi;
  nr_cgi_support_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoS-Parameters-Support-List-ItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using qos_params_support_list_item_ext_ies_o = protocol_ext_empty_o;

// Transport-UP-Layer-Addresses-Info-To-Add-ItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using transport_up_layer_addresses_info_to_add_item_ext_ies_o = protocol_ext_empty_o;

// Transport-UP-Layer-Addresses-Info-To-Remove-ItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using transport_up_layer_addresses_info_to_rem_item_ext_ies_o = protocol_ext_empty_o;

using ue_associated_lc_e1_conn_item_ext_ies_container = protocol_ext_container_empty_l;

// UE-associatedLogicalE1-ConnectionItem ::= SEQUENCE
struct ue_associated_lc_e1_conn_item_s {
  bool                                            ext                          = false;
  bool                                            gnb_cu_cp_ue_e1ap_id_present = false;
  bool                                            gnb_cu_up_ue_e1ap_id_present = false;
  bool                                            ie_exts_present              = false;
  uint64_t                                        gnb_cu_cp_ue_e1ap_id         = 0;
  uint64_t                                        gnb_cu_up_ue_e1ap_id         = 0;
  ue_associated_lc_e1_conn_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-CP-TNLA-Failed-To-Setup-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gnb_cu_cp_tnl_a_failed_to_setup_item_ext_ies_o = protocol_ext_empty_o;

// GNB-CU-CP-TNLA-Setup-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gnb_cu_cp_tnl_a_setup_item_ext_ies_o = protocol_ext_empty_o;

// GNB-CU-CP-TNLA-To-Add-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gnb_cu_cp_tnl_a_to_add_item_ext_ies_o = protocol_ext_empty_o;

// GNB-CU-CP-TNLA-To-Remove-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { tnl_assoc_transport_layer_address_gnb_cu_up, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::tnl_assoc_transport_layer_address_gnb_cu_up; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cp_tnl_info_c&       tnl_assoc_transport_layer_address_gnb_cu_up() { return c; }
    const cp_tnl_info_c& tnl_assoc_transport_layer_address_gnb_cu_up() const { return c; }

  private:
    cp_tnl_info_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-CP-TNLA-To-Update-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gnb_cu_cp_tnl_a_to_upd_item_ext_ies_o = protocol_ext_empty_o;

// GNB-CU-UP-TNLA-To-Remove-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gnb_cu_up_tnl_a_to_rem_item_ext_ies_o = protocol_ext_empty_o;

// IAB-donor-CU-UPPSKInfoItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using iab_donor_cu_up_psk_info_item_ext_ies_o = protocol_ext_empty_o;

// MBS-DL-Data-Arrival-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mbs_dl_data_arrival_ext_ies_o = protocol_ext_empty_o;

using mbs_support_info_to_add_item_ext_ies_container = protocol_ext_container_empty_l;

// MBS-Support-Info-ToAdd-Item ::= SEQUENCE
struct mbs_support_info_to_add_item_s {
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  global_mbs_session_id_s                        global_mbs_session_id;
  mbs_support_info_to_add_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mbs_support_info_to_rem_item_ext_ies_container = protocol_ext_container_empty_l;

// MBS-Support-Info-ToRemove-Item ::= SEQUENCE
struct mbs_support_info_to_rem_item_s {
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  global_mbs_session_id_s                        global_mbs_session_id;
  mbs_support_info_to_rem_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mbs_session_associated_info_non_support_to_support_ext_ies_container = protocol_ext_container_empty_l;

// MBSSessionAssociatedInfoNonSupportToSupport ::= SEQUENCE
struct mbs_session_associated_info_non_support_to_support_s {
  bool                                                                 ext             = false;
  bool                                                                 ie_exts_present = false;
  uint64_t                                                             ue_ref_id       = 0;
  uint16_t                                                             pdu_session_id  = 0;
  mbs_session_associated_info_list_l                                   associated_qos_flow_info_list;
  mbs_session_associated_info_non_support_to_support_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MCBearerContext-Inactivity-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mc_bearer_context_inactivity_ext_ies_o = protocol_ext_empty_o;

// MCBearerContextNGU-TNLInfoatNGRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using mc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o = protocol_ies_empty_o;

// MCBearerContextNGU-TNLInfoatNGRANModifyResponse-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_ext_ies_o = protocol_ext_empty_o;

// MCBearerContextNGUTNLInfoat5GC-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mc_bearer_context_ngu_tnl_infoat5_gc_ext_ies_o = protocol_ext_empty_o;

// MCBearerContextNGUTnlInfoatNGRANRequest-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mc_bearer_context_ngu_tnl_infoat_ngra_nr_equest_ext_ies_o = protocol_ext_empty_o;

// MCBearerContextStatusChange ::= ENUMERATED
struct mc_bearer_context_status_change_opts {
  enum options { suspend, resume, /*...*/ nulltype } value;

  const char* to_string() const;
};
using mc_bearer_context_status_change_e = enumerated<mc_bearer_context_status_change_opts, true>;

using mc_forwarding_res_ind_ext_ies_container = protocol_ext_container_empty_l;

// MCForwardingResourceIndication ::= SEQUENCE
struct mc_forwarding_res_ind_s {
  bool                                    ext                                 = false;
  bool                                    mbs_session_associated_info_present = false;
  bool                                    ie_exts_present                     = false;
  fixed_octstring<2, true>                mc_forwarding_res_id;
  mrb_forwarding_res_ind_list_l           mrb_forwarding_res_ind_list;
  mbs_session_associated_info_s           mbs_session_associated_info;
  mc_forwarding_res_ind_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mc_forwarding_res_release_ext_ies_container = protocol_ext_container_empty_l;

// MCForwardingResourceRelease ::= SEQUENCE
struct mc_forwarding_res_release_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  fixed_octstring<2, true>                    mc_forwarding_res_id;
  mc_forwarding_res_release_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mc_forwarding_res_release_ind_ext_ies_container = protocol_ext_container_empty_l;

// MCForwardingResourceReleaseIndication ::= SEQUENCE
struct mc_forwarding_res_release_ind_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  fixed_octstring<2, true>                        mc_forwarding_res_id;
  mc_forwarding_res_release_ind_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mc_forwarding_res_request_ext_ies_container = protocol_ext_container_empty_l;

// MCForwardingResourceRequest ::= SEQUENCE
struct mc_forwarding_res_request_s {
  bool                                        ext                         = false;
  bool                                        mbs_area_session_id_present = false;
  bool                                        ie_exts_present             = false;
  fixed_octstring<2, true>                    mc_forwarding_res_id;
  uint32_t                                    mbs_area_session_id = 0;
  mrb_forwarding_res_request_list_l           mrb_forwarding_res_request_list;
  mc_forwarding_res_request_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mc_forwarding_res_resp_ext_ies_container = protocol_ext_container_empty_l;

// MCForwardingResourceResponse ::= SEQUENCE
struct mc_forwarding_res_resp_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  fixed_octstring<2, true>                 mc_forwarding_res_id;
  mrb_forwarding_res_resp_list_l           mrb_forwarding_res_resp_list;
  mc_forwarding_res_resp_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mcmrb_failed_list_item_ext_ies_container = protocol_ext_container_empty_l;

// MCMRBFailedList-Item ::= SEQUENCE
struct mcmrb_failed_list_item_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  uint16_t                                 mrb_id          = 1;
  cause_c                                  cause;
  mcmrb_failed_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mcmrb_modify_confirm_list_item_ext_ies_container = protocol_ext_container_empty_l;

// MCMRBModifyConfirmList-Item ::= SEQUENCE
struct mcmrb_modify_confirm_list_item_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  uint16_t                                         mrb_id          = 1;
  mcmrb_modify_confirm_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mcmrb_modify_required_cfg_item_ext_ies_container = protocol_ext_container_empty_l;

// MCMRBModifyRequiredConfiguration-Item ::= SEQUENCE
struct mcmrb_modify_required_cfg_item_s {
  bool                                             ext                    = false;
  bool                                             mbs_pdcp_count_present = false;
  bool                                             ie_exts_present        = false;
  uint16_t                                         mrb_id                 = 1;
  fixed_bitstring<32, false, true>                 mbs_pdcp_count;
  mcmrb_modify_required_cfg_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mcmrb_setup_cfg_item_ext_ies_container = protocol_ext_container_empty_l;

// MCMRBSetupConfiguration-Item ::= SEQUENCE
struct mcmrb_setup_cfg_item_s {
  bool                                   ext                               = false;
  bool                                   qos_flow_level_qos_params_present = false;
  bool                                   ie_exts_present                   = false;
  uint16_t                               mrb_id                            = 1;
  pdcp_cfg_s                             mbs_pdcp_cfg;
  qos_flow_qos_param_list_l              qos_flow_qos_param_list;
  qos_flow_level_qos_params_s            qos_flow_level_qos_params;
  mcmrb_setup_cfg_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mcmrb_setup_modify_cfg_item_ext_ies_container = protocol_ext_container_empty_l;

// MCMRBSetupModifyConfiguration-Item ::= SEQUENCE
struct mcmrb_setup_modify_cfg_item_s {
  bool                                          ext                        = false;
  bool                                          f1u_tnl_at_du_present      = false;
  bool                                          mbs_pdcp_cfg_present       = false;
  bool                                          mrb_qos_present            = false;
  bool                                          mbs_pdcp_count_req_present = false;
  bool                                          ie_exts_present            = false;
  uint16_t                                      mrb_id                     = 1;
  mc_bearer_context_f1_u_tnl_infoat_du_s        f1u_tnl_at_du;
  pdcp_cfg_s                                    mbs_pdcp_cfg;
  qos_flow_qos_param_list_l                     qos_flow_qos_param_list;
  qos_flow_level_qos_params_s                   mrb_qos;
  mbs_pdcp_count_req_e                          mbs_pdcp_count_req;
  mcmrb_setup_modify_cfg_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mcmrb_setup_modify_resp_list_item_ext_ies_container = protocol_ext_container_empty_l;

// MCMRBSetupModifyResponseList-Item ::= SEQUENCE
struct mcmrb_setup_modify_resp_list_item_s {
  bool                                                ext                                          = false;
  bool                                                mc_bearer_context_f1_u_tnl_infoat_cu_present = false;
  bool                                                mbs_pdcp_count_present                       = false;
  bool                                                ie_exts_present                              = false;
  uint16_t                                            mrb_id                                       = 1;
  qos_flow_list_l                                     qosflow_setup;
  qos_flow_failed_list_l                              qosflow_failed;
  up_tnl_info_c                                       mc_bearer_context_f1_u_tnl_infoat_cu;
  fixed_bitstring<32, false, true>                    mbs_pdcp_count;
  mcmrb_setup_modify_resp_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mcmrb_setup_resp_list_item_ext_ies_container = protocol_ext_container_empty_l;

// MCMRBSetupResponseList-Item ::= SEQUENCE
struct mcmrb_setup_resp_list_item_s {
  bool                                         ext                    = false;
  bool                                         mbs_pdcp_count_present = false;
  bool                                         ie_exts_present        = false;
  uint16_t                                     mrb_id                 = 1;
  qos_flow_list_l                              qosflow_setup;
  qos_flow_failed_list_l                       qosflow_failed;
  fixed_bitstring<32, false, true>             mbs_pdcp_count;
  mcmrb_setup_resp_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mrdc_usage_info_ext_ies_container = protocol_ext_container_empty_l;

// MRDC-Usage-Information ::= SEQUENCE
struct mrdc_usage_info_s {
  bool                                ext                                       = false;
  bool                                data_usage_per_pdu_session_report_present = false;
  bool                                ie_exts_present                           = false;
  data_usage_per_pdu_session_report_s data_usage_per_pdu_session_report;
  data_usage_per_qos_flow_list_l      data_usage_per_qos_flow_list;
  mrdc_usage_info_ext_ies_container   ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-CGI-Support-List ::= SEQUENCE (SIZE (1..512)) OF NR-CGI-Support-Item
using nr_cgi_support_list_l = dyn_array<nr_cgi_support_item_s>;

// PDU-Session-Resource-Data-Usage-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using pdu_session_res_data_usage_item_ext_ies_o = protocol_ext_empty_o;

using qos_params_support_list_item_ext_ies_container = protocol_ext_container_empty_l;

// QoS-Parameters-Support-List ::= SEQUENCE
struct qos_params_support_list_s {
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  eutran_qos_support_list_l                      eutran_qos_support_list;
  ng_ran_qos_support_list_l                      ng_ran_qos_support_list;
  qos_params_support_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Slice-Support-List ::= SEQUENCE (SIZE (1..1024)) OF Slice-Support-Item
using slice_support_list_l = dyn_array<slice_support_item_s>;

// TNLAssociationUsage ::= ENUMERATED
struct tnl_assoc_usage_opts {
  enum options { ue, non_ue, both, /*...*/ nulltype } value;

  const char* to_string() const;
};
using tnl_assoc_usage_e = enumerated<tnl_assoc_usage_opts, true>;

using transport_up_layer_addresses_info_to_add_item_ext_ies_container = protocol_ext_container_empty_l;

// Transport-UP-Layer-Addresses-Info-To-Add-Item ::= SEQUENCE
struct transport_up_layer_addresses_info_to_add_item_s {
  bool                                                            ext             = false;
  bool                                                            ie_exts_present = false;
  bounded_bitstring<1, 160, true, true>                           ip_sec_transport_layer_address;
  gtp_tlas_l                                                      gtp_transport_layer_addresses_to_add;
  transport_up_layer_addresses_info_to_add_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using transport_up_layer_addresses_info_to_rem_item_ext_ies_container = protocol_ext_container_empty_l;

// Transport-UP-Layer-Addresses-Info-To-Remove-Item ::= SEQUENCE
struct transport_up_layer_addresses_info_to_rem_item_s {
  bool                                                            ext             = false;
  bool                                                            ie_exts_present = false;
  bounded_bitstring<1, 160, true, true>                           ip_sec_transport_layer_address;
  gtp_tlas_l                                                      gtp_transport_layer_addresses_to_rem;
  transport_up_layer_addresses_info_to_rem_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULUPTNLAddressToUpdateItemExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using ul_up_tnl_address_to_upd_item_ext_ies_o = protocol_ext_empty_o;

// Extended-GNB-CU-CP-Name-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using extended_gnb_cu_cp_name_ext_ies_o = protocol_ext_empty_o;

// Extended-GNB-CU-UP-Name-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using extended_gnb_cu_up_name_ext_ies_o = protocol_ext_empty_o;

using gnb_cu_cp_tnl_a_failed_to_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-CP-TNLA-Failed-To-Setup-Item ::= SEQUENCE
struct gnb_cu_cp_tnl_a_failed_to_setup_item_s {
  bool                                                   ie_exts_present = false;
  cp_tnl_info_c                                          tnl_assoc_transport_layer_address;
  cause_c                                                cause;
  gnb_cu_cp_tnl_a_failed_to_setup_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gnb_cu_cp_tnl_a_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-CP-TNLA-Setup-Item ::= SEQUENCE
struct gnb_cu_cp_tnl_a_setup_item_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  cp_tnl_info_c                                tnl_assoc_transport_layer_address;
  gnb_cu_cp_tnl_a_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gnb_cu_cp_tnl_a_to_add_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-CP-TNLA-To-Add-Item ::= SEQUENCE
struct gnb_cu_cp_tnl_a_to_add_item_s {
  bool                                          ie_exts_present = false;
  cp_tnl_info_c                                 tnl_assoc_transport_layer_address;
  tnl_assoc_usage_e                             tnl_assoc_usage;
  gnb_cu_cp_tnl_a_to_add_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-CP-TNLA-To-Remove-Item ::= SEQUENCE
struct gnb_cu_cp_tnl_a_to_rem_item_s {
  cp_tnl_info_c                                                   tnl_assoc_transport_layer_address;
  protocol_ext_container_l<gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o> ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gnb_cu_cp_tnl_a_to_upd_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-CP-TNLA-To-Update-Item ::= SEQUENCE
struct gnb_cu_cp_tnl_a_to_upd_item_s {
  bool                                          tnl_assoc_usage_present = false;
  bool                                          ie_exts_present         = false;
  cp_tnl_info_c                                 tnl_assoc_transport_layer_address;
  tnl_assoc_usage_e                             tnl_assoc_usage;
  gnb_cu_cp_tnl_a_to_upd_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-UP-MBS-Support-Info-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using gnb_cu_up_mbs_support_info_ext_ies_o = protocol_ext_empty_o;

using gnb_cu_up_tnl_a_to_rem_item_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-UP-TNLA-To-Remove-Item ::= SEQUENCE
struct gnb_cu_up_tnl_a_to_rem_item_s {
  bool                                          tnl_assoc_transport_layer_address_gnb_cu_cp_present = false;
  bool                                          ie_exts_present                                     = false;
  cp_tnl_info_c                                 tnl_assoc_transport_layer_address;
  cp_tnl_info_c                                 tnl_assoc_transport_layer_address_gnb_cu_cp;
  gnb_cu_up_tnl_a_to_rem_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HW-CapacityIndicator-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using hw_capacity_ind_ext_ies_o = protocol_ext_empty_o;

using iab_donor_cu_up_psk_info_item_ext_ies_container = protocol_ext_container_empty_l;

// IAB-Donor-CU-UPPSKInfo-Item ::= SEQUENCE
struct iab_donor_cu_up_psk_info_item_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  unbounded_octstring<true>                       iab_donor_cu_up_psk;
  bounded_bitstring<1, 160, true, true>           iab_donor_cu_up_ip_address;
  bounded_bitstring<1, 160, true, true>           iab_du_ip_address;
  iab_donor_cu_up_psk_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mbs_dl_data_arrival_ext_ies_container = protocol_ext_container_empty_l;

// MBS-DL-Data-Arrival ::= SEQUENCE
struct mbs_dl_data_arrival_s {
  struct dl_data_arrival_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using dl_data_arrival_e_ = enumerated<dl_data_arrival_opts, true>;

  // member variables
  bool                                  ext             = false;
  bool                                  ppi_present     = false;
  bool                                  ie_exts_present = false;
  dl_data_arrival_e_                    dl_data_arrival;
  uint8_t                               ppi = 0;
  mbs_dl_data_arrival_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-Support-Info-ToAdd-List ::= SEQUENCE (SIZE (1..512)) OF MBS-Support-Info-ToAdd-Item
using mbs_support_info_to_add_list_l = dyn_array<mbs_support_info_to_add_item_s>;

// MBS-Support-Info-ToRemove-List ::= SEQUENCE (SIZE (1..512)) OF MBS-Support-Info-ToRemove-Item
using mbs_support_info_to_rem_list_l = dyn_array<mbs_support_info_to_rem_item_s>;

// MBSSessionResourceNotification-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
using mbs_session_res_notif_ext_ies_o = protocol_ies_empty_o;

using mc_bearer_context_inactivity_ext_ies_container = protocol_ext_container_empty_l;

// MCBearerContext-Inactivity ::= SEQUENCE
struct mc_bearer_context_inactivity_s {
  struct mc_bearer_context_inactivity_ind_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using mc_bearer_context_inactivity_ind_e_ = enumerated<mc_bearer_context_inactivity_ind_opts, true>;

  // member variables
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  mc_bearer_context_inactivity_ind_e_            mc_bearer_context_inactivity_ind;
  mc_bearer_context_inactivity_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MCBearerContextNGU-TNLInfoatNGRAN ::= CHOICE
struct mc_bearer_context_ngu_tnl_infoat_ngran_c {
  struct types_opts {
    enum options { locationindependent, locationdependent, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  mc_bearer_context_ngu_tnl_infoat_ngran_c() = default;
  mc_bearer_context_ngu_tnl_infoat_ngran_c(const mc_bearer_context_ngu_tnl_infoat_ngran_c& other);
  mc_bearer_context_ngu_tnl_infoat_ngran_c& operator=(const mc_bearer_context_ngu_tnl_infoat_ngran_c& other);
  ~mc_bearer_context_ngu_tnl_infoat_ngran_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  mb_sn_gu_info_at_ngran_c& locationindependent()
  {
    assert_choice_type(types::locationindependent, type_, "MCBearerContextNGU-TNLInfoatNGRAN");
    return c.get<mb_sn_gu_info_at_ngran_c>();
  }
  location_dependent_mb_sn_gu_info_at_ngran_l& locationdependent()
  {
    assert_choice_type(types::locationdependent, type_, "MCBearerContextNGU-TNLInfoatNGRAN");
    return c.get<location_dependent_mb_sn_gu_info_at_ngran_l>();
  }
  protocol_ie_single_container_s<mc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "MCBearerContextNGU-TNLInfoatNGRAN");
    return c.get<protocol_ie_single_container_s<mc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>>();
  }
  const mb_sn_gu_info_at_ngran_c& locationindependent() const
  {
    assert_choice_type(types::locationindependent, type_, "MCBearerContextNGU-TNLInfoatNGRAN");
    return c.get<mb_sn_gu_info_at_ngran_c>();
  }
  const location_dependent_mb_sn_gu_info_at_ngran_l& locationdependent() const
  {
    assert_choice_type(types::locationdependent, type_, "MCBearerContextNGU-TNLInfoatNGRAN");
    return c.get<location_dependent_mb_sn_gu_info_at_ngran_l>();
  }
  const protocol_ie_single_container_s<mc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "MCBearerContextNGU-TNLInfoatNGRAN");
    return c.get<protocol_ie_single_container_s<mc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>>();
  }
  mb_sn_gu_info_at_ngran_c&                                                         set_locationindependent();
  location_dependent_mb_sn_gu_info_at_ngran_l&                                      set_locationdependent();
  protocol_ie_single_container_s<mc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<location_dependent_mb_sn_gu_info_at_ngran_l,
                  mb_sn_gu_info_at_ngran_c,
                  protocol_ie_single_container_s<mc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>>
      c;

  void destroy_();
};

using mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_ext_ies_container = protocol_ext_container_empty_l;

// MCBearerContextNGU-TNLInfoatNGRANModifyResponse ::= SEQUENCE
struct mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s {
  bool                                                                 ext                      = false;
  bool                                                                 mbs_area_session_present = false;
  bool                                                                 ie_exts_present          = false;
  mb_sn_gu_info_at_ngran_c                                             mbs_ngu_infoat_ngran;
  uint32_t                                                             mbs_area_session = 0;
  mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mc_bearer_context_ngu_tnl_infoat5_gc_ext_ies_container = protocol_ext_container_empty_l;

// MCBearerContextNGUTNLInfoat5GC ::= SEQUENCE
struct mc_bearer_context_ngu_tnl_infoat5_gc_s {
  bool                                                   ext                         = false;
  bool                                                   mbs_area_session_id_present = false;
  bool                                                   ie_exts_present             = false;
  mb_sn_gu_info_at5_gc_c                                 mbs_ngu_info_at5_gc;
  uint32_t                                               mbs_area_session_id = 0;
  mc_bearer_context_ngu_tnl_infoat5_gc_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mc_bearer_context_ngu_tnl_infoat_ngra_nr_equest_ext_ies_container = protocol_ext_container_empty_l;

// MCBearerContextNGUTnlInfoatNGRANRequest ::= SEQUENCE
struct mc_bearer_context_ngu_tnl_infoat_ngra_nr_equest_s {
  struct ng_ranngu_tnl_requested_opts {
    enum options { requested, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ng_ranngu_tnl_requested_e_ = enumerated<ng_ranngu_tnl_requested_opts, true>;

  // member variables
  bool                                                              ext                         = false;
  bool                                                              mbs_area_session_id_present = false;
  bool                                                              ie_exts_present             = false;
  ng_ranngu_tnl_requested_e_                                        ng_ranngu_tnl_requested;
  uint32_t                                                          mbs_area_session_id = 0;
  mc_bearer_context_ngu_tnl_infoat_ngra_nr_equest_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MCBearerContextToModify-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct mc_bearer_context_to_modify_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        mc_forwarding_res_request,
        mc_forwarding_res_ind,
        mc_forwarding_res_release,
        mbs_session_associated_info_non_support_to_support,
        mc_bearer_context_inactivity_timer,
        mc_bearer_context_status_change,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mc_forwarding_res_request_s&                          mc_forwarding_res_request();
    mc_forwarding_res_ind_s&                              mc_forwarding_res_ind();
    mc_forwarding_res_release_s&                          mc_forwarding_res_release();
    mbs_session_associated_info_non_support_to_support_s& mbs_session_associated_info_non_support_to_support();
    uint16_t&                                             mc_bearer_context_inactivity_timer();
    mc_bearer_context_status_change_e&                    mc_bearer_context_status_change();
    const mc_forwarding_res_request_s&                    mc_forwarding_res_request() const;
    const mc_forwarding_res_ind_s&                        mc_forwarding_res_ind() const;
    const mc_forwarding_res_release_s&                    mc_forwarding_res_release() const;
    const mbs_session_associated_info_non_support_to_support_s&
                                             mbs_session_associated_info_non_support_to_support() const;
    const uint16_t&                          mc_bearer_context_inactivity_timer() const;
    const mc_bearer_context_status_change_e& mc_bearer_context_status_change() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MCBearerContextToModifyConfirm-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mc_bearer_context_to_modify_confirm_ext_ies_o = protocol_ext_empty_o;

// MCBearerContextToModifyRequired-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct mc_bearer_context_to_modify_required_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { mc_forwarding_res_release_ind, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::mc_forwarding_res_release_ind; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mc_forwarding_res_release_ind_s&       mc_forwarding_res_release_ind() { return c; }
    const mc_forwarding_res_release_ind_s& mc_forwarding_res_release_ind() const { return c; }

  private:
    mc_forwarding_res_release_ind_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MCBearerContextToModifyResponse-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct mc_bearer_context_to_modify_resp_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { mc_forwarding_res_resp, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::mc_forwarding_res_resp; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mc_forwarding_res_resp_s&       mc_forwarding_res_resp() { return c; }
    const mc_forwarding_res_resp_s& mc_forwarding_res_resp() const { return c; }

  private:
    mc_forwarding_res_resp_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MCBearerContextToSetup-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
struct mc_bearer_context_to_setup_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        mbs_session_associated_info_non_support_to_support,
        mbs_area_session_id,
        mc_bearer_context_inactivity_timer,
        mc_bearer_context_status_change,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_associated_info_non_support_to_support_s& mbs_session_associated_info_non_support_to_support();
    uint32_t&                                             mbs_area_session_id();
    uint16_t&                                             mc_bearer_context_inactivity_timer();
    mc_bearer_context_status_change_e&                    mc_bearer_context_status_change();
    const mbs_session_associated_info_non_support_to_support_s&
                                             mbs_session_associated_info_non_support_to_support() const;
    const uint32_t&                          mbs_area_session_id() const;
    const uint16_t&                          mc_bearer_context_inactivity_timer() const;
    const mc_bearer_context_status_change_e& mc_bearer_context_status_change() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MCBearerContextToSetupResponse-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using mc_bearer_context_to_setup_resp_ext_ies_o = protocol_ext_empty_o;

// MCMRBFailedList ::= SEQUENCE (SIZE (1..32)) OF MCMRBFailedList-Item
using mcmrb_failed_list_l = dyn_array<mcmrb_failed_list_item_s>;

// MCMRBModifyConfirmList ::= SEQUENCE (SIZE (1..32)) OF MCMRBModifyConfirmList-Item
using mcmrb_modify_confirm_list_l = dyn_array<mcmrb_modify_confirm_list_item_s>;

// MCMRBModifyRequiredConfiguration ::= SEQUENCE (SIZE (1..32)) OF MCMRBModifyRequiredConfiguration-Item
using mcmrb_modify_required_cfg_l = dyn_array<mcmrb_modify_required_cfg_item_s>;

// MCMRBRemoveConfiguration ::= SEQUENCE (SIZE (1..32)) OF INTEGER (1..512,...)
using mcmrb_rem_cfg_l = bounded_array<uint16_t, 32>;

// MCMRBSetupConfiguration ::= SEQUENCE (SIZE (1..32)) OF MCMRBSetupConfiguration-Item
using mcmrb_setup_cfg_l = dyn_array<mcmrb_setup_cfg_item_s>;

// MCMRBSetupModifyConfiguration ::= SEQUENCE (SIZE (1..32)) OF MCMRBSetupModifyConfiguration-Item
using mcmrb_setup_modify_cfg_l = dyn_array<mcmrb_setup_modify_cfg_item_s>;

// MCMRBSetupModifyResponseList ::= SEQUENCE (SIZE (1..32)) OF MCMRBSetupModifyResponseList-Item
using mcmrb_setup_modify_resp_list_l = dyn_array<mcmrb_setup_modify_resp_list_item_s>;

// MCMRBSetupResponseList ::= SEQUENCE (SIZE (1..32)) OF MCMRBSetupResponseList-Item
using mcmrb_setup_resp_list_l = dyn_array<mcmrb_setup_resp_list_item_s>;

using pdu_session_res_data_usage_item_ext_ies_container = protocol_ext_container_empty_l;

// PDU-Session-Resource-Data-Usage-Item ::= SEQUENCE
struct pdu_session_res_data_usage_item_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  uint16_t                                          pdu_session_id  = 0;
  mrdc_usage_info_s                                 mrdc_usage_info;
  pdu_session_res_data_usage_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TNL-AvailableCapacityIndicator-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using tnl_available_capacity_ind_ext_ies_o = protocol_ext_empty_o;

// Transport-Layer-Address-Info-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
using transport_layer_address_info_ext_ies_o = protocol_ext_empty_o;

// Transport-UP-Layer-Addresses-Info-To-Add-List ::= SEQUENCE (SIZE (1..16)) OF
// Transport-UP-Layer-Addresses-Info-To-Add-Item
using transport_up_layer_addresses_info_to_add_list_l = dyn_array<transport_up_layer_addresses_info_to_add_item_s>;

// Transport-UP-Layer-Addresses-Info-To-Remove-List ::= SEQUENCE (SIZE (1..16)) OF
// Transport-UP-Layer-Addresses-Info-To-Remove-Item
using transport_up_layer_addresses_info_to_rem_list_l = dyn_array<transport_up_layer_addresses_info_to_rem_item_s>;

using ul_up_tnl_address_to_upd_item_ext_ies_container = protocol_ext_container_empty_l;

// ULUPTNLAddressToUpdateItem ::= SEQUENCE
struct ul_up_tnl_address_to_upd_item_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  bounded_bitstring<1, 160, true, true>           old_tnl_adress;
  bounded_bitstring<1, 160, true, true>           new_tnl_adress;
  ul_up_tnl_address_to_upd_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CNSupport ::= ENUMERATED
struct cn_support_opts {
  enum options { c_epc, c_5gc, both, /*...*/ nulltype } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
using cn_support_e = enumerated<cn_support_opts, true>;

using extended_gnb_cu_cp_name_ext_ies_container = protocol_ext_container_empty_l;

// Extended-GNB-CU-CP-Name ::= SEQUENCE
struct extended_gnb_cu_cp_name_s {
  bool                                      ext                                   = false;
  bool                                      gnb_cu_cp_name_visible_string_present = false;
  bool                                      gnb_cu_cp_name_utf8_string_present    = false;
  bool                                      ie_exts_present                       = false;
  visible_string<1, 150, true, true>        gnb_cu_cp_name_visible_string;
  utf8_string<1, 150, true, true>           gnb_cu_cp_name_utf8_string;
  extended_gnb_cu_cp_name_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using extended_gnb_cu_up_name_ext_ies_container = protocol_ext_container_empty_l;

// Extended-GNB-CU-UP-Name ::= SEQUENCE
struct extended_gnb_cu_up_name_s {
  bool                                      ext                                   = false;
  bool                                      gnb_cu_up_name_visible_string_present = false;
  bool                                      gnb_cu_up_name_utf8_string_present    = false;
  bool                                      ie_exts_present                       = false;
  visible_string<1, 150, true, true>        gnb_cu_up_name_visible_string;
  utf8_string<1, 150, true, true>           gnb_cu_up_name_utf8_string;
  extended_gnb_cu_up_name_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using gnb_cu_up_mbs_support_info_ext_ies_container = protocol_ext_container_empty_l;

// GNB-CU-UP-MBS-Support-Info ::= SEQUENCE
struct gnb_cu_up_mbs_support_info_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  mbs_support_info_to_add_list_l               mbs_support_info_to_add_list;
  mbs_support_info_to_rem_list_l               mbs_support_info_to_rem_list;
  gnb_cu_up_mbs_support_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-CU-UP-OverloadInformation ::= ENUMERATED
struct gnb_cu_up_overload_info_opts {
  enum options { overloaded, not_overloaded, nulltype } value;

  const char* to_string() const;
};
using gnb_cu_up_overload_info_e = enumerated<gnb_cu_up_overload_info_opts>;

using hw_capacity_ind_ext_ies_container = protocol_ext_container_empty_l;

// HW-CapacityIndicator ::= SEQUENCE
struct hw_capacity_ind_s {
  bool                              ext                  = false;
  bool                              ie_exts_present      = false;
  uint32_t                          offered_throughput   = 1;
  uint8_t                           available_throughput = 0;
  hw_capacity_ind_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSSessionResourceNotification ::= CHOICE
struct mbs_session_res_notif_c {
  struct types_opts {
    enum options { mbs_dl_data_arrival, inactivity, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  mbs_session_res_notif_c() = default;
  mbs_session_res_notif_c(const mbs_session_res_notif_c& other);
  mbs_session_res_notif_c& operator=(const mbs_session_res_notif_c& other);
  ~mbs_session_res_notif_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  mbs_dl_data_arrival_s& mbs_dl_data_arrival()
  {
    assert_choice_type(types::mbs_dl_data_arrival, type_, "MBSSessionResourceNotification");
    return c.get<mbs_dl_data_arrival_s>();
  }
  mc_bearer_context_inactivity_s& inactivity()
  {
    assert_choice_type(types::inactivity, type_, "MBSSessionResourceNotification");
    return c.get<mc_bearer_context_inactivity_s>();
  }
  protocol_ie_single_container_s<mbs_session_res_notif_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "MBSSessionResourceNotification");
    return c.get<protocol_ie_single_container_s<mbs_session_res_notif_ext_ies_o>>();
  }
  const mbs_dl_data_arrival_s& mbs_dl_data_arrival() const
  {
    assert_choice_type(types::mbs_dl_data_arrival, type_, "MBSSessionResourceNotification");
    return c.get<mbs_dl_data_arrival_s>();
  }
  const mc_bearer_context_inactivity_s& inactivity() const
  {
    assert_choice_type(types::inactivity, type_, "MBSSessionResourceNotification");
    return c.get<mc_bearer_context_inactivity_s>();
  }
  const protocol_ie_single_container_s<mbs_session_res_notif_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "MBSSessionResourceNotification");
    return c.get<protocol_ie_single_container_s<mbs_session_res_notif_ext_ies_o>>();
  }
  mbs_dl_data_arrival_s&                                           set_mbs_dl_data_arrival();
  mc_bearer_context_inactivity_s&                                  set_inactivity();
  protocol_ie_single_container_s<mbs_session_res_notif_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<mbs_dl_data_arrival_s,
                  mc_bearer_context_inactivity_s,
                  protocol_ie_single_container_s<mbs_session_res_notif_ext_ies_o>>
      c;

  void destroy_();
};

struct mc_bearer_context_to_modify_ext_ies_container {
  bool                        mc_forwarding_res_request_present                          = false;
  bool                        mc_forwarding_res_ind_present                              = false;
  bool                        mc_forwarding_res_release_present                          = false;
  bool                        mbs_session_associated_info_non_support_to_support_present = false;
  bool                        mc_bearer_context_inactivity_timer_present                 = false;
  bool                        mc_bearer_context_status_change_present                    = false;
  mc_forwarding_res_request_s mc_forwarding_res_request;
  mc_forwarding_res_ind_s     mc_forwarding_res_ind;
  mc_forwarding_res_release_s mc_forwarding_res_release;
  mbs_session_associated_info_non_support_to_support_s mbs_session_associated_info_non_support_to_support;
  uint16_t                                             mc_bearer_context_inactivity_timer;
  mc_bearer_context_status_change_e                    mc_bearer_context_status_change;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MCBearerContextToModify ::= SEQUENCE
struct mc_bearer_context_to_modify_s {
  bool                                              ext                                                     = false;
  bool                                              mc_bearer_context_ngu_tnl_infoat5_gc_present            = false;
  bool                                              mc_bearer_context_ngu_tnl_infoat_ngra_nr_equest_present = false;
  bool                                              mbs_multicast_f1_u_context_descriptor_present           = false;
  bool                                              requested_action_present                                = false;
  bool                                              ie_exts_present                                         = false;
  mc_bearer_context_ngu_tnl_infoat5_gc_s            mc_bearer_context_ngu_tnl_infoat5_gc;
  mc_bearer_context_ngu_tnl_infoat_ngra_nr_equest_s mc_bearer_context_ngu_tnl_infoat_ngra_nr_equest;
  mbs_multicast_f1_u_context_descriptor_s           mbs_multicast_f1_u_context_descriptor;
  requested_action4_avail_ngu_termination_e         requested_action;
  mcmrb_setup_modify_cfg_l                          mc_mrb_to_setup_modify_list;
  mcmrb_rem_cfg_l                                   mc_mrb_to_rem_list;
  mc_bearer_context_to_modify_ext_ies_container     ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mc_bearer_context_to_modify_confirm_ext_ies_container = protocol_ext_container_empty_l;

// MCBearerContextToModifyConfirm ::= SEQUENCE
struct mc_bearer_context_to_modify_confirm_s {
  bool                                                  ext                                           = false;
  bool                                                  mbs_multicast_f1_u_context_descriptor_present = false;
  bool                                                  ie_exts_present                               = false;
  mbs_multicast_f1_u_context_descriptor_s               mbs_multicast_f1_u_context_descriptor;
  mcmrb_modify_confirm_list_l                           mc_mrb_modify_confirm_list;
  mc_bearer_context_to_modify_confirm_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MCBearerContextToModifyRequired ::= SEQUENCE
struct mc_bearer_context_to_modify_required_s {
  bool                                    ext                                           = false;
  bool                                    mbs_multicast_f1_u_context_descriptor_present = false;
  mbs_multicast_f1_u_context_descriptor_s mbs_multicast_f1_u_context_descriptor;
  mcmrb_rem_cfg_l                         mc_mrb_to_rem_required_list;
  mcmrb_modify_required_cfg_l             mc_mrb_to_modify_required_list;
  protocol_ext_container_l<mc_bearer_context_to_modify_required_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MCBearerContextToModifyResponse ::= SEQUENCE
struct mc_bearer_context_to_modify_resp_s {
  bool ext                                                        = false;
  bool mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_present = false;
  bool mbs_multicast_f1_u_context_descriptor_present              = false;
  mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp;
  mbs_multicast_f1_u_context_descriptor_s              mbs_multicast_f1_u_context_descriptor;
  mcmrb_setup_modify_resp_list_l                       mc_mrb_modify_setup_resp_list;
  mcmrb_failed_list_l                                  mc_mrb_failed_list;
  mcmrb_setup_cfg_l                                    available_mcmrb_cfg;
  protocol_ext_container_l<mc_bearer_context_to_modify_resp_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct mc_bearer_context_to_setup_ext_ies_container {
  bool mbs_session_associated_info_non_support_to_support_present = false;
  bool mbs_area_session_id_present                                = false;
  bool mc_bearer_context_inactivity_timer_present                 = false;
  bool mc_bearer_context_status_change_present                    = false;
  mbs_session_associated_info_non_support_to_support_s mbs_session_associated_info_non_support_to_support;
  uint32_t                                             mbs_area_session_id;
  uint16_t                                             mc_bearer_context_inactivity_timer;
  mc_bearer_context_status_change_e                    mc_bearer_context_status_change;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MCBearerContextToSetup ::= SEQUENCE
struct mc_bearer_context_to_setup_s {
  bool                                         ext                      = false;
  bool                                         requested_action_present = false;
  bool                                         ie_exts_present          = false;
  snssai_s                                     snssai;
  mcmrb_setup_cfg_l                            mc_mrb_to_setup_list;
  requested_action4_avail_ngu_termination_e    requested_action;
  mc_bearer_context_to_setup_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mc_bearer_context_to_setup_resp_ext_ies_container = protocol_ext_container_empty_l;

// MCBearerContextToSetupResponse ::= SEQUENCE
struct mc_bearer_context_to_setup_resp_s {
  bool                                              ext                                            = false;
  bool                                              mc_bearer_context_ngu_tnl_infoat_ngran_present = false;
  bool                                              ie_exts_present                                = false;
  mc_bearer_context_ngu_tnl_infoat_ngran_c          mc_bearer_context_ngu_tnl_infoat_ngran;
  mcmrb_setup_resp_list_l                           mc_mrb_setup_resp_list;
  mcmrb_failed_list_l                               mc_mrb_failed_list;
  mcmrb_setup_cfg_l                                 available_mcmrb_cfg;
  mc_bearer_context_to_setup_resp_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDU-Session-Resource-Data-Usage-List ::= SEQUENCE (SIZE (1..256)) OF PDU-Session-Resource-Data-Usage-Item
using pdu_session_res_data_usage_list_l = dyn_array<pdu_session_res_data_usage_item_s>;

// RegistrationRequest ::= ENUMERATED
struct regist_request_opts {
  enum options { start, stop, /*...*/ nulltype } value;

  const char* to_string() const;
};
using regist_request_e = enumerated<regist_request_opts, true>;

// ReportingPeriodicity ::= ENUMERATED
struct report_periodicity_opts {
  enum options {
    ms500,
    ms1000,
    ms2000,
    ms5000,
    ms10000,
    ms20000,
    ms30000,
    ms40000,
    ms50000,
    ms60000,
    ms70000,
    ms80000,
    ms90000,
    ms100000,
    ms110000,
    ms120000,
    // ...
    nulltype
  } value;
  typedef uint32_t number_type;

  const char* to_string() const;
  uint32_t    to_number() const;
};
using report_periodicity_e = enumerated<report_periodicity_opts, true>;

using tnl_available_capacity_ind_ext_ies_container = protocol_ext_container_empty_l;

// TNL-AvailableCapacityIndicator ::= SEQUENCE
struct tnl_available_capacity_ind_s {
  bool                                         ext                       = false;
  bool                                         ie_exts_present           = false;
  uint32_t                                     dl_tnl_offered_capacity   = 0;
  uint8_t                                      dl_tnl_available_capacity = 0;
  uint32_t                                     ul_tnl_offered_capacity   = 0;
  uint8_t                                      ul_tnl_available_capacity = 0;
  tnl_available_capacity_ind_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TimeToWait ::= ENUMERATED
struct time_to_wait_opts {
  enum options { v1s, v2s, v5s, v10s, v20s, v60s, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using time_to_wait_e = enumerated<time_to_wait_opts, true>;

using transport_layer_address_info_ext_ies_container = protocol_ext_container_empty_l;

// Transport-Layer-Address-Info ::= SEQUENCE
struct transport_layer_address_info_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  transport_up_layer_addresses_info_to_add_list_l transport_up_layer_addresses_info_to_add_list;
  transport_up_layer_addresses_info_to_rem_list_l transport_up_layer_addresses_info_to_rem_list;
  transport_layer_address_info_ext_ies_container  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace e1ap
} // namespace asn1

extern template struct asn1::protocol_ext_field_s<asn1::e1ap::dyn_5qi_descriptor_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::non_dyn_5qi_descriptor_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::tsc_assist_info_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::qos_flow_level_qos_params_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::qos_flow_qos_param_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::pdcp_cfg_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::bcmrb_modify_cfg_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::qos_flow_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::up_params_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::data_forwarding_info_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::drb_to_modify_item_ng_ran_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::drb_to_setup_item_ng_ran_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::drb_to_setup_mod_item_ng_ran_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::drb_to_setup_mod_item_eutran_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::pdu_session_res_to_modify_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::pdu_session_res_to_rem_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::pdu_session_res_to_setup_mod_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::drb_modified_item_ng_ran_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::drb_setup_mod_item_eutran_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::drb_to_setup_item_eutran_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::pdu_session_res_to_setup_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::trace_activation_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::drb_setup_item_eutran_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::pdu_session_res_setup_item_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e1ap::cp_tnl_info_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::mc_bearer_context_to_modify_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::e1ap::mc_bearer_context_to_setup_ext_ies_o>;

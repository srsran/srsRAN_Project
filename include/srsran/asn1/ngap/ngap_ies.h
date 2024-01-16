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
 *                     3GPP TS ASN1 NGAP v17.4.0 (2023-03)
 *
 ******************************************************************************/

#pragma once

#include "common.h"

namespace asn1 {
namespace ngap {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// EndpointIPAddressAndPort-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using endpoint_ip_address_and_port_ext_ies_o = protocol_ext_empty_o;

using endpoint_ip_address_and_port_ext_ies_container = protocol_ext_container_empty_l;

// EndpointIPAddressAndPort ::= SEQUENCE
struct endpoint_ip_address_and_port_s {
  bool                                           ie_exts_present = false;
  bounded_bitstring<1, 160, true, true>          endpoint_ip_address;
  fixed_octstring<2, true>                       port_num;
  endpoint_ip_address_and_port_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CPTransportLayerInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct cp_transport_layer_info_ext_ies_o {
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

// AMF-TNLAssociationSetupItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using amf_tnl_assoc_setup_item_ext_ies_o = protocol_ext_empty_o;

// CPTransportLayerInformation ::= CHOICE
struct cp_transport_layer_info_c {
  struct types_opts {
    enum options { endpoint_ip_address, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  cp_transport_layer_info_c() = default;
  cp_transport_layer_info_c(const cp_transport_layer_info_c& other);
  cp_transport_layer_info_c& operator=(const cp_transport_layer_info_c& other);
  ~cp_transport_layer_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  bounded_bitstring<1, 160, true, true>& endpoint_ip_address()
  {
    assert_choice_type(types::endpoint_ip_address, type_, "CPTransportLayerInformation");
    return c.get<bounded_bitstring<1, 160, true, true>>();
  }
  protocol_ie_single_container_s<cp_transport_layer_info_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "CPTransportLayerInformation");
    return c.get<protocol_ie_single_container_s<cp_transport_layer_info_ext_ies_o>>();
  }
  const bounded_bitstring<1, 160, true, true>& endpoint_ip_address() const
  {
    assert_choice_type(types::endpoint_ip_address, type_, "CPTransportLayerInformation");
    return c.get<bounded_bitstring<1, 160, true, true>>();
  }
  const protocol_ie_single_container_s<cp_transport_layer_info_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "CPTransportLayerInformation");
    return c.get<protocol_ie_single_container_s<cp_transport_layer_info_ext_ies_o>>();
  }
  bounded_bitstring<1, 160, true, true>&                             set_endpoint_ip_address();
  protocol_ie_single_container_s<cp_transport_layer_info_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<bounded_bitstring<1, 160, true, true>,
                  protocol_ie_single_container_s<cp_transport_layer_info_ext_ies_o>>
      c;

  void destroy_();
};

using amf_tnl_assoc_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// AMF-TNLAssociationSetupItem ::= SEQUENCE
struct amf_tnl_assoc_setup_item_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  cp_transport_layer_info_c                  amf_tnl_assoc_address;
  amf_tnl_assoc_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AMF-TNLAssociationSetupList ::= SEQUENCE (SIZE (1..32)) OF AMF-TNLAssociationSetupItem
using amf_tnl_assoc_setup_list_l = dyn_array<amf_tnl_assoc_setup_item_s>;

// AMF-TNLAssociationToAddItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using amf_tnl_assoc_to_add_item_ext_ies_o = protocol_ext_empty_o;

// TNLAssociationUsage ::= ENUMERATED
struct tnl_assoc_usage_opts {
  enum options { ue, non_ue, both, /*...*/ nulltype } value;

  const char* to_string() const;
};
using tnl_assoc_usage_e = enumerated<tnl_assoc_usage_opts, true>;

using amf_tnl_assoc_to_add_item_ext_ies_container = protocol_ext_container_empty_l;

// AMF-TNLAssociationToAddItem ::= SEQUENCE
struct amf_tnl_assoc_to_add_item_s {
  bool                                        ext                     = false;
  bool                                        tnl_assoc_usage_present = false;
  bool                                        ie_exts_present         = false;
  cp_transport_layer_info_c                   amf_tnl_assoc_address;
  tnl_assoc_usage_e                           tnl_assoc_usage;
  uint16_t                                    tnla_ddress_weight_factor = 0;
  amf_tnl_assoc_to_add_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AMF-TNLAssociationToAddList ::= SEQUENCE (SIZE (1..32)) OF AMF-TNLAssociationToAddItem
using amf_tnl_assoc_to_add_list_l = dyn_array<amf_tnl_assoc_to_add_item_s>;

// AMF-TNLAssociationToRemoveItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct amf_tnl_assoc_to_rem_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { tnl_assoc_transport_layer_address_ngran, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::tnl_assoc_transport_layer_address_ngran; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cp_transport_layer_info_c&       tnl_assoc_transport_layer_address_ngran() { return c; }
    const cp_transport_layer_info_c& tnl_assoc_transport_layer_address_ngran() const { return c; }

  private:
    cp_transport_layer_info_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// AMF-TNLAssociationToRemoveItem ::= SEQUENCE
struct amf_tnl_assoc_to_rem_item_s {
  bool                                                          ext = false;
  cp_transport_layer_info_c                                     amf_tnl_assoc_address;
  protocol_ext_container_l<amf_tnl_assoc_to_rem_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AMF-TNLAssociationToRemoveList ::= SEQUENCE (SIZE (1..32)) OF AMF-TNLAssociationToRemoveItem
using amf_tnl_assoc_to_rem_list_l = dyn_array<amf_tnl_assoc_to_rem_item_s>;

// AMF-TNLAssociationToUpdateItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using amf_tnl_assoc_to_upd_item_ext_ies_o = protocol_ext_empty_o;

using amf_tnl_assoc_to_upd_item_ext_ies_container = protocol_ext_container_empty_l;

// AMF-TNLAssociationToUpdateItem ::= SEQUENCE
struct amf_tnl_assoc_to_upd_item_s {
  bool                                        ext                               = false;
  bool                                        tnl_assoc_usage_present           = false;
  bool                                        tnla_ddress_weight_factor_present = false;
  bool                                        ie_exts_present                   = false;
  cp_transport_layer_info_c                   amf_tnl_assoc_address;
  tnl_assoc_usage_e                           tnl_assoc_usage;
  uint16_t                                    tnla_ddress_weight_factor = 0;
  amf_tnl_assoc_to_upd_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AMF-TNLAssociationToUpdateList ::= SEQUENCE (SIZE (1..32)) OF AMF-TNLAssociationToUpdateItem
using amf_tnl_assoc_to_upd_list_l = dyn_array<amf_tnl_assoc_to_upd_item_s>;

// S-NSSAI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using s_nssai_ext_ies_o = protocol_ext_empty_o;

// AllowedNSSAI-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using allowed_nssai_item_ext_ies_o = protocol_ext_empty_o;

using s_nssai_ext_ies_container = protocol_ext_container_empty_l;

// S-NSSAI ::= SEQUENCE
struct s_nssai_s {
  bool                      ext             = false;
  bool                      sd_present      = false;
  bool                      ie_exts_present = false;
  fixed_octstring<1, true>  sst;
  fixed_octstring<3, true>  sd;
  s_nssai_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using allowed_nssai_item_ext_ies_container = protocol_ext_container_empty_l;

// AllowedNSSAI-Item ::= SEQUENCE
struct allowed_nssai_item_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  s_nssai_s                            s_nssai;
  allowed_nssai_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AllowedNSSAI ::= SEQUENCE (SIZE (1..8)) OF AllowedNSSAI-Item
using allowed_nssai_l = dyn_array<allowed_nssai_item_s>;

// SliceSupportItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using slice_support_item_ext_ies_o = protocol_ext_empty_o;

// NPN-Support-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using npn_support_ext_ies_o = protocol_ies_empty_o;

using slice_support_item_ext_ies_container = protocol_ext_container_empty_l;

// SliceSupportItem ::= SEQUENCE
struct slice_support_item_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  s_nssai_s                            s_nssai;
  slice_support_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ExtendedSliceSupportList ::= SEQUENCE (SIZE (1..65535)) OF SliceSupportItem
using extended_slice_support_list_l = dyn_array<slice_support_item_s>;

// GUAMI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using guami_ext_ies_o = protocol_ext_empty_o;

// GUAMIType ::= ENUMERATED
struct guami_type_opts {
  enum options { native, mapped, /*...*/ nulltype } value;

  const char* to_string() const;
};
using guami_type_e = enumerated<guami_type_opts, true>;

// NPN-Support ::= CHOICE
struct npn_support_c {
  struct types_opts {
    enum options { sn_pn, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  npn_support_c() = default;
  npn_support_c(const npn_support_c& other);
  npn_support_c& operator=(const npn_support_c& other);
  ~npn_support_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<44, false, true>& sn_pn()
  {
    assert_choice_type(types::sn_pn, type_, "NPN-Support");
    return c.get<fixed_bitstring<44, false, true>>();
  }
  protocol_ie_single_container_s<npn_support_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "NPN-Support");
    return c.get<protocol_ie_single_container_s<npn_support_ext_ies_o>>();
  }
  const fixed_bitstring<44, false, true>& sn_pn() const
  {
    assert_choice_type(types::sn_pn, type_, "NPN-Support");
    return c.get<fixed_bitstring<44, false, true>>();
  }
  const protocol_ie_single_container_s<npn_support_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "NPN-Support");
    return c.get<protocol_ie_single_container_s<npn_support_ext_ies_o>>();
  }
  fixed_bitstring<44, false, true>&                      set_sn_pn();
  protocol_ie_single_container_s<npn_support_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                    type_;
  choice_buffer_t<fixed_bitstring<44, false, true>, protocol_ie_single_container_s<npn_support_ext_ies_o>> c;

  void destroy_();
};

// OnboardingSupport ::= ENUMERATED
struct onboarding_support_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using onboarding_support_e = enumerated<onboarding_support_opts, true>;

using guami_ext_ies_container = protocol_ext_container_empty_l;

// GUAMI ::= SEQUENCE
struct guami_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<8, false, true>  amf_region_id;
  fixed_bitstring<10, false, true> amf_set_id;
  fixed_bitstring<6, false, true>  amf_pointer;
  guami_ext_ies_container          ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMNSupportItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct plmn_support_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { npn_support, extended_slice_support_list, onboarding_support, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    npn_support_c&                       npn_support();
    extended_slice_support_list_l&       extended_slice_support_list();
    onboarding_support_e&                onboarding_support();
    const npn_support_c&                 npn_support() const;
    const extended_slice_support_list_l& extended_slice_support_list() const;
    const onboarding_support_e&          onboarding_support() const;

  private:
    types                                                         type_;
    choice_buffer_t<extended_slice_support_list_l, npn_support_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ServedGUAMIItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct served_guami_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { guami_type, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::guami_type; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    guami_type_e&       guami_type() { return c; }
    const guami_type_e& guami_type() const { return c; }

  private:
    guami_type_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SliceSupportList ::= SEQUENCE (SIZE (1..1024)) OF SliceSupportItem
using slice_support_list_l = dyn_array<slice_support_item_s>;

// Extended-AMFName-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using extended_amf_name_ext_ies_o = protocol_ext_empty_o;

struct plmn_support_item_ext_ies_container {
  bool                          npn_support_present                 = false;
  bool                          extended_slice_support_list_present = false;
  bool                          onboarding_support_present          = false;
  npn_support_c                 npn_support;
  extended_slice_support_list_l extended_slice_support_list;
  onboarding_support_e          onboarding_support;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMNSupportItem ::= SEQUENCE
struct plmn_support_item_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  fixed_octstring<3, true>            plmn_id;
  slice_support_list_l                slice_support_list;
  plmn_support_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServedGUAMIItem ::= SEQUENCE
struct served_guami_item_s {
  bool                                                  ext                     = false;
  bool                                                  backup_amf_name_present = false;
  guami_s                                               guami;
  printable_string<1, 150, true, true>                  backup_amf_name;
  protocol_ext_container_l<served_guami_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using extended_amf_name_ext_ies_container = protocol_ext_container_empty_l;

// Extended-AMFName ::= SEQUENCE
struct extended_amf_name_s {
  bool                                ext                             = false;
  bool                                amf_name_visible_string_present = false;
  bool                                amf_name_utf8_string_present    = false;
  bool                                ie_exts_present                 = false;
  visible_string<1, 150, true, true>  amf_name_visible_string;
  utf8_string<1, 150, true, true>     amf_name_utf8_string;
  extended_amf_name_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMNSupportList ::= SEQUENCE (SIZE (1..12)) OF PLMNSupportItem
using plmn_support_list_l = dyn_array<plmn_support_item_s>;

// ServedGUAMIList ::= SEQUENCE (SIZE (1..256)) OF ServedGUAMIItem
using served_guami_list_l = dyn_array<served_guami_item_s>;

// Cause-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using cause_ext_ies_o = protocol_ies_empty_o;

// CauseMisc ::= ENUMERATED
struct cause_misc_opts {
  enum options {
    ctrl_processing_overload,
    not_enough_user_plane_processing_res,
    hardware_fail,
    om_intervention,
    unknown_plmn_or_sn_pn,
    unspecified,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
using cause_misc_e = enumerated<cause_misc_opts, true>;

// CauseNas ::= ENUMERATED
struct cause_nas_opts {
  enum options {
    normal_release,
    authentication_fail,
    deregister,
    unspecified,
    /*...*/ ue_not_in_plmn_serving_area,
    nulltype
  } value;

  const char* to_string() const;
};
using cause_nas_e = enumerated<cause_nas_opts, true, 1>;

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
    txnrelocoverall_expiry,
    successful_ho,
    release_due_to_ngran_generated_reason,
    release_due_to_5gc_generated_reason,
    ho_cancelled,
    partial_ho,
    ho_fail_in_target_5_gc_ngran_node_or_target_sys,
    ho_target_not_allowed,
    tngrelocoverall_expiry,
    tngrelocprep_expiry,
    cell_not_available,
    unknown_target_id,
    no_radio_res_available_in_target_cell,
    unknown_local_ue_ngap_id,
    inconsistent_remote_ue_ngap_id,
    ho_desirable_for_radio_reason,
    time_crit_ho,
    res_optim_ho,
    reduce_load_in_serving_cell,
    user_inactivity,
    radio_conn_with_ue_lost,
    radio_res_not_available,
    invalid_qos_combination,
    fail_in_radio_interface_proc,
    interaction_with_other_proc,
    unknown_pdu_session_id,
    unkown_qos_flow_id,
    multiple_pdu_session_id_instances,
    multiple_qos_flow_id_instances,
    encryption_and_or_integrity_protection_algorithms_not_supported,
    ng_intra_sys_ho_triggered,
    ng_inter_sys_ho_triggered,
    xn_ho_triggered,
    not_supported_5qi_value,
    ue_context_transfer,
    ims_voice_eps_fallback_or_rat_fallback_triggered,
    up_integrity_protection_not_possible,
    up_confidentiality_protection_not_possible,
    slice_not_supported,
    ue_in_rrc_inactive_state_not_reachable,
    redirection,
    res_not_available_for_the_slice,
    ue_max_integrity_protected_data_rate_reason,
    release_due_to_cn_detected_mob,
    // ...
    n26_interface_not_available,
    release_due_to_pre_emption,
    multiple_location_report_ref_id_instances,
    rsn_not_available_for_the_up,
    npn_access_denied,
    cag_only_access_denied,
    insufficient_ue_cap,
    redcap_ue_not_supported,
    unknown_mbs_session_id,
    indicated_mbs_session_area_info_not_served_by_the_gnb,
    inconsistent_slice_info_for_the_session,
    misaligned_assoc_for_multicast_unicast,
    nulltype
  } value;

  const char* to_string() const;
};
using cause_radio_network_e = enumerated<cause_radio_network_opts, true, 12>;

// CauseTransport ::= ENUMERATED
struct cause_transport_opts {
  enum options { transport_res_unavailable, unspecified, /*...*/ nulltype } value;

  const char* to_string() const;
};
using cause_transport_e = enumerated<cause_transport_opts, true>;

// CriticalityDiagnostics-IE-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using crit_diagnostics_ie_item_ext_ies_o = protocol_ext_empty_o;

// TypeOfError ::= ENUMERATED
struct type_of_error_opts {
  enum options { not_understood, missing, /*...*/ nulltype } value;

  const char* to_string() const;
};
using type_of_error_e = enumerated<type_of_error_opts, true>;

// Cause ::= CHOICE
struct cause_c {
  struct types_opts {
    enum options { radio_network, transport, nas, protocol, misc, choice_exts, nulltype } value;
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
  cause_nas_e& nas()
  {
    assert_choice_type(types::nas, type_, "Cause");
    return c.get<cause_nas_e>();
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
  protocol_ie_single_container_s<cause_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "Cause");
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
  const cause_nas_e& nas() const
  {
    assert_choice_type(types::nas, type_, "Cause");
    return c.get<cause_nas_e>();
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
  const protocol_ie_single_container_s<cause_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "Cause");
    return c.get<protocol_ie_single_container_s<cause_ext_ies_o>>();
  }
  cause_radio_network_e&                           set_radio_network();
  cause_transport_e&                               set_transport();
  cause_nas_e&                                     set_nas();
  cause_protocol_e&                                set_protocol();
  cause_misc_e&                                    set_misc();
  protocol_ie_single_container_s<cause_ext_ies_o>& set_choice_exts();

private:
  types                                                            type_;
  choice_buffer_t<protocol_ie_single_container_s<cause_ext_ies_o>> c;

  void destroy_();
};

using crit_diagnostics_ie_item_ext_ies_container = protocol_ext_container_empty_l;

// CriticalityDiagnostics-IE-Item ::= SEQUENCE
struct crit_diagnostics_ie_item_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  crit_e                                     ie_crit;
  uint32_t                                   ie_id = 0;
  type_of_error_e                            type_of_error;
  crit_diagnostics_ie_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TNLAssociationItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using tnl_assoc_item_ext_ies_o = protocol_ext_empty_o;

// CriticalityDiagnostics-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using crit_diagnostics_ext_ies_o = protocol_ext_empty_o;

// CriticalityDiagnostics-IE-List ::= SEQUENCE (SIZE (1..256)) OF CriticalityDiagnostics-IE-Item
using crit_diagnostics_ie_list_l = dyn_array<crit_diagnostics_ie_item_s>;

using tnl_assoc_item_ext_ies_container = protocol_ext_container_empty_l;

// TNLAssociationItem ::= SEQUENCE
struct tnl_assoc_item_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  cp_transport_layer_info_c        tnl_assoc_address;
  cause_c                          cause;
  tnl_assoc_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TriggeringMessage ::= ENUMERATED
struct trigger_msg_opts {
  enum options { init_msg, successful_outcome, unsuccessful_outcome, nulltype } value;

  const char* to_string() const;
};
using trigger_msg_e = enumerated<trigger_msg_opts>;

using crit_diagnostics_ext_ies_container = protocol_ext_container_empty_l;

// CriticalityDiagnostics ::= SEQUENCE
struct crit_diagnostics_s {
  bool                               ext                 = false;
  bool                               proc_code_present   = false;
  bool                               trigger_msg_present = false;
  bool                               proc_crit_present   = false;
  bool                               ie_exts_present     = false;
  uint16_t                           proc_code           = 0;
  trigger_msg_e                      trigger_msg;
  crit_e                             proc_crit;
  crit_diagnostics_ie_list_l         ies_crit_diagnostics;
  crit_diagnostics_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TNLAssociationList ::= SEQUENCE (SIZE (1..32)) OF TNLAssociationItem
using tnl_assoc_list_l = dyn_array<tnl_assoc_item_s>;

// TimeToWait ::= ENUMERATED
struct time_to_wait_opts {
  enum options { v1s, v2s, v5s, v10s, v20s, v60s, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using time_to_wait_e = enumerated<time_to_wait_opts, true>;

// TNGF-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using tngf_id_ext_ies_o = protocol_ies_empty_o;

// TWIF-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using twif_id_ext_ies_o = protocol_ies_empty_o;

// W-AGF-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using w_agf_id_ext_ies_o = protocol_ies_empty_o;

// GNB-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using gnb_id_ext_ies_o = protocol_ies_empty_o;

// GlobalTNGF-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using global_tngf_id_ext_ies_o = protocol_ext_empty_o;

// GlobalTWIF-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using global_twif_id_ext_ies_o = protocol_ext_empty_o;

// GlobalW-AGF-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using global_w_agf_id_ext_ies_o = protocol_ext_empty_o;

// N3IWF-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using n3_iwf_id_ext_ies_o = protocol_ies_empty_o;

// NgENB-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using ng_enb_id_ext_ies_o = protocol_ies_empty_o;

// TNGF-ID ::= CHOICE
struct tngf_id_c {
  struct types_opts {
    enum options { tngf_id, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  tngf_id_c() = default;
  tngf_id_c(const tngf_id_c& other);
  tngf_id_c& operator=(const tngf_id_c& other);
  ~tngf_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<32, true, true>& tngf_id()
  {
    assert_choice_type(types::tngf_id, type_, "TNGF-ID");
    return c.get<fixed_bitstring<32, true, true>>();
  }
  protocol_ie_single_container_s<tngf_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "TNGF-ID");
    return c.get<protocol_ie_single_container_s<tngf_id_ext_ies_o>>();
  }
  const fixed_bitstring<32, true, true>& tngf_id() const
  {
    assert_choice_type(types::tngf_id, type_, "TNGF-ID");
    return c.get<fixed_bitstring<32, true, true>>();
  }
  const protocol_ie_single_container_s<tngf_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "TNGF-ID");
    return c.get<protocol_ie_single_container_s<tngf_id_ext_ies_o>>();
  }
  fixed_bitstring<32, true, true>&                   set_tngf_id();
  protocol_ie_single_container_s<tngf_id_ext_ies_o>& set_choice_exts();

private:
  types                                                                                               type_;
  choice_buffer_t<fixed_bitstring<32, true, true>, protocol_ie_single_container_s<tngf_id_ext_ies_o>> c;

  void destroy_();
};

// TWIF-ID ::= CHOICE
struct twif_id_c {
  struct types_opts {
    enum options { twif_id, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  twif_id_c() = default;
  twif_id_c(const twif_id_c& other);
  twif_id_c& operator=(const twif_id_c& other);
  ~twif_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<32, true, true>& twif_id()
  {
    assert_choice_type(types::twif_id, type_, "TWIF-ID");
    return c.get<fixed_bitstring<32, true, true>>();
  }
  protocol_ie_single_container_s<twif_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "TWIF-ID");
    return c.get<protocol_ie_single_container_s<twif_id_ext_ies_o>>();
  }
  const fixed_bitstring<32, true, true>& twif_id() const
  {
    assert_choice_type(types::twif_id, type_, "TWIF-ID");
    return c.get<fixed_bitstring<32, true, true>>();
  }
  const protocol_ie_single_container_s<twif_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "TWIF-ID");
    return c.get<protocol_ie_single_container_s<twif_id_ext_ies_o>>();
  }
  fixed_bitstring<32, true, true>&                   set_twif_id();
  protocol_ie_single_container_s<twif_id_ext_ies_o>& set_choice_exts();

private:
  types                                                                                               type_;
  choice_buffer_t<fixed_bitstring<32, true, true>, protocol_ie_single_container_s<twif_id_ext_ies_o>> c;

  void destroy_();
};

// W-AGF-ID ::= CHOICE
struct w_agf_id_c {
  struct types_opts {
    enum options { w_agf_id, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  w_agf_id_c() = default;
  w_agf_id_c(const w_agf_id_c& other);
  w_agf_id_c& operator=(const w_agf_id_c& other);
  ~w_agf_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<16, true, true>& w_agf_id()
  {
    assert_choice_type(types::w_agf_id, type_, "W-AGF-ID");
    return c.get<fixed_bitstring<16, true, true>>();
  }
  protocol_ie_single_container_s<w_agf_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "W-AGF-ID");
    return c.get<protocol_ie_single_container_s<w_agf_id_ext_ies_o>>();
  }
  const fixed_bitstring<16, true, true>& w_agf_id() const
  {
    assert_choice_type(types::w_agf_id, type_, "W-AGF-ID");
    return c.get<fixed_bitstring<16, true, true>>();
  }
  const protocol_ie_single_container_s<w_agf_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "W-AGF-ID");
    return c.get<protocol_ie_single_container_s<w_agf_id_ext_ies_o>>();
  }
  fixed_bitstring<16, true, true>&                    set_w_agf_id();
  protocol_ie_single_container_s<w_agf_id_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                type_;
  choice_buffer_t<fixed_bitstring<16, true, true>, protocol_ie_single_container_s<w_agf_id_ext_ies_o>> c;

  void destroy_();
};

// GNB-ID ::= CHOICE
struct gnb_id_c {
  struct types_opts {
    enum options { gnb_id, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  gnb_id_c() = default;
  gnb_id_c(const gnb_id_c& other);
  gnb_id_c& operator=(const gnb_id_c& other);
  ~gnb_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  bounded_bitstring<22, 32, false, true>& gnb_id()
  {
    assert_choice_type(types::gnb_id, type_, "GNB-ID");
    return c.get<bounded_bitstring<22, 32, false, true>>();
  }
  protocol_ie_single_container_s<gnb_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "GNB-ID");
    return c.get<protocol_ie_single_container_s<gnb_id_ext_ies_o>>();
  }
  const bounded_bitstring<22, 32, false, true>& gnb_id() const
  {
    assert_choice_type(types::gnb_id, type_, "GNB-ID");
    return c.get<bounded_bitstring<22, 32, false, true>>();
  }
  const protocol_ie_single_container_s<gnb_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "GNB-ID");
    return c.get<protocol_ie_single_container_s<gnb_id_ext_ies_o>>();
  }
  bounded_bitstring<22, 32, false, true>&           set_gnb_id();
  protocol_ie_single_container_s<gnb_id_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                     type_;
  choice_buffer_t<bounded_bitstring<22, 32, false, true>, protocol_ie_single_container_s<gnb_id_ext_ies_o>> c;

  void destroy_();
};

// GlobalGNB-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using global_gnb_id_ext_ies_o = protocol_ext_empty_o;

// GlobalN3IWF-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using global_n3_iwf_id_ext_ies_o = protocol_ext_empty_o;

// GlobalNgENB-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using global_ng_enb_id_ext_ies_o = protocol_ext_empty_o;

using global_tngf_id_ext_ies_container = protocol_ext_container_empty_l;

// GlobalTNGF-ID ::= SEQUENCE
struct global_tngf_id_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_octstring<3, true>         plmn_id;
  tngf_id_c                        tngf_id;
  global_tngf_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using global_twif_id_ext_ies_container = protocol_ext_container_empty_l;

// GlobalTWIF-ID ::= SEQUENCE
struct global_twif_id_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_octstring<3, true>         plmn_id;
  twif_id_c                        twif_id;
  global_twif_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using global_w_agf_id_ext_ies_container = protocol_ext_container_empty_l;

// GlobalW-AGF-ID ::= SEQUENCE
struct global_w_agf_id_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  fixed_octstring<3, true>          plmn_id;
  w_agf_id_c                        w_agf_id;
  global_w_agf_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// N3IWF-ID ::= CHOICE
struct n3_iwf_id_c {
  struct types_opts {
    enum options { n3_iwf_id, choice_exts, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  n3_iwf_id_c() = default;
  n3_iwf_id_c(const n3_iwf_id_c& other);
  n3_iwf_id_c& operator=(const n3_iwf_id_c& other);
  ~n3_iwf_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<16, false, true>& n3_iwf_id()
  {
    assert_choice_type(types::n3_iwf_id, type_, "N3IWF-ID");
    return c.get<fixed_bitstring<16, false, true>>();
  }
  protocol_ie_single_container_s<n3_iwf_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "N3IWF-ID");
    return c.get<protocol_ie_single_container_s<n3_iwf_id_ext_ies_o>>();
  }
  const fixed_bitstring<16, false, true>& n3_iwf_id() const
  {
    assert_choice_type(types::n3_iwf_id, type_, "N3IWF-ID");
    return c.get<fixed_bitstring<16, false, true>>();
  }
  const protocol_ie_single_container_s<n3_iwf_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "N3IWF-ID");
    return c.get<protocol_ie_single_container_s<n3_iwf_id_ext_ies_o>>();
  }
  fixed_bitstring<16, false, true>&                    set_n3_iwf_id();
  protocol_ie_single_container_s<n3_iwf_id_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                  type_;
  choice_buffer_t<fixed_bitstring<16, false, true>, protocol_ie_single_container_s<n3_iwf_id_ext_ies_o>> c;

  void destroy_();
};

// NgENB-ID ::= CHOICE
struct ng_enb_id_c {
  struct types_opts {
    enum options { macro_ng_enb_id, short_macro_ng_enb_id, long_macro_ng_enb_id, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ng_enb_id_c() = default;
  ng_enb_id_c(const ng_enb_id_c& other);
  ng_enb_id_c& operator=(const ng_enb_id_c& other);
  ~ng_enb_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<20, false, true>& macro_ng_enb_id()
  {
    assert_choice_type(types::macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<20, false, true>>();
  }
  fixed_bitstring<18, false, true>& short_macro_ng_enb_id()
  {
    assert_choice_type(types::short_macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<18, false, true>>();
  }
  fixed_bitstring<21, false, true>& long_macro_ng_enb_id()
  {
    assert_choice_type(types::long_macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<21, false, true>>();
  }
  protocol_ie_single_container_s<ng_enb_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "NgENB-ID");
    return c.get<protocol_ie_single_container_s<ng_enb_id_ext_ies_o>>();
  }
  const fixed_bitstring<20, false, true>& macro_ng_enb_id() const
  {
    assert_choice_type(types::macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<20, false, true>>();
  }
  const fixed_bitstring<18, false, true>& short_macro_ng_enb_id() const
  {
    assert_choice_type(types::short_macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<18, false, true>>();
  }
  const fixed_bitstring<21, false, true>& long_macro_ng_enb_id() const
  {
    assert_choice_type(types::long_macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<21, false, true>>();
  }
  const protocol_ie_single_container_s<ng_enb_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "NgENB-ID");
    return c.get<protocol_ie_single_container_s<ng_enb_id_ext_ies_o>>();
  }
  fixed_bitstring<20, false, true>&                    set_macro_ng_enb_id();
  fixed_bitstring<18, false, true>&                    set_short_macro_ng_enb_id();
  fixed_bitstring<21, false, true>&                    set_long_macro_ng_enb_id();
  protocol_ie_single_container_s<ng_enb_id_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                  type_;
  choice_buffer_t<fixed_bitstring<21, false, true>, protocol_ie_single_container_s<ng_enb_id_ext_ies_o>> c;

  void destroy_();
};

using global_gnb_id_ext_ies_container = protocol_ext_container_empty_l;

// GlobalGNB-ID ::= SEQUENCE
struct global_gnb_id_s {
  bool                            ext             = false;
  bool                            ie_exts_present = false;
  fixed_octstring<3, true>        plmn_id;
  gnb_id_c                        gnb_id;
  global_gnb_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using global_n3_iwf_id_ext_ies_container = protocol_ext_container_empty_l;

// GlobalN3IWF-ID ::= SEQUENCE
struct global_n3_iwf_id_s {
  bool                               ext             = false;
  bool                               ie_exts_present = false;
  fixed_octstring<3, true>           plmn_id;
  n3_iwf_id_c                        n3_iwf_id;
  global_n3_iwf_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using global_ng_enb_id_ext_ies_container = protocol_ext_container_empty_l;

// GlobalNgENB-ID ::= SEQUENCE
struct global_ng_enb_id_s {
  bool                               ext             = false;
  bool                               ie_exts_present = false;
  fixed_octstring<3, true>           plmn_id;
  ng_enb_id_c                        ng_enb_id;
  global_ng_enb_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalRANNodeID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct global_ran_node_id_ext_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { global_tngf_id, global_twif_id, global_w_agf_id, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    global_tngf_id_s&        global_tngf_id();
    global_twif_id_s&        global_twif_id();
    global_w_agf_id_s&       global_w_agf_id();
    const global_tngf_id_s&  global_tngf_id() const;
    const global_twif_id_s&  global_twif_id() const;
    const global_w_agf_id_s& global_w_agf_id() const;

  private:
    types                                                                  type_;
    choice_buffer_t<global_tngf_id_s, global_twif_id_s, global_w_agf_id_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TAI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using tai_ext_ies_o = protocol_ext_empty_o;

// AMFPagingTarget-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using amf_paging_target_ext_ies_o = protocol_ies_empty_o;

// GlobalRANNodeID ::= CHOICE
struct global_ran_node_id_c {
  struct types_opts {
    enum options { global_gnb_id, global_ng_enb_id, global_n3_iwf_id, choice_exts, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  global_ran_node_id_c() = default;
  global_ran_node_id_c(const global_ran_node_id_c& other);
  global_ran_node_id_c& operator=(const global_ran_node_id_c& other);
  ~global_ran_node_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  global_gnb_id_s& global_gnb_id()
  {
    assert_choice_type(types::global_gnb_id, type_, "GlobalRANNodeID");
    return c.get<global_gnb_id_s>();
  }
  global_ng_enb_id_s& global_ng_enb_id()
  {
    assert_choice_type(types::global_ng_enb_id, type_, "GlobalRANNodeID");
    return c.get<global_ng_enb_id_s>();
  }
  global_n3_iwf_id_s& global_n3_iwf_id()
  {
    assert_choice_type(types::global_n3_iwf_id, type_, "GlobalRANNodeID");
    return c.get<global_n3_iwf_id_s>();
  }
  protocol_ie_single_container_s<global_ran_node_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "GlobalRANNodeID");
    return c.get<protocol_ie_single_container_s<global_ran_node_id_ext_ies_o>>();
  }
  const global_gnb_id_s& global_gnb_id() const
  {
    assert_choice_type(types::global_gnb_id, type_, "GlobalRANNodeID");
    return c.get<global_gnb_id_s>();
  }
  const global_ng_enb_id_s& global_ng_enb_id() const
  {
    assert_choice_type(types::global_ng_enb_id, type_, "GlobalRANNodeID");
    return c.get<global_ng_enb_id_s>();
  }
  const global_n3_iwf_id_s& global_n3_iwf_id() const
  {
    assert_choice_type(types::global_n3_iwf_id, type_, "GlobalRANNodeID");
    return c.get<global_n3_iwf_id_s>();
  }
  const protocol_ie_single_container_s<global_ran_node_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "GlobalRANNodeID");
    return c.get<protocol_ie_single_container_s<global_ran_node_id_ext_ies_o>>();
  }
  global_gnb_id_s&                                              set_global_gnb_id();
  global_ng_enb_id_s&                                           set_global_ng_enb_id();
  global_n3_iwf_id_s&                                           set_global_n3_iwf_id();
  protocol_ie_single_container_s<global_ran_node_id_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<global_gnb_id_s,
                  global_n3_iwf_id_s,
                  global_ng_enb_id_s,
                  protocol_ie_single_container_s<global_ran_node_id_ext_ies_o>>
      c;

  void destroy_();
};

using tai_ext_ies_container = protocol_ext_container_empty_l;

// TAI ::= SEQUENCE
struct tai_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  fixed_octstring<3, true> plmn_id;
  fixed_octstring<3, true> tac;
  tai_ext_ies_container    ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AMFPagingTarget ::= CHOICE
struct amf_paging_target_c {
  struct types_opts {
    enum options { global_ran_node_id, tai, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  amf_paging_target_c() = default;
  amf_paging_target_c(const amf_paging_target_c& other);
  amf_paging_target_c& operator=(const amf_paging_target_c& other);
  ~amf_paging_target_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  global_ran_node_id_c& global_ran_node_id()
  {
    assert_choice_type(types::global_ran_node_id, type_, "AMFPagingTarget");
    return c.get<global_ran_node_id_c>();
  }
  tai_s& tai()
  {
    assert_choice_type(types::tai, type_, "AMFPagingTarget");
    return c.get<tai_s>();
  }
  protocol_ie_single_container_s<amf_paging_target_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "AMFPagingTarget");
    return c.get<protocol_ie_single_container_s<amf_paging_target_ext_ies_o>>();
  }
  const global_ran_node_id_c& global_ran_node_id() const
  {
    assert_choice_type(types::global_ran_node_id, type_, "AMFPagingTarget");
    return c.get<global_ran_node_id_c>();
  }
  const tai_s& tai() const
  {
    assert_choice_type(types::tai, type_, "AMFPagingTarget");
    return c.get<tai_s>();
  }
  const protocol_ie_single_container_s<amf_paging_target_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "AMFPagingTarget");
    return c.get<protocol_ie_single_container_s<amf_paging_target_ext_ies_o>>();
  }
  global_ran_node_id_c&                                        set_global_ran_node_id();
  tai_s&                                                       set_tai();
  protocol_ie_single_container_s<amf_paging_target_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                     type_;
  choice_buffer_t<global_ran_node_id_c, protocol_ie_single_container_s<amf_paging_target_ext_ies_o>, tai_s> c;

  void destroy_();
};

// TimerApproachForGUAMIRemoval ::= ENUMERATED
struct timer_approach_for_guami_removal_opts {
  enum options { apply_timer, /*...*/ nulltype } value;

  const char* to_string() const;
};
using timer_approach_for_guami_removal_e = enumerated<timer_approach_for_guami_removal_opts, true>;

// UnavailableGUAMIItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using unavailable_guami_item_ext_ies_o = protocol_ext_empty_o;

using unavailable_guami_item_ext_ies_container = protocol_ext_container_empty_l;

// UnavailableGUAMIItem ::= SEQUENCE
struct unavailable_guami_item_s {
  bool                                     ext                                      = false;
  bool                                     timer_approach_for_guami_removal_present = false;
  bool                                     backup_amf_name_present                  = false;
  bool                                     ie_exts_present                          = false;
  guami_s                                  guami;
  timer_approach_for_guami_removal_e       timer_approach_for_guami_removal;
  printable_string<1, 150, true, true>     backup_amf_name;
  unavailable_guami_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UnavailableGUAMIList ::= SEQUENCE (SIZE (1..256)) OF UnavailableGUAMIItem
using unavailable_guami_list_l = dyn_array<unavailable_guami_item_s>;

// EUTRA-CGI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using eutra_cgi_ext_ies_o = protocol_ext_empty_o;

// NR-CGI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using nr_cgi_ext_ies_o = protocol_ext_empty_o;

using eutra_cgi_ext_ies_container = protocol_ext_container_empty_l;

// EUTRA-CGI ::= SEQUENCE
struct eutra_cgi_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<28, false, true> eutra_cell_id;
  eutra_cgi_ext_ies_container      ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGRAN-CGI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using ngran_cgi_ext_ies_o = protocol_ies_empty_o;

using nr_cgi_ext_ies_container = protocol_ext_container_empty_l;

// NR-CGI ::= SEQUENCE
struct nr_cgi_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<36, false, true> nr_cell_id;
  nr_cgi_ext_ies_container         ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGRAN-CGI ::= CHOICE
struct ngran_cgi_c {
  struct types_opts {
    enum options { nr_cgi, eutra_cgi, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ngran_cgi_c() = default;
  ngran_cgi_c(const ngran_cgi_c& other);
  ngran_cgi_c& operator=(const ngran_cgi_c& other);
  ~ngran_cgi_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  nr_cgi_s& nr_cgi()
  {
    assert_choice_type(types::nr_cgi, type_, "NGRAN-CGI");
    return c.get<nr_cgi_s>();
  }
  eutra_cgi_s& eutra_cgi()
  {
    assert_choice_type(types::eutra_cgi, type_, "NGRAN-CGI");
    return c.get<eutra_cgi_s>();
  }
  protocol_ie_single_container_s<ngran_cgi_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "NGRAN-CGI");
    return c.get<protocol_ie_single_container_s<ngran_cgi_ext_ies_o>>();
  }
  const nr_cgi_s& nr_cgi() const
  {
    assert_choice_type(types::nr_cgi, type_, "NGRAN-CGI");
    return c.get<nr_cgi_s>();
  }
  const eutra_cgi_s& eutra_cgi() const
  {
    assert_choice_type(types::eutra_cgi, type_, "NGRAN-CGI");
    return c.get<eutra_cgi_s>();
  }
  const protocol_ie_single_container_s<ngran_cgi_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "NGRAN-CGI");
    return c.get<protocol_ie_single_container_s<ngran_cgi_ext_ies_o>>();
  }
  nr_cgi_s&                                            set_nr_cgi();
  eutra_cgi_s&                                         set_eutra_cgi();
  protocol_ie_single_container_s<ngran_cgi_ext_ies_o>& set_choice_exts();

private:
  types                                                                                       type_;
  choice_buffer_t<eutra_cgi_s, nr_cgi_s, protocol_ie_single_container_s<ngran_cgi_ext_ies_o>> c;

  void destroy_();
};

// ActivatedCellList ::= SEQUENCE (SIZE (1..16384)) OF NGRAN-CGI
using activ_cell_list_l = dyn_array<ngran_cgi_c>;

// GTPTunnel-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using gtp_tunnel_ext_ies_o = protocol_ext_empty_o;

// DataForwardingAccepted ::= ENUMERATED
struct data_forwarding_accepted_opts {
  enum options { data_forwarding_accepted, /*...*/ nulltype } value;

  const char* to_string() const;
};
using data_forwarding_accepted_e = enumerated<data_forwarding_accepted_opts, true>;

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

// QosFlowItemWithDataForwarding-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct qos_flow_item_with_data_forwarding_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { current_qos_para_set_idx, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::current_qos_para_set_idx; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&       current_qos_para_set_idx() { return c; }
    const uint8_t& current_qos_para_set_idx() const { return c; }

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

// UPTransportLayerInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using up_transport_layer_info_ext_ies_o = protocol_ies_empty_o;

// QosFlowItemWithDataForwarding ::= SEQUENCE
struct qos_flow_item_with_data_forwarding_s {
  bool                                                                   ext                              = false;
  bool                                                                   data_forwarding_accepted_present = false;
  uint8_t                                                                qos_flow_id                      = 0;
  data_forwarding_accepted_e                                             data_forwarding_accepted;
  protocol_ext_container_l<qos_flow_item_with_data_forwarding_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UPTransportLayerInformation ::= CHOICE
struct up_transport_layer_info_c {
  struct types_opts {
    enum options { gtp_tunnel, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  up_transport_layer_info_c() = default;
  up_transport_layer_info_c(const up_transport_layer_info_c& other);
  up_transport_layer_info_c& operator=(const up_transport_layer_info_c& other);
  ~up_transport_layer_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  gtp_tunnel_s& gtp_tunnel()
  {
    assert_choice_type(types::gtp_tunnel, type_, "UPTransportLayerInformation");
    return c.get<gtp_tunnel_s>();
  }
  protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "UPTransportLayerInformation");
    return c.get<protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o>>();
  }
  const gtp_tunnel_s& gtp_tunnel() const
  {
    assert_choice_type(types::gtp_tunnel, type_, "UPTransportLayerInformation");
    return c.get<gtp_tunnel_s>();
  }
  const protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "UPTransportLayerInformation");
    return c.get<protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o>>();
  }
  gtp_tunnel_s&                                                      set_gtp_tunnel();
  protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o>& set_choice_exts();

private:
  types                                                                                            type_;
  choice_buffer_t<gtp_tunnel_s, protocol_ie_single_container_s<up_transport_layer_info_ext_ies_o>> c;

  void destroy_();
};

// AdditionalDLUPTNLInformationForHOItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct add_dl_up_tnl_info_for_ho_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { add_redundant_dl_ngu_up_tnl_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::add_redundant_dl_ngu_up_tnl_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    up_transport_layer_info_c&       add_redundant_dl_ngu_up_tnl_info() { return c; }
    const up_transport_layer_info_c& add_redundant_dl_ngu_up_tnl_info() const { return c; }

  private:
    up_transport_layer_info_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// QosFlowListWithDataForwarding ::= SEQUENCE (SIZE (1..64)) OF QosFlowItemWithDataForwarding
using qos_flow_list_with_data_forwarding_l = dyn_array<qos_flow_item_with_data_forwarding_s>;

// AdditionalDLUPTNLInformationForHOItem ::= SEQUENCE
struct add_dl_up_tnl_info_for_ho_item_s {
  bool                                                               ext                                   = false;
  bool                                                               add_dl_forwarding_up_tnl_info_present = false;
  up_transport_layer_info_c                                          add_dl_ngu_up_tnl_info;
  qos_flow_list_with_data_forwarding_l                               add_qos_flow_setup_resp_list;
  up_transport_layer_info_c                                          add_dl_forwarding_up_tnl_info;
  protocol_ext_container_l<add_dl_up_tnl_info_for_ho_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AdditionalDLUPTNLInformationForHOList ::= SEQUENCE (SIZE (1..3)) OF AdditionalDLUPTNLInformationForHOItem
using add_dl_up_tnl_info_for_ho_list_l = dyn_array<add_dl_up_tnl_info_for_ho_item_s>;

// AllocationAndRetentionPriority-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using alloc_and_retention_prio_ext_ies_o = protocol_ext_empty_o;

// Pre-emptionCapability ::= ENUMERATED
struct pre_emption_cap_opts {
  enum options { shall_not_trigger_pre_emption, may_trigger_pre_emption, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pre_emption_cap_e = enumerated<pre_emption_cap_opts, true>;

// Pre-emptionVulnerability ::= ENUMERATED
struct pre_emption_vulnerability_opts {
  enum options { not_pre_emptable, pre_emptable, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pre_emption_vulnerability_e = enumerated<pre_emption_vulnerability_opts, true>;

using alloc_and_retention_prio_ext_ies_container = protocol_ext_container_empty_l;

// AllocationAndRetentionPriority ::= SEQUENCE
struct alloc_and_retention_prio_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  uint8_t                                    prio_level_arp  = 1;
  pre_emption_cap_e                          pre_emption_cap;
  pre_emption_vulnerability_e                pre_emption_vulnerability;
  alloc_and_retention_prio_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Allowed-CAG-List-per-PLMN ::= SEQUENCE (SIZE (1..256)) OF BIT STRING (SIZE (32))
using allowed_cag_list_per_plmn_l = dyn_array<fixed_bitstring<32, false, true>>;

// Allowed-PNI-NPN-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using allowed_pni_npn_item_ext_ies_o = protocol_ext_empty_o;

using allowed_pni_npn_item_ext_ies_container = protocol_ext_container_empty_l;

// Allowed-PNI-NPN-Item ::= SEQUENCE
struct allowed_pni_npn_item_s {
  struct pni_npn_restricted_opts {
    enum options { restricted, not_restricted, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using pni_npn_restricted_e_ = enumerated<pni_npn_restricted_opts, true>;

  // member variables
  bool                                   ext             = false;
  bool                                   ie_exts_present = false;
  fixed_octstring<3, true>               plmn_id;
  pni_npn_restricted_e_                  pni_npn_restricted;
  allowed_cag_list_per_plmn_l            allowed_cag_list_per_plmn;
  allowed_pni_npn_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Allowed-PNI-NPN-List ::= SEQUENCE (SIZE (1..16)) OF Allowed-PNI-NPN-Item
using allowed_pni_npn_list_l = dyn_array<allowed_pni_npn_item_s>;

// AllowedTACs ::= SEQUENCE (SIZE (1..16)) OF OCTET STRING (SIZE (3))
using allowed_tacs_l = bounded_array<fixed_octstring<3, true>, 16>;

// PacketErrorRate-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using packet_error_rate_ext_ies_o = protocol_ext_empty_o;

// AlternativeQoSParaSetItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using alt_qos_para_set_item_ext_ies_o = protocol_ext_empty_o;

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

using alt_qos_para_set_item_ext_ies_container = protocol_ext_container_empty_l;

// AlternativeQoSParaSetItem ::= SEQUENCE
struct alt_qos_para_set_item_s {
  bool                                    ext                                 = false;
  bool                                    guaranteed_flow_bit_rate_dl_present = false;
  bool                                    guaranteed_flow_bit_rate_ul_present = false;
  bool                                    packet_delay_budget_present         = false;
  bool                                    packet_error_rate_present           = false;
  bool                                    ie_exts_present                     = false;
  uint8_t                                 alt_qos_para_set_idx                = 1;
  uint64_t                                guaranteed_flow_bit_rate_dl         = 0;
  uint64_t                                guaranteed_flow_bit_rate_ul         = 0;
  uint16_t                                packet_delay_budget                 = 0;
  packet_error_rate_s                     packet_error_rate;
  alt_qos_para_set_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AlternativeQoSParaSetList ::= SEQUENCE (SIZE (1..8)) OF AlternativeQoSParaSetItem
using alt_qos_para_set_list_l = dyn_array<alt_qos_para_set_item_s>;

// AreaOfInterestCellItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using area_of_interest_cell_item_ext_ies_o = protocol_ext_empty_o;

// AreaOfInterestRANNodeItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using area_of_interest_ran_node_item_ext_ies_o = protocol_ext_empty_o;

// AreaOfInterestTAIItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using area_of_interest_tai_item_ext_ies_o = protocol_ext_empty_o;

using area_of_interest_cell_item_ext_ies_container = protocol_ext_container_empty_l;

// AreaOfInterestCellItem ::= SEQUENCE
struct area_of_interest_cell_item_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  ngran_cgi_c                                  ngran_cgi;
  area_of_interest_cell_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using area_of_interest_ran_node_item_ext_ies_container = protocol_ext_container_empty_l;

// AreaOfInterestRANNodeItem ::= SEQUENCE
struct area_of_interest_ran_node_item_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  global_ran_node_id_c                             global_ran_node_id;
  area_of_interest_ran_node_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using area_of_interest_tai_item_ext_ies_container = protocol_ext_container_empty_l;

// AreaOfInterestTAIItem ::= SEQUENCE
struct area_of_interest_tai_item_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  tai_s                                       tai;
  area_of_interest_tai_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaOfInterest-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using area_of_interest_ext_ies_o = protocol_ext_empty_o;

// AreaOfInterestCellList ::= SEQUENCE (SIZE (1..256)) OF AreaOfInterestCellItem
using area_of_interest_cell_list_l = dyn_array<area_of_interest_cell_item_s>;

// AreaOfInterestRANNodeList ::= SEQUENCE (SIZE (1..64)) OF AreaOfInterestRANNodeItem
using area_of_interest_ran_node_list_l = dyn_array<area_of_interest_ran_node_item_s>;

// AreaOfInterestTAIList ::= SEQUENCE (SIZE (1..16)) OF AreaOfInterestTAIItem
using area_of_interest_tai_list_l = dyn_array<area_of_interest_tai_item_s>;

using area_of_interest_ext_ies_container = protocol_ext_container_empty_l;

// AreaOfInterest ::= SEQUENCE
struct area_of_interest_s {
  bool                               ext             = false;
  bool                               ie_exts_present = false;
  area_of_interest_tai_list_l        area_of_interest_tai_list;
  area_of_interest_cell_list_l       area_of_interest_cell_list;
  area_of_interest_ran_node_list_l   area_of_interest_ran_node_list;
  area_of_interest_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaOfInterestItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using area_of_interest_item_ext_ies_o = protocol_ext_empty_o;

using area_of_interest_item_ext_ies_container = protocol_ext_container_empty_l;

// AreaOfInterestItem ::= SEQUENCE
struct area_of_interest_item_s {
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  area_of_interest_s                      area_of_interest;
  uint8_t                                 location_report_ref_id = 1;
  area_of_interest_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaOfInterestList ::= SEQUENCE (SIZE (1..64)) OF AreaOfInterestItem
using area_of_interest_list_l = dyn_array<area_of_interest_item_s>;

// CellBasedMDT-EUTRA-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using cell_based_mdt_eutra_ext_ies_o = protocol_ext_empty_o;

// CellIdListforMDT-EUTRA ::= SEQUENCE (SIZE (1..32)) OF EUTRA-CGI
using cell_id_listfor_mdt_eutra_l = dyn_array<eutra_cgi_s>;

// TABasedMDT-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ta_based_mdt_ext_ies_o = protocol_ext_empty_o;

// TAIBasedMDT-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using tai_based_mdt_ext_ies_o = protocol_ext_empty_o;

// TAIListforMDT ::= SEQUENCE (SIZE (1..8)) OF TAI
using tai_listfor_mdt_l = dyn_array<tai_s>;

// TAListforMDT ::= SEQUENCE (SIZE (1..8)) OF OCTET STRING (SIZE (3))
using ta_listfor_mdt_l = bounded_array<fixed_octstring<3, true>, 8>;

// AreaScopeOfMDT-EUTRA-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using area_scope_of_mdt_eutra_ext_ies_o = protocol_ies_empty_o;

using cell_based_mdt_eutra_ext_ies_container = protocol_ext_container_empty_l;

// CellBasedMDT-EUTRA ::= SEQUENCE
struct cell_based_mdt_eutra_s {
  bool                                   ext             = false;
  bool                                   ie_exts_present = false;
  cell_id_listfor_mdt_eutra_l            cell_id_listfor_mdt;
  cell_based_mdt_eutra_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ta_based_mdt_ext_ies_container = protocol_ext_container_empty_l;

// TABasedMDT ::= SEQUENCE
struct ta_based_mdt_s {
  bool                           ext             = false;
  bool                           ie_exts_present = false;
  ta_listfor_mdt_l               ta_listfor_mdt;
  ta_based_mdt_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using tai_based_mdt_ext_ies_container = protocol_ext_container_empty_l;

// TAIBasedMDT ::= SEQUENCE
struct tai_based_mdt_s {
  bool                            ext             = false;
  bool                            ie_exts_present = false;
  tai_listfor_mdt_l               tai_listfor_mdt;
  tai_based_mdt_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaScopeOfMDT-EUTRA ::= CHOICE
struct area_scope_of_mdt_eutra_c {
  struct types_opts {
    enum options { cell_based, ta_based, plmn_wide, tai_based, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  area_scope_of_mdt_eutra_c() = default;
  area_scope_of_mdt_eutra_c(const area_scope_of_mdt_eutra_c& other);
  area_scope_of_mdt_eutra_c& operator=(const area_scope_of_mdt_eutra_c& other);
  ~area_scope_of_mdt_eutra_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cell_based_mdt_eutra_s& cell_based()
  {
    assert_choice_type(types::cell_based, type_, "AreaScopeOfMDT-EUTRA");
    return c.get<cell_based_mdt_eutra_s>();
  }
  ta_based_mdt_s& ta_based()
  {
    assert_choice_type(types::ta_based, type_, "AreaScopeOfMDT-EUTRA");
    return c.get<ta_based_mdt_s>();
  }
  tai_based_mdt_s& tai_based()
  {
    assert_choice_type(types::tai_based, type_, "AreaScopeOfMDT-EUTRA");
    return c.get<tai_based_mdt_s>();
  }
  protocol_ie_single_container_s<area_scope_of_mdt_eutra_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "AreaScopeOfMDT-EUTRA");
    return c.get<protocol_ie_single_container_s<area_scope_of_mdt_eutra_ext_ies_o>>();
  }
  const cell_based_mdt_eutra_s& cell_based() const
  {
    assert_choice_type(types::cell_based, type_, "AreaScopeOfMDT-EUTRA");
    return c.get<cell_based_mdt_eutra_s>();
  }
  const ta_based_mdt_s& ta_based() const
  {
    assert_choice_type(types::ta_based, type_, "AreaScopeOfMDT-EUTRA");
    return c.get<ta_based_mdt_s>();
  }
  const tai_based_mdt_s& tai_based() const
  {
    assert_choice_type(types::tai_based, type_, "AreaScopeOfMDT-EUTRA");
    return c.get<tai_based_mdt_s>();
  }
  const protocol_ie_single_container_s<area_scope_of_mdt_eutra_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "AreaScopeOfMDT-EUTRA");
    return c.get<protocol_ie_single_container_s<area_scope_of_mdt_eutra_ext_ies_o>>();
  }
  cell_based_mdt_eutra_s&                                            set_cell_based();
  ta_based_mdt_s&                                                    set_ta_based();
  void                                                               set_plmn_wide();
  tai_based_mdt_s&                                                   set_tai_based();
  protocol_ie_single_container_s<area_scope_of_mdt_eutra_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<cell_based_mdt_eutra_s,
                  protocol_ie_single_container_s<area_scope_of_mdt_eutra_ext_ies_o>,
                  ta_based_mdt_s,
                  tai_based_mdt_s>
      c;

  void destroy_();
};

// CellBasedMDT-NR-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using cell_based_mdt_nr_ext_ies_o = protocol_ext_empty_o;

// CellIdListforMDT-NR ::= SEQUENCE (SIZE (1..32)) OF NR-CGI
using cell_id_listfor_mdt_nr_l = dyn_array<nr_cgi_s>;

// AreaScopeOfMDT-NR-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using area_scope_of_mdt_nr_ext_ies_o = protocol_ies_empty_o;

using cell_based_mdt_nr_ext_ies_container = protocol_ext_container_empty_l;

// CellBasedMDT-NR ::= SEQUENCE
struct cell_based_mdt_nr_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  cell_id_listfor_mdt_nr_l            cell_id_listfor_mdt;
  cell_based_mdt_nr_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaScopeOfMDT-NR ::= CHOICE
struct area_scope_of_mdt_nr_c {
  struct types_opts {
    enum options { cell_based, ta_based, plmn_wide, tai_based, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  area_scope_of_mdt_nr_c() = default;
  area_scope_of_mdt_nr_c(const area_scope_of_mdt_nr_c& other);
  area_scope_of_mdt_nr_c& operator=(const area_scope_of_mdt_nr_c& other);
  ~area_scope_of_mdt_nr_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cell_based_mdt_nr_s& cell_based()
  {
    assert_choice_type(types::cell_based, type_, "AreaScopeOfMDT-NR");
    return c.get<cell_based_mdt_nr_s>();
  }
  ta_based_mdt_s& ta_based()
  {
    assert_choice_type(types::ta_based, type_, "AreaScopeOfMDT-NR");
    return c.get<ta_based_mdt_s>();
  }
  tai_based_mdt_s& tai_based()
  {
    assert_choice_type(types::tai_based, type_, "AreaScopeOfMDT-NR");
    return c.get<tai_based_mdt_s>();
  }
  protocol_ie_single_container_s<area_scope_of_mdt_nr_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "AreaScopeOfMDT-NR");
    return c.get<protocol_ie_single_container_s<area_scope_of_mdt_nr_ext_ies_o>>();
  }
  const cell_based_mdt_nr_s& cell_based() const
  {
    assert_choice_type(types::cell_based, type_, "AreaScopeOfMDT-NR");
    return c.get<cell_based_mdt_nr_s>();
  }
  const ta_based_mdt_s& ta_based() const
  {
    assert_choice_type(types::ta_based, type_, "AreaScopeOfMDT-NR");
    return c.get<ta_based_mdt_s>();
  }
  const tai_based_mdt_s& tai_based() const
  {
    assert_choice_type(types::tai_based, type_, "AreaScopeOfMDT-NR");
    return c.get<tai_based_mdt_s>();
  }
  const protocol_ie_single_container_s<area_scope_of_mdt_nr_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "AreaScopeOfMDT-NR");
    return c.get<protocol_ie_single_container_s<area_scope_of_mdt_nr_ext_ies_o>>();
  }
  cell_based_mdt_nr_s&                                            set_cell_based();
  ta_based_mdt_s&                                                 set_ta_based();
  void                                                            set_plmn_wide();
  tai_based_mdt_s&                                                set_tai_based();
  protocol_ie_single_container_s<area_scope_of_mdt_nr_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<cell_based_mdt_nr_s,
                  protocol_ie_single_container_s<area_scope_of_mdt_nr_ext_ies_o>,
                  ta_based_mdt_s,
                  tai_based_mdt_s>
      c;

  void destroy_();
};

// NRFrequencyBandItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using nr_freq_band_item_ext_ies_o = protocol_ext_empty_o;

using nr_freq_band_item_ext_ies_container = protocol_ext_container_empty_l;

// NRFrequencyBandItem ::= SEQUENCE
struct nr_freq_band_item_s {
  bool                                ext            = false;
  bool                                ie_ext_present = false;
  uint16_t                            nr_freq_band   = 1;
  nr_freq_band_item_ext_ies_container ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NRFrequencyBand-List ::= SEQUENCE (SIZE (1..32)) OF NRFrequencyBandItem
using nr_freq_band_list_l = dyn_array<nr_freq_band_item_s>;

// NRFrequencyInfo-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using nr_freq_info_ext_ies_o = protocol_ext_empty_o;

// AreaScopeOfNeighCellsItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using area_scope_of_neigh_cells_item_ext_ies_o = protocol_ext_empty_o;

using nr_freq_info_ext_ies_container = protocol_ext_container_empty_l;

// NRFrequencyInfo ::= SEQUENCE
struct nr_freq_info_s {
  bool                           ext            = false;
  bool                           ie_ext_present = false;
  uint32_t                       nr_arfcn       = 0;
  nr_freq_band_list_l            freq_band_list;
  nr_freq_info_ext_ies_container ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCIListForMDT ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..1007,...)
using pci_list_for_mdt_l = bounded_array<uint16_t, 32>;

using area_scope_of_neigh_cells_item_ext_ies_container = protocol_ext_container_empty_l;

// AreaScopeOfNeighCellsItem ::= SEQUENCE
struct area_scope_of_neigh_cells_item_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  nr_freq_info_s                                   nr_freq_info;
  pci_list_for_mdt_l                               pci_list_for_mdt;
  area_scope_of_neigh_cells_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaScopeOfNeighCellsList ::= SEQUENCE (SIZE (1..8)) OF AreaScopeOfNeighCellsItem
using area_scope_of_neigh_cells_list_l = dyn_array<area_scope_of_neigh_cells_item_s>;

// CellBasedQMC-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using cell_based_qmc_ext_ies_o = protocol_ext_empty_o;

// CellIdListforQMC ::= SEQUENCE (SIZE (1..32)) OF NGRAN-CGI
using cell_id_listfor_qmc_l = dyn_array<ngran_cgi_c>;

// PLMNAreaBasedQMC-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using plmn_area_based_qmc_ext_ies_o = protocol_ext_empty_o;

// PLMNListforQMC ::= SEQUENCE (SIZE (1..16)) OF OCTET STRING (SIZE (3))
using plmn_listfor_qmc_l = bounded_array<fixed_octstring<3, true>, 16>;

// TABasedQMC-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ta_based_qmc_ext_ies_o = protocol_ext_empty_o;

// TAIBasedQMC-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using tai_based_qmc_ext_ies_o = protocol_ext_empty_o;

// TAIListforQMC ::= SEQUENCE (SIZE (1..8)) OF TAI
using tai_listfor_qmc_l = dyn_array<tai_s>;

// TAListforQMC ::= SEQUENCE (SIZE (1..8)) OF OCTET STRING (SIZE (3))
using ta_listfor_qmc_l = bounded_array<fixed_octstring<3, true>, 8>;

// AreaScopeOfQMC-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using area_scope_of_qmc_ext_ies_o = protocol_ies_empty_o;

using cell_based_qmc_ext_ies_container = protocol_ext_container_empty_l;

// CellBasedQMC ::= SEQUENCE
struct cell_based_qmc_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  cell_id_listfor_qmc_l            cell_id_listfor_qmc;
  cell_based_qmc_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using plmn_area_based_qmc_ext_ies_container = protocol_ext_container_empty_l;

// PLMNAreaBasedQMC ::= SEQUENCE
struct plmn_area_based_qmc_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  plmn_listfor_qmc_l                    plmn_listfor_qmc;
  plmn_area_based_qmc_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ta_based_qmc_ext_ies_container = protocol_ext_container_empty_l;

// TABasedQMC ::= SEQUENCE
struct ta_based_qmc_s {
  bool                           ext             = false;
  bool                           ie_exts_present = false;
  ta_listfor_qmc_l               ta_listfor_qmc;
  ta_based_qmc_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using tai_based_qmc_ext_ies_container = protocol_ext_container_empty_l;

// TAIBasedQMC ::= SEQUENCE
struct tai_based_qmc_s {
  bool                            ext             = false;
  bool                            ie_exts_present = false;
  tai_listfor_qmc_l               tai_listfor_qmc;
  tai_based_qmc_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AreaScopeOfQMC ::= CHOICE
struct area_scope_of_qmc_c {
  struct types_opts {
    enum options { cell_based, ta_based, tai_based, plmn_area_based, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  area_scope_of_qmc_c() = default;
  area_scope_of_qmc_c(const area_scope_of_qmc_c& other);
  area_scope_of_qmc_c& operator=(const area_scope_of_qmc_c& other);
  ~area_scope_of_qmc_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cell_based_qmc_s& cell_based()
  {
    assert_choice_type(types::cell_based, type_, "AreaScopeOfQMC");
    return c.get<cell_based_qmc_s>();
  }
  ta_based_qmc_s& ta_based()
  {
    assert_choice_type(types::ta_based, type_, "AreaScopeOfQMC");
    return c.get<ta_based_qmc_s>();
  }
  tai_based_qmc_s& tai_based()
  {
    assert_choice_type(types::tai_based, type_, "AreaScopeOfQMC");
    return c.get<tai_based_qmc_s>();
  }
  plmn_area_based_qmc_s& plmn_area_based()
  {
    assert_choice_type(types::plmn_area_based, type_, "AreaScopeOfQMC");
    return c.get<plmn_area_based_qmc_s>();
  }
  protocol_ie_single_container_s<area_scope_of_qmc_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "AreaScopeOfQMC");
    return c.get<protocol_ie_single_container_s<area_scope_of_qmc_ext_ies_o>>();
  }
  const cell_based_qmc_s& cell_based() const
  {
    assert_choice_type(types::cell_based, type_, "AreaScopeOfQMC");
    return c.get<cell_based_qmc_s>();
  }
  const ta_based_qmc_s& ta_based() const
  {
    assert_choice_type(types::ta_based, type_, "AreaScopeOfQMC");
    return c.get<ta_based_qmc_s>();
  }
  const tai_based_qmc_s& tai_based() const
  {
    assert_choice_type(types::tai_based, type_, "AreaScopeOfQMC");
    return c.get<tai_based_qmc_s>();
  }
  const plmn_area_based_qmc_s& plmn_area_based() const
  {
    assert_choice_type(types::plmn_area_based, type_, "AreaScopeOfQMC");
    return c.get<plmn_area_based_qmc_s>();
  }
  const protocol_ie_single_container_s<area_scope_of_qmc_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "AreaScopeOfQMC");
    return c.get<protocol_ie_single_container_s<area_scope_of_qmc_ext_ies_o>>();
  }
  cell_based_qmc_s&                                            set_cell_based();
  ta_based_qmc_s&                                              set_ta_based();
  tai_based_qmc_s&                                             set_tai_based();
  plmn_area_based_qmc_s&                                       set_plmn_area_based();
  protocol_ie_single_container_s<area_scope_of_qmc_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<cell_based_qmc_s,
                  plmn_area_based_qmc_s,
                  protocol_ie_single_container_s<area_scope_of_qmc_ext_ies_o>,
                  ta_based_qmc_s,
                  tai_based_qmc_s>
      c;

  void destroy_();
};

// RecommendedCellItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using recommended_cell_item_ext_ies_o = protocol_ext_empty_o;

using recommended_cell_item_ext_ies_container = protocol_ext_container_empty_l;

// RecommendedCellItem ::= SEQUENCE
struct recommended_cell_item_s {
  bool                                    ext                         = false;
  bool                                    time_stayed_in_cell_present = false;
  bool                                    ie_exts_present             = false;
  ngran_cgi_c                             ngran_cgi;
  uint16_t                                time_stayed_in_cell = 0;
  recommended_cell_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPN-PagingAssistanceInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using npn_paging_assist_info_ext_ies_o = protocol_ies_empty_o;

// PagingAssisDataforCEcapabUE-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using paging_assis_datafor_c_ecapab_ue_ext_ies_o = protocol_ext_empty_o;

// RecommendedCellList ::= SEQUENCE (SIZE (1..16)) OF RecommendedCellItem
using recommended_cell_list_l = dyn_array<recommended_cell_item_s>;

// RecommendedCellsForPaging-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using recommended_cells_for_paging_ext_ies_o = protocol_ext_empty_o;

// AssistanceDataForRecommendedCells-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using assist_data_for_recommended_cells_ext_ies_o = protocol_ext_empty_o;

// NPN-PagingAssistanceInformation ::= CHOICE
struct npn_paging_assist_info_c {
  struct types_opts {
    enum options { pni_npn_paging_assist, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  npn_paging_assist_info_c() = default;
  npn_paging_assist_info_c(const npn_paging_assist_info_c& other);
  npn_paging_assist_info_c& operator=(const npn_paging_assist_info_c& other);
  ~npn_paging_assist_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  allowed_pni_npn_list_l& pni_npn_paging_assist()
  {
    assert_choice_type(types::pni_npn_paging_assist, type_, "NPN-PagingAssistanceInformation");
    return c.get<allowed_pni_npn_list_l>();
  }
  protocol_ie_single_container_s<npn_paging_assist_info_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "NPN-PagingAssistanceInformation");
    return c.get<protocol_ie_single_container_s<npn_paging_assist_info_ext_ies_o>>();
  }
  const allowed_pni_npn_list_l& pni_npn_paging_assist() const
  {
    assert_choice_type(types::pni_npn_paging_assist, type_, "NPN-PagingAssistanceInformation");
    return c.get<allowed_pni_npn_list_l>();
  }
  const protocol_ie_single_container_s<npn_paging_assist_info_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "NPN-PagingAssistanceInformation");
    return c.get<protocol_ie_single_container_s<npn_paging_assist_info_ext_ies_o>>();
  }
  allowed_pni_npn_list_l&                                           set_pni_npn_paging_assist();
  protocol_ie_single_container_s<npn_paging_assist_info_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                     type_;
  choice_buffer_t<allowed_pni_npn_list_l, protocol_ie_single_container_s<npn_paging_assist_info_ext_ies_o>> c;

  void destroy_();
};

// NextPagingAreaScope ::= ENUMERATED
struct next_paging_area_scope_opts {
  enum options { same, changed, /*...*/ nulltype } value;

  const char* to_string() const;
};
using next_paging_area_scope_e = enumerated<next_paging_area_scope_opts, true>;

using paging_assis_datafor_c_ecapab_ue_ext_ies_container = protocol_ext_container_empty_l;

// PagingAssisDataforCEcapabUE ::= SEQUENCE
struct paging_assis_datafor_c_ecapab_ue_s {
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  eutra_cgi_s                                        eutra_cgi;
  unbounded_octstring<true>                          coverage_enhancement_level;
  paging_assis_datafor_c_ecapab_ue_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingAttemptInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using paging_attempt_info_ext_ies_o = protocol_ext_empty_o;

using recommended_cells_for_paging_ext_ies_container = protocol_ext_container_empty_l;

// RecommendedCellsForPaging ::= SEQUENCE
struct recommended_cells_for_paging_s {
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  recommended_cell_list_l                        recommended_cell_list;
  recommended_cells_for_paging_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AssistanceDataForPaging-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct assist_data_for_paging_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { npn_paging_assist_info, paging_assis_datafor_c_ecapab_ue, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    npn_paging_assist_info_c&                 npn_paging_assist_info();
    paging_assis_datafor_c_ecapab_ue_s&       paging_assis_datafor_c_ecapab_ue();
    const npn_paging_assist_info_c&           npn_paging_assist_info() const;
    const paging_assis_datafor_c_ecapab_ue_s& paging_assis_datafor_c_ecapab_ue() const;

  private:
    types                                                                         type_;
    choice_buffer_t<npn_paging_assist_info_c, paging_assis_datafor_c_ecapab_ue_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using assist_data_for_recommended_cells_ext_ies_container = protocol_ext_container_empty_l;

// AssistanceDataForRecommendedCells ::= SEQUENCE
struct assist_data_for_recommended_cells_s {
  bool                                                ext             = false;
  bool                                                ie_exts_present = false;
  recommended_cells_for_paging_s                      recommended_cells_for_paging;
  assist_data_for_recommended_cells_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using paging_attempt_info_ext_ies_container = protocol_ext_container_empty_l;

// PagingAttemptInformation ::= SEQUENCE
struct paging_attempt_info_s {
  bool                                  ext                            = false;
  bool                                  next_paging_area_scope_present = false;
  bool                                  ie_exts_present                = false;
  uint8_t                               paging_attempt_count           = 1;
  uint8_t                               intended_nof_paging_attempts   = 1;
  next_paging_area_scope_e              next_paging_area_scope;
  paging_attempt_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct assist_data_for_paging_ext_ies_container {
  bool                               npn_paging_assist_info_present           = false;
  bool                               paging_assis_datafor_c_ecapab_ue_present = false;
  npn_paging_assist_info_c           npn_paging_assist_info;
  paging_assis_datafor_c_ecapab_ue_s paging_assis_datafor_c_ecapab_ue;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AssistanceDataForPaging ::= SEQUENCE
struct assist_data_for_paging_s {
  bool                                     ext                                       = false;
  bool                                     assist_data_for_recommended_cells_present = false;
  bool                                     paging_attempt_info_present               = false;
  bool                                     ie_exts_present                           = false;
  assist_data_for_recommended_cells_s      assist_data_for_recommended_cells;
  paging_attempt_info_s                    paging_attempt_info;
  assist_data_for_paging_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AssociatedMBSQosFlowSetupRequestItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using associated_mbs_qos_flow_setup_request_item_ext_ies_o = protocol_ext_empty_o;

using associated_mbs_qos_flow_setup_request_item_ext_ies_container = protocol_ext_container_empty_l;

// AssociatedMBSQosFlowSetupRequestItem ::= SEQUENCE
struct associated_mbs_qos_flow_setup_request_item_s {
  bool                                                         ext                            = false;
  bool                                                         ie_exts_present                = false;
  uint8_t                                                      mbs_qos_flow_id                = 0;
  uint8_t                                                      associated_unicast_qos_flow_id = 0;
  associated_mbs_qos_flow_setup_request_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AssociatedMBSQosFlowSetupRequestList ::= SEQUENCE (SIZE (1..64)) OF AssociatedMBSQosFlowSetupRequestItem
using associated_mbs_qos_flow_setup_request_list_l = dyn_array<associated_mbs_qos_flow_setup_request_item_s>;

// AssociatedMBSQosFlowSetuporModifyRequestItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using associated_mbs_qos_flow_setupor_modify_request_item_ext_ies_o = protocol_ext_empty_o;

using associated_mbs_qos_flow_setupor_modify_request_item_ext_ies_container = protocol_ext_container_empty_l;

// AssociatedMBSQosFlowSetuporModifyRequestItem ::= SEQUENCE
struct associated_mbs_qos_flow_setupor_modify_request_item_s {
  bool                                                                  ext                            = false;
  bool                                                                  ie_exts_present                = false;
  uint8_t                                                               mbs_qos_flow_id                = 0;
  uint8_t                                                               associated_unicast_qos_flow_id = 0;
  associated_mbs_qos_flow_setupor_modify_request_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AssociatedMBSQosFlowSetuporModifyRequestList ::= SEQUENCE (SIZE (1..64)) OF
// AssociatedMBSQosFlowSetuporModifyRequestItem
using associated_mbs_qos_flow_setupor_modify_request_list_l =
    dyn_array<associated_mbs_qos_flow_setupor_modify_request_item_s>;

// AssociatedQosFlowItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct associated_qos_flow_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { current_qos_para_set_idx, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::current_qos_para_set_idx; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&       current_qos_para_set_idx() { return c; }
    const uint8_t& current_qos_para_set_idx() const { return c; }

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

// AssociatedQosFlowItem ::= SEQUENCE
struct associated_qos_flow_item_s {
  struct qos_flow_map_ind_opts {
    enum options { ul, dl, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using qos_flow_map_ind_e_ = enumerated<qos_flow_map_ind_opts, true>;

  // member variables
  bool                                                         ext                      = false;
  bool                                                         qos_flow_map_ind_present = false;
  uint8_t                                                      qos_flow_id              = 0;
  qos_flow_map_ind_e_                                          qos_flow_map_ind;
  protocol_ext_container_l<associated_qos_flow_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AssociatedQosFlowList ::= SEQUENCE (SIZE (1..64)) OF AssociatedQosFlowItem
using associated_qos_flow_list_l = dyn_array<associated_qos_flow_item_s>;

// AvailableRANVisibleQoEMetrics-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using available_ran_visible_qo_e_metrics_ext_ies_o = protocol_ext_empty_o;

using available_ran_visible_qo_e_metrics_ext_ies_container = protocol_ext_container_empty_l;

// AvailableRANVisibleQoEMetrics ::= SEQUENCE
struct available_ran_visible_qo_e_metrics_s {
  struct application_layer_buffer_level_list_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using application_layer_buffer_level_list_e_ = enumerated<application_layer_buffer_level_list_opts, true>;
  struct playout_delay_for_media_startup_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using playout_delay_for_media_startup_e_ = enumerated<playout_delay_for_media_startup_opts, true>;

  // member variables
  bool                                                 ext                                         = false;
  bool                                                 application_layer_buffer_level_list_present = false;
  bool                                                 playout_delay_for_media_startup_present     = false;
  bool                                                 ie_exts_present                             = false;
  application_layer_buffer_level_list_e_               application_layer_buffer_level_list;
  playout_delay_for_media_startup_e_                   playout_delay_for_media_startup;
  available_ran_visible_qo_e_metrics_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BeamMeasurementsReportQuantity-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using beam_meass_report_quant_ext_ies_o = protocol_ext_empty_o;

// BeamMeasurementsReportConfiguration-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using beam_meass_report_cfg_ext_ies_o = protocol_ext_empty_o;

using beam_meass_report_quant_ext_ies_container = protocol_ext_container_empty_l;

// BeamMeasurementsReportQuantity ::= SEQUENCE
struct beam_meass_report_quant_s {
  struct rsrp_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using rsrp_e_ = enumerated<rsrp_opts, true>;
  struct rsrq_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using rsrq_e_ = enumerated<rsrq_opts, true>;
  struct sinr_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using sinr_e_ = enumerated<sinr_opts, true>;

  // member variables
  bool                                      ext             = false;
  bool                                      ie_exts_present = false;
  rsrp_e_                                   rsrp;
  rsrq_e_                                   rsrq;
  sinr_e_                                   sinr;
  beam_meass_report_quant_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using beam_meass_report_cfg_ext_ies_container = protocol_ext_container_empty_l;

// BeamMeasurementsReportConfiguration ::= SEQUENCE
struct beam_meass_report_cfg_s {
  bool                                    ext                                 = false;
  bool                                    beam_meass_report_quant_present     = false;
  bool                                    max_nrof_rs_idxes_to_report_present = false;
  bool                                    ie_exts_present                     = false;
  beam_meass_report_quant_s               beam_meass_report_quant;
  uint8_t                                 max_nrof_rs_idxes_to_report = 1;
  beam_meass_report_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BluetoothMeasConfigNameItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using bluetooth_meas_cfg_name_item_ext_ies_o = protocol_ext_empty_o;

using bluetooth_meas_cfg_name_item_ext_ies_container = protocol_ext_container_empty_l;

// BluetoothMeasConfigNameItem ::= SEQUENCE
struct bluetooth_meas_cfg_name_item_s {
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  bounded_octstring<1, 248, true>                bluetooth_name;
  bluetooth_meas_cfg_name_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BluetoothMeasConfigNameList ::= SEQUENCE (SIZE (1..4)) OF BluetoothMeasConfigNameItem
using bluetooth_meas_cfg_name_list_l = dyn_array<bluetooth_meas_cfg_name_item_s>;

// BluetoothMeasConfig ::= ENUMERATED
struct bluetooth_meas_cfg_opts {
  enum options { setup, /*...*/ nulltype } value;

  const char* to_string() const;
};
using bluetooth_meas_cfg_e = enumerated<bluetooth_meas_cfg_opts, true>;

// BluetoothMeasurementConfiguration-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using bluetooth_meas_cfg_ext_ies_o = protocol_ext_empty_o;

using bluetooth_meas_cfg_ext_ies_container = protocol_ext_container_empty_l;

// BluetoothMeasurementConfiguration ::= SEQUENCE
struct bluetooth_meas_cfg_s {
  struct bt_rssi_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using bt_rssi_e_ = enumerated<bt_rssi_opts, true>;

  // member variables
  bool                                 ext             = false;
  bool                                 bt_rssi_present = false;
  bool                                 ie_exts_present = false;
  bluetooth_meas_cfg_e                 bluetooth_meas_cfg;
  bluetooth_meas_cfg_name_list_l       bluetooth_meas_cfg_name_list;
  bt_rssi_e_                           bt_rssi;
  bluetooth_meas_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CancelledCellsInEAI-EUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using cancelled_cells_in_eai_eutra_item_ext_ies_o = protocol_ext_empty_o;

// CancelledCellsInEAI-NR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using cancelled_cells_in_eai_nr_item_ext_ies_o = protocol_ext_empty_o;

// CancelledCellsInTAI-EUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using cancelled_cells_in_tai_eutra_item_ext_ies_o = protocol_ext_empty_o;

// CancelledCellsInTAI-NR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using cancelled_cells_in_tai_nr_item_ext_ies_o = protocol_ext_empty_o;

using cancelled_cells_in_eai_eutra_item_ext_ies_container = protocol_ext_container_empty_l;

// CancelledCellsInEAI-EUTRA-Item ::= SEQUENCE
struct cancelled_cells_in_eai_eutra_item_s {
  bool                                                ext             = false;
  bool                                                ie_exts_present = false;
  eutra_cgi_s                                         eutra_cgi;
  uint32_t                                            nof_broadcasts = 0;
  cancelled_cells_in_eai_eutra_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using cancelled_cells_in_eai_nr_item_ext_ies_container = protocol_ext_container_empty_l;

// CancelledCellsInEAI-NR-Item ::= SEQUENCE
struct cancelled_cells_in_eai_nr_item_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  nr_cgi_s                                         nr_cgi;
  uint32_t                                         nof_broadcasts = 0;
  cancelled_cells_in_eai_nr_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using cancelled_cells_in_tai_eutra_item_ext_ies_container = protocol_ext_container_empty_l;

// CancelledCellsInTAI-EUTRA-Item ::= SEQUENCE
struct cancelled_cells_in_tai_eutra_item_s {
  bool                                                ext             = false;
  bool                                                ie_exts_present = false;
  eutra_cgi_s                                         eutra_cgi;
  uint32_t                                            nof_broadcasts = 0;
  cancelled_cells_in_tai_eutra_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using cancelled_cells_in_tai_nr_item_ext_ies_container = protocol_ext_container_empty_l;

// CancelledCellsInTAI-NR-Item ::= SEQUENCE
struct cancelled_cells_in_tai_nr_item_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  nr_cgi_s                                         nr_cgi;
  uint32_t                                         nof_broadcasts = 0;
  cancelled_cells_in_tai_nr_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CancelledCellsInEAI-EUTRA ::= SEQUENCE (SIZE (1..65535)) OF CancelledCellsInEAI-EUTRA-Item
using cancelled_cells_in_eai_eutra_l = dyn_array<cancelled_cells_in_eai_eutra_item_s>;

// CancelledCellsInEAI-NR ::= SEQUENCE (SIZE (1..65535)) OF CancelledCellsInEAI-NR-Item
using cancelled_cells_in_eai_nr_l = dyn_array<cancelled_cells_in_eai_nr_item_s>;

// CancelledCellsInTAI-EUTRA ::= SEQUENCE (SIZE (1..65535)) OF CancelledCellsInTAI-EUTRA-Item
using cancelled_cells_in_tai_eutra_l = dyn_array<cancelled_cells_in_tai_eutra_item_s>;

// CancelledCellsInTAI-NR ::= SEQUENCE (SIZE (1..65535)) OF CancelledCellsInTAI-NR-Item
using cancelled_cells_in_tai_nr_l = dyn_array<cancelled_cells_in_tai_nr_item_s>;

// CellIDCancelledEUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using cell_id_cancelled_eutra_item_ext_ies_o = protocol_ext_empty_o;

// CellIDCancelledNR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using cell_id_cancelled_nr_item_ext_ies_o = protocol_ext_empty_o;

// EmergencyAreaIDCancelledEUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using emergency_area_id_cancelled_eutra_item_ext_ies_o = protocol_ext_empty_o;

// EmergencyAreaIDCancelledNR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using emergency_area_id_cancelled_nr_item_ext_ies_o = protocol_ext_empty_o;

// TAICancelledEUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using tai_cancelled_eutra_item_ext_ies_o = protocol_ext_empty_o;

// TAICancelledNR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using tai_cancelled_nr_item_ext_ies_o = protocol_ext_empty_o;

using cell_id_cancelled_eutra_item_ext_ies_container = protocol_ext_container_empty_l;

// CellIDCancelledEUTRA-Item ::= SEQUENCE
struct cell_id_cancelled_eutra_item_s {
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  eutra_cgi_s                                    eutra_cgi;
  uint32_t                                       nof_broadcasts = 0;
  cell_id_cancelled_eutra_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using cell_id_cancelled_nr_item_ext_ies_container = protocol_ext_container_empty_l;

// CellIDCancelledNR-Item ::= SEQUENCE
struct cell_id_cancelled_nr_item_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  nr_cgi_s                                    nr_cgi;
  uint32_t                                    nof_broadcasts = 0;
  cell_id_cancelled_nr_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using emergency_area_id_cancelled_eutra_item_ext_ies_container = protocol_ext_container_empty_l;

// EmergencyAreaIDCancelledEUTRA-Item ::= SEQUENCE
struct emergency_area_id_cancelled_eutra_item_s {
  bool                                                     ext             = false;
  bool                                                     ie_exts_present = false;
  fixed_octstring<3, true>                                 emergency_area_id;
  cancelled_cells_in_eai_eutra_l                           cancelled_cells_in_eai_eutra;
  emergency_area_id_cancelled_eutra_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using emergency_area_id_cancelled_nr_item_ext_ies_container = protocol_ext_container_empty_l;

// EmergencyAreaIDCancelledNR-Item ::= SEQUENCE
struct emergency_area_id_cancelled_nr_item_s {
  bool                                                  ext             = false;
  bool                                                  ie_exts_present = false;
  fixed_octstring<3, true>                              emergency_area_id;
  cancelled_cells_in_eai_nr_l                           cancelled_cells_in_eai_nr;
  emergency_area_id_cancelled_nr_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using tai_cancelled_eutra_item_ext_ies_container = protocol_ext_container_empty_l;

// TAICancelledEUTRA-Item ::= SEQUENCE
struct tai_cancelled_eutra_item_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  tai_s                                      tai;
  cancelled_cells_in_tai_eutra_l             cancelled_cells_in_tai_eutra;
  tai_cancelled_eutra_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using tai_cancelled_nr_item_ext_ies_container = protocol_ext_container_empty_l;

// TAICancelledNR-Item ::= SEQUENCE
struct tai_cancelled_nr_item_s {
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  tai_s                                   tai;
  cancelled_cells_in_tai_nr_l             cancelled_cells_in_tai_nr;
  tai_cancelled_nr_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastCancelledAreaList-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using broadcast_cancelled_area_list_ext_ies_o = protocol_ies_empty_o;

// CellIDCancelledEUTRA ::= SEQUENCE (SIZE (1..65535)) OF CellIDCancelledEUTRA-Item
using cell_id_cancelled_eutra_l = dyn_array<cell_id_cancelled_eutra_item_s>;

// CellIDCancelledNR ::= SEQUENCE (SIZE (1..65535)) OF CellIDCancelledNR-Item
using cell_id_cancelled_nr_l = dyn_array<cell_id_cancelled_nr_item_s>;

// EmergencyAreaIDCancelledEUTRA ::= SEQUENCE (SIZE (1..65535)) OF EmergencyAreaIDCancelledEUTRA-Item
using emergency_area_id_cancelled_eutra_l = dyn_array<emergency_area_id_cancelled_eutra_item_s>;

// EmergencyAreaIDCancelledNR ::= SEQUENCE (SIZE (1..65535)) OF EmergencyAreaIDCancelledNR-Item
using emergency_area_id_cancelled_nr_l = dyn_array<emergency_area_id_cancelled_nr_item_s>;

// TAICancelledEUTRA ::= SEQUENCE (SIZE (1..65535)) OF TAICancelledEUTRA-Item
using tai_cancelled_eutra_l = dyn_array<tai_cancelled_eutra_item_s>;

// TAICancelledNR ::= SEQUENCE (SIZE (1..65535)) OF TAICancelledNR-Item
using tai_cancelled_nr_l = dyn_array<tai_cancelled_nr_item_s>;

// BroadcastCancelledAreaList ::= CHOICE
struct broadcast_cancelled_area_list_c {
  struct types_opts {
    enum options {
      cell_id_cancelled_eutra,
      tai_cancelled_eutra,
      emergency_area_id_cancelled_eutra,
      cell_id_cancelled_nr,
      tai_cancelled_nr,
      emergency_area_id_cancelled_nr,
      choice_exts,
      nulltype
    } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  broadcast_cancelled_area_list_c() = default;
  broadcast_cancelled_area_list_c(const broadcast_cancelled_area_list_c& other);
  broadcast_cancelled_area_list_c& operator=(const broadcast_cancelled_area_list_c& other);
  ~broadcast_cancelled_area_list_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cell_id_cancelled_eutra_l& cell_id_cancelled_eutra()
  {
    assert_choice_type(types::cell_id_cancelled_eutra, type_, "BroadcastCancelledAreaList");
    return c.get<cell_id_cancelled_eutra_l>();
  }
  tai_cancelled_eutra_l& tai_cancelled_eutra()
  {
    assert_choice_type(types::tai_cancelled_eutra, type_, "BroadcastCancelledAreaList");
    return c.get<tai_cancelled_eutra_l>();
  }
  emergency_area_id_cancelled_eutra_l& emergency_area_id_cancelled_eutra()
  {
    assert_choice_type(types::emergency_area_id_cancelled_eutra, type_, "BroadcastCancelledAreaList");
    return c.get<emergency_area_id_cancelled_eutra_l>();
  }
  cell_id_cancelled_nr_l& cell_id_cancelled_nr()
  {
    assert_choice_type(types::cell_id_cancelled_nr, type_, "BroadcastCancelledAreaList");
    return c.get<cell_id_cancelled_nr_l>();
  }
  tai_cancelled_nr_l& tai_cancelled_nr()
  {
    assert_choice_type(types::tai_cancelled_nr, type_, "BroadcastCancelledAreaList");
    return c.get<tai_cancelled_nr_l>();
  }
  emergency_area_id_cancelled_nr_l& emergency_area_id_cancelled_nr()
  {
    assert_choice_type(types::emergency_area_id_cancelled_nr, type_, "BroadcastCancelledAreaList");
    return c.get<emergency_area_id_cancelled_nr_l>();
  }
  protocol_ie_single_container_s<broadcast_cancelled_area_list_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "BroadcastCancelledAreaList");
    return c.get<protocol_ie_single_container_s<broadcast_cancelled_area_list_ext_ies_o>>();
  }
  const cell_id_cancelled_eutra_l& cell_id_cancelled_eutra() const
  {
    assert_choice_type(types::cell_id_cancelled_eutra, type_, "BroadcastCancelledAreaList");
    return c.get<cell_id_cancelled_eutra_l>();
  }
  const tai_cancelled_eutra_l& tai_cancelled_eutra() const
  {
    assert_choice_type(types::tai_cancelled_eutra, type_, "BroadcastCancelledAreaList");
    return c.get<tai_cancelled_eutra_l>();
  }
  const emergency_area_id_cancelled_eutra_l& emergency_area_id_cancelled_eutra() const
  {
    assert_choice_type(types::emergency_area_id_cancelled_eutra, type_, "BroadcastCancelledAreaList");
    return c.get<emergency_area_id_cancelled_eutra_l>();
  }
  const cell_id_cancelled_nr_l& cell_id_cancelled_nr() const
  {
    assert_choice_type(types::cell_id_cancelled_nr, type_, "BroadcastCancelledAreaList");
    return c.get<cell_id_cancelled_nr_l>();
  }
  const tai_cancelled_nr_l& tai_cancelled_nr() const
  {
    assert_choice_type(types::tai_cancelled_nr, type_, "BroadcastCancelledAreaList");
    return c.get<tai_cancelled_nr_l>();
  }
  const emergency_area_id_cancelled_nr_l& emergency_area_id_cancelled_nr() const
  {
    assert_choice_type(types::emergency_area_id_cancelled_nr, type_, "BroadcastCancelledAreaList");
    return c.get<emergency_area_id_cancelled_nr_l>();
  }
  const protocol_ie_single_container_s<broadcast_cancelled_area_list_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "BroadcastCancelledAreaList");
    return c.get<protocol_ie_single_container_s<broadcast_cancelled_area_list_ext_ies_o>>();
  }
  cell_id_cancelled_eutra_l&                                               set_cell_id_cancelled_eutra();
  tai_cancelled_eutra_l&                                                   set_tai_cancelled_eutra();
  emergency_area_id_cancelled_eutra_l&                                     set_emergency_area_id_cancelled_eutra();
  cell_id_cancelled_nr_l&                                                  set_cell_id_cancelled_nr();
  tai_cancelled_nr_l&                                                      set_tai_cancelled_nr();
  emergency_area_id_cancelled_nr_l&                                        set_emergency_area_id_cancelled_nr();
  protocol_ie_single_container_s<broadcast_cancelled_area_list_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<cell_id_cancelled_eutra_l,
                  cell_id_cancelled_nr_l,
                  emergency_area_id_cancelled_eutra_l,
                  emergency_area_id_cancelled_nr_l,
                  protocol_ie_single_container_s<broadcast_cancelled_area_list_ext_ies_o>,
                  tai_cancelled_eutra_l,
                  tai_cancelled_nr_l>
      c;

  void destroy_();
};

// CompletedCellsInEAI-EUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using completed_cells_in_eai_eutra_item_ext_ies_o = protocol_ext_empty_o;

// CompletedCellsInEAI-NR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using completed_cells_in_eai_nr_item_ext_ies_o = protocol_ext_empty_o;

// CompletedCellsInTAI-EUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using completed_cells_in_tai_eutra_item_ext_ies_o = protocol_ext_empty_o;

// CompletedCellsInTAI-NR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using completed_cells_in_tai_nr_item_ext_ies_o = protocol_ext_empty_o;

using completed_cells_in_eai_eutra_item_ext_ies_container = protocol_ext_container_empty_l;

// CompletedCellsInEAI-EUTRA-Item ::= SEQUENCE
struct completed_cells_in_eai_eutra_item_s {
  bool                                                ext             = false;
  bool                                                ie_exts_present = false;
  eutra_cgi_s                                         eutra_cgi;
  completed_cells_in_eai_eutra_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using completed_cells_in_eai_nr_item_ext_ies_container = protocol_ext_container_empty_l;

// CompletedCellsInEAI-NR-Item ::= SEQUENCE
struct completed_cells_in_eai_nr_item_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  nr_cgi_s                                         nr_cgi;
  completed_cells_in_eai_nr_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using completed_cells_in_tai_eutra_item_ext_ies_container = protocol_ext_container_empty_l;

// CompletedCellsInTAI-EUTRA-Item ::= SEQUENCE
struct completed_cells_in_tai_eutra_item_s {
  bool                                                ext             = false;
  bool                                                ie_exts_present = false;
  eutra_cgi_s                                         eutra_cgi;
  completed_cells_in_tai_eutra_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using completed_cells_in_tai_nr_item_ext_ies_container = protocol_ext_container_empty_l;

// CompletedCellsInTAI-NR-Item ::= SEQUENCE
struct completed_cells_in_tai_nr_item_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  nr_cgi_s                                         nr_cgi;
  completed_cells_in_tai_nr_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellIDBroadcastEUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using cell_id_broadcast_eutra_item_ext_ies_o = protocol_ext_empty_o;

// CellIDBroadcastNR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using cell_id_broadcast_nr_item_ext_ies_o = protocol_ext_empty_o;

// CompletedCellsInEAI-EUTRA ::= SEQUENCE (SIZE (1..65535)) OF CompletedCellsInEAI-EUTRA-Item
using completed_cells_in_eai_eutra_l = dyn_array<completed_cells_in_eai_eutra_item_s>;

// CompletedCellsInEAI-NR ::= SEQUENCE (SIZE (1..65535)) OF CompletedCellsInEAI-NR-Item
using completed_cells_in_eai_nr_l = dyn_array<completed_cells_in_eai_nr_item_s>;

// CompletedCellsInTAI-EUTRA ::= SEQUENCE (SIZE (1..65535)) OF CompletedCellsInTAI-EUTRA-Item
using completed_cells_in_tai_eutra_l = dyn_array<completed_cells_in_tai_eutra_item_s>;

// CompletedCellsInTAI-NR ::= SEQUENCE (SIZE (1..65535)) OF CompletedCellsInTAI-NR-Item
using completed_cells_in_tai_nr_l = dyn_array<completed_cells_in_tai_nr_item_s>;

// EmergencyAreaIDBroadcastEUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using emergency_area_id_broadcast_eutra_item_ext_ies_o = protocol_ext_empty_o;

// EmergencyAreaIDBroadcastNR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using emergency_area_id_broadcast_nr_item_ext_ies_o = protocol_ext_empty_o;

// TAIBroadcastEUTRA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using tai_broadcast_eutra_item_ext_ies_o = protocol_ext_empty_o;

// TAIBroadcastNR-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using tai_broadcast_nr_item_ext_ies_o = protocol_ext_empty_o;

using cell_id_broadcast_eutra_item_ext_ies_container = protocol_ext_container_empty_l;

// CellIDBroadcastEUTRA-Item ::= SEQUENCE
struct cell_id_broadcast_eutra_item_s {
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  eutra_cgi_s                                    eutra_cgi;
  cell_id_broadcast_eutra_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using cell_id_broadcast_nr_item_ext_ies_container = protocol_ext_container_empty_l;

// CellIDBroadcastNR-Item ::= SEQUENCE
struct cell_id_broadcast_nr_item_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  nr_cgi_s                                    nr_cgi;
  cell_id_broadcast_nr_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using emergency_area_id_broadcast_eutra_item_ext_ies_container = protocol_ext_container_empty_l;

// EmergencyAreaIDBroadcastEUTRA-Item ::= SEQUENCE
struct emergency_area_id_broadcast_eutra_item_s {
  bool                                                     ext             = false;
  bool                                                     ie_exts_present = false;
  fixed_octstring<3, true>                                 emergency_area_id;
  completed_cells_in_eai_eutra_l                           completed_cells_in_eai_eutra;
  emergency_area_id_broadcast_eutra_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using emergency_area_id_broadcast_nr_item_ext_ies_container = protocol_ext_container_empty_l;

// EmergencyAreaIDBroadcastNR-Item ::= SEQUENCE
struct emergency_area_id_broadcast_nr_item_s {
  bool                                                  ext             = false;
  bool                                                  ie_exts_present = false;
  fixed_octstring<3, true>                              emergency_area_id;
  completed_cells_in_eai_nr_l                           completed_cells_in_eai_nr;
  emergency_area_id_broadcast_nr_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using tai_broadcast_eutra_item_ext_ies_container = protocol_ext_container_empty_l;

// TAIBroadcastEUTRA-Item ::= SEQUENCE
struct tai_broadcast_eutra_item_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  tai_s                                      tai;
  completed_cells_in_tai_eutra_l             completed_cells_in_tai_eutra;
  tai_broadcast_eutra_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using tai_broadcast_nr_item_ext_ies_container = protocol_ext_container_empty_l;

// TAIBroadcastNR-Item ::= SEQUENCE
struct tai_broadcast_nr_item_s {
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  tai_s                                   tai;
  completed_cells_in_tai_nr_l             completed_cells_in_tai_nr;
  tai_broadcast_nr_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastCompletedAreaList-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using broadcast_completed_area_list_ext_ies_o = protocol_ies_empty_o;

// CellIDBroadcastEUTRA ::= SEQUENCE (SIZE (1..65535)) OF CellIDBroadcastEUTRA-Item
using cell_id_broadcast_eutra_l = dyn_array<cell_id_broadcast_eutra_item_s>;

// CellIDBroadcastNR ::= SEQUENCE (SIZE (1..65535)) OF CellIDBroadcastNR-Item
using cell_id_broadcast_nr_l = dyn_array<cell_id_broadcast_nr_item_s>;

// EmergencyAreaIDBroadcastEUTRA ::= SEQUENCE (SIZE (1..65535)) OF EmergencyAreaIDBroadcastEUTRA-Item
using emergency_area_id_broadcast_eutra_l = dyn_array<emergency_area_id_broadcast_eutra_item_s>;

// EmergencyAreaIDBroadcastNR ::= SEQUENCE (SIZE (1..65535)) OF EmergencyAreaIDBroadcastNR-Item
using emergency_area_id_broadcast_nr_l = dyn_array<emergency_area_id_broadcast_nr_item_s>;

// TAIBroadcastEUTRA ::= SEQUENCE (SIZE (1..65535)) OF TAIBroadcastEUTRA-Item
using tai_broadcast_eutra_l = dyn_array<tai_broadcast_eutra_item_s>;

// TAIBroadcastNR ::= SEQUENCE (SIZE (1..65535)) OF TAIBroadcastNR-Item
using tai_broadcast_nr_l = dyn_array<tai_broadcast_nr_item_s>;

// BroadcastCompletedAreaList ::= CHOICE
struct broadcast_completed_area_list_c {
  struct types_opts {
    enum options {
      cell_id_broadcast_eutra,
      tai_broadcast_eutra,
      emergency_area_id_broadcast_eutra,
      cell_id_broadcast_nr,
      tai_broadcast_nr,
      emergency_area_id_broadcast_nr,
      choice_exts,
      nulltype
    } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  broadcast_completed_area_list_c() = default;
  broadcast_completed_area_list_c(const broadcast_completed_area_list_c& other);
  broadcast_completed_area_list_c& operator=(const broadcast_completed_area_list_c& other);
  ~broadcast_completed_area_list_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cell_id_broadcast_eutra_l& cell_id_broadcast_eutra()
  {
    assert_choice_type(types::cell_id_broadcast_eutra, type_, "BroadcastCompletedAreaList");
    return c.get<cell_id_broadcast_eutra_l>();
  }
  tai_broadcast_eutra_l& tai_broadcast_eutra()
  {
    assert_choice_type(types::tai_broadcast_eutra, type_, "BroadcastCompletedAreaList");
    return c.get<tai_broadcast_eutra_l>();
  }
  emergency_area_id_broadcast_eutra_l& emergency_area_id_broadcast_eutra()
  {
    assert_choice_type(types::emergency_area_id_broadcast_eutra, type_, "BroadcastCompletedAreaList");
    return c.get<emergency_area_id_broadcast_eutra_l>();
  }
  cell_id_broadcast_nr_l& cell_id_broadcast_nr()
  {
    assert_choice_type(types::cell_id_broadcast_nr, type_, "BroadcastCompletedAreaList");
    return c.get<cell_id_broadcast_nr_l>();
  }
  tai_broadcast_nr_l& tai_broadcast_nr()
  {
    assert_choice_type(types::tai_broadcast_nr, type_, "BroadcastCompletedAreaList");
    return c.get<tai_broadcast_nr_l>();
  }
  emergency_area_id_broadcast_nr_l& emergency_area_id_broadcast_nr()
  {
    assert_choice_type(types::emergency_area_id_broadcast_nr, type_, "BroadcastCompletedAreaList");
    return c.get<emergency_area_id_broadcast_nr_l>();
  }
  protocol_ie_single_container_s<broadcast_completed_area_list_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "BroadcastCompletedAreaList");
    return c.get<protocol_ie_single_container_s<broadcast_completed_area_list_ext_ies_o>>();
  }
  const cell_id_broadcast_eutra_l& cell_id_broadcast_eutra() const
  {
    assert_choice_type(types::cell_id_broadcast_eutra, type_, "BroadcastCompletedAreaList");
    return c.get<cell_id_broadcast_eutra_l>();
  }
  const tai_broadcast_eutra_l& tai_broadcast_eutra() const
  {
    assert_choice_type(types::tai_broadcast_eutra, type_, "BroadcastCompletedAreaList");
    return c.get<tai_broadcast_eutra_l>();
  }
  const emergency_area_id_broadcast_eutra_l& emergency_area_id_broadcast_eutra() const
  {
    assert_choice_type(types::emergency_area_id_broadcast_eutra, type_, "BroadcastCompletedAreaList");
    return c.get<emergency_area_id_broadcast_eutra_l>();
  }
  const cell_id_broadcast_nr_l& cell_id_broadcast_nr() const
  {
    assert_choice_type(types::cell_id_broadcast_nr, type_, "BroadcastCompletedAreaList");
    return c.get<cell_id_broadcast_nr_l>();
  }
  const tai_broadcast_nr_l& tai_broadcast_nr() const
  {
    assert_choice_type(types::tai_broadcast_nr, type_, "BroadcastCompletedAreaList");
    return c.get<tai_broadcast_nr_l>();
  }
  const emergency_area_id_broadcast_nr_l& emergency_area_id_broadcast_nr() const
  {
    assert_choice_type(types::emergency_area_id_broadcast_nr, type_, "BroadcastCompletedAreaList");
    return c.get<emergency_area_id_broadcast_nr_l>();
  }
  const protocol_ie_single_container_s<broadcast_completed_area_list_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "BroadcastCompletedAreaList");
    return c.get<protocol_ie_single_container_s<broadcast_completed_area_list_ext_ies_o>>();
  }
  cell_id_broadcast_eutra_l&                                               set_cell_id_broadcast_eutra();
  tai_broadcast_eutra_l&                                                   set_tai_broadcast_eutra();
  emergency_area_id_broadcast_eutra_l&                                     set_emergency_area_id_broadcast_eutra();
  cell_id_broadcast_nr_l&                                                  set_cell_id_broadcast_nr();
  tai_broadcast_nr_l&                                                      set_tai_broadcast_nr();
  emergency_area_id_broadcast_nr_l&                                        set_emergency_area_id_broadcast_nr();
  protocol_ie_single_container_s<broadcast_completed_area_list_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<cell_id_broadcast_eutra_l,
                  cell_id_broadcast_nr_l,
                  emergency_area_id_broadcast_eutra_l,
                  emergency_area_id_broadcast_nr_l,
                  protocol_ie_single_container_s<broadcast_completed_area_list_ext_ies_o>,
                  tai_broadcast_eutra_l,
                  tai_broadcast_nr_l>
      c;

  void destroy_();
};

// TAINSAGSupportItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using tai_nsag_support_item_ext_ies_o = protocol_ext_empty_o;

using tai_nsag_support_item_ext_ies_container = protocol_ext_container_empty_l;

// TAINSAGSupportItem ::= SEQUENCE
struct tai_nsag_support_item_s {
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  uint16_t                                nsag_id         = 0;
  extended_slice_support_list_l           nsag_slice_support_list;
  tai_nsag_support_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAINSAGSupportList ::= SEQUENCE (SIZE (1..256)) OF TAINSAGSupportItem
using tai_nsag_support_list_l = dyn_array<tai_nsag_support_item_s>;

// BroadcastPLMNItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct broadcast_plmn_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { npn_support, extended_tai_slice_support_list, tai_nsag_support_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    npn_support_c&                       npn_support();
    extended_slice_support_list_l&       extended_tai_slice_support_list();
    tai_nsag_support_list_l&             tai_nsag_support_list();
    const npn_support_c&                 npn_support() const;
    const extended_slice_support_list_l& extended_tai_slice_support_list() const;
    const tai_nsag_support_list_l&       tai_nsag_support_list() const;

  private:
    types                                                                                  type_;
    choice_buffer_t<extended_slice_support_list_l, npn_support_c, tai_nsag_support_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_plmn_item_ext_ies_container {
  bool                          npn_support_present                     = false;
  bool                          extended_tai_slice_support_list_present = false;
  bool                          tai_nsag_support_list_present           = false;
  npn_support_c                 npn_support;
  extended_slice_support_list_l extended_tai_slice_support_list;
  tai_nsag_support_list_l       tai_nsag_support_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastPLMNItem ::= SEQUENCE
struct broadcast_plmn_item_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  fixed_octstring<3, true>              plmn_id;
  slice_support_list_l                  tai_slice_support_list;
  broadcast_plmn_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastPLMNList ::= SEQUENCE (SIZE (1..12)) OF BroadcastPLMNItem
using broadcast_plmn_list_l = dyn_array<broadcast_plmn_item_s>;

// MBS-SessionID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_session_id_ext_ies_o = protocol_ext_empty_o;

using mbs_session_id_ext_ies_container = protocol_ext_container_empty_l;

// MBS-SessionID ::= SEQUENCE
struct mbs_session_id_s {
  bool                             ext             = false;
  bool                             nid_present     = false;
  bool                             ie_exts_present = false;
  fixed_octstring<6, true>         tmgi;
  fixed_bitstring<44, false, true> nid;
  mbs_session_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-ServiceAreaCellList ::= SEQUENCE (SIZE (1..8192)) OF NR-CGI
using mbs_service_area_cell_list_l = dyn_array<nr_cgi_s>;

// MBS-ServiceAreaInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_service_area_info_ext_ies_o = protocol_ext_empty_o;

// MBS-ServiceAreaTAIList ::= SEQUENCE (SIZE (1..1024)) OF TAI
using mbs_service_area_tai_list_l = dyn_array<tai_s>;

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

// MBS-ServiceAreaInformationItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
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

// MBS-ServiceArea-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using mbs_service_area_ext_ies_o = protocol_ies_empty_o;

// MBS-ServiceAreaInformationList ::= SEQUENCE (SIZE (1..256)) OF MBS-ServiceAreaInformationItem
using mbs_service_area_info_list_l = dyn_array<mbs_service_area_info_item_s>;

// MBS-ServiceArea ::= CHOICE
struct mbs_service_area_c {
  struct types_opts {
    enum options { locationindependent, locationdependent, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  mbs_service_area_c() = default;
  mbs_service_area_c(const mbs_service_area_c& other);
  mbs_service_area_c& operator=(const mbs_service_area_c& other);
  ~mbs_service_area_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  mbs_service_area_info_s& locationindependent()
  {
    assert_choice_type(types::locationindependent, type_, "MBS-ServiceArea");
    return c.get<mbs_service_area_info_s>();
  }
  mbs_service_area_info_list_l& locationdependent()
  {
    assert_choice_type(types::locationdependent, type_, "MBS-ServiceArea");
    return c.get<mbs_service_area_info_list_l>();
  }
  protocol_ie_single_container_s<mbs_service_area_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "MBS-ServiceArea");
    return c.get<protocol_ie_single_container_s<mbs_service_area_ext_ies_o>>();
  }
  const mbs_service_area_info_s& locationindependent() const
  {
    assert_choice_type(types::locationindependent, type_, "MBS-ServiceArea");
    return c.get<mbs_service_area_info_s>();
  }
  const mbs_service_area_info_list_l& locationdependent() const
  {
    assert_choice_type(types::locationdependent, type_, "MBS-ServiceArea");
    return c.get<mbs_service_area_info_list_l>();
  }
  const protocol_ie_single_container_s<mbs_service_area_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "MBS-ServiceArea");
    return c.get<protocol_ie_single_container_s<mbs_service_area_ext_ies_o>>();
  }
  mbs_service_area_info_s&                                    set_locationindependent();
  mbs_service_area_info_list_l&                               set_locationdependent();
  protocol_ie_single_container_s<mbs_service_area_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<mbs_service_area_info_list_l,
                  mbs_service_area_info_s,
                  protocol_ie_single_container_s<mbs_service_area_ext_ies_o>>
      c;

  void destroy_();
};

// ExpectedUEMovingTrajectoryItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using expected_ue_moving_trajectory_item_ext_ies_o = protocol_ext_empty_o;

// ExpectedUEActivityBehaviour-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using expected_ue_activity_behaviour_ext_ies_o = protocol_ext_empty_o;

using expected_ue_moving_trajectory_item_ext_ies_container = protocol_ext_container_empty_l;

// ExpectedUEMovingTrajectoryItem ::= SEQUENCE
struct expected_ue_moving_trajectory_item_s {
  bool                                                 ext                         = false;
  bool                                                 time_stayed_in_cell_present = false;
  bool                                                 ie_exts_present             = false;
  ngran_cgi_c                                          ngran_cgi;
  uint16_t                                             time_stayed_in_cell = 0;
  expected_ue_moving_trajectory_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SourceOfUEActivityBehaviourInformation ::= ENUMERATED
struct source_of_ue_activity_behaviour_info_opts {
  enum options { sub_info, stats, /*...*/ nulltype } value;

  const char* to_string() const;
};
using source_of_ue_activity_behaviour_info_e = enumerated<source_of_ue_activity_behaviour_info_opts, true>;

// ExpectedHOInterval ::= ENUMERATED
struct expected_ho_interv_opts {
  enum options { sec15, sec30, sec60, sec90, sec120, sec180, long_time, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using expected_ho_interv_e = enumerated<expected_ho_interv_opts, true>;

using expected_ue_activity_behaviour_ext_ies_container = protocol_ext_container_empty_l;

// ExpectedUEActivityBehaviour ::= SEQUENCE
struct expected_ue_activity_behaviour_s {
  bool                                             ext                                          = false;
  bool                                             expected_activity_period_present             = false;
  bool                                             expected_idle_period_present                 = false;
  bool                                             source_of_ue_activity_behaviour_info_present = false;
  bool                                             ie_exts_present                              = false;
  uint8_t                                          expected_activity_period                     = 1;
  uint8_t                                          expected_idle_period                         = 1;
  source_of_ue_activity_behaviour_info_e           source_of_ue_activity_behaviour_info;
  expected_ue_activity_behaviour_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ExpectedUEBehaviour-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using expected_ue_behaviour_ext_ies_o = protocol_ext_empty_o;

// ExpectedUEMobility ::= ENUMERATED
struct expected_ue_mob_opts {
  enum options { stationary, mobile, /*...*/ nulltype } value;

  const char* to_string() const;
};
using expected_ue_mob_e = enumerated<expected_ue_mob_opts, true>;

// ExpectedUEMovingTrajectory ::= SEQUENCE (SIZE (1..16)) OF ExpectedUEMovingTrajectoryItem
using expected_ue_moving_trajectory_l = dyn_array<expected_ue_moving_trajectory_item_s>;

// CNAssistedRANTuning-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using cn_assisted_ran_tuning_ext_ies_o = protocol_ext_empty_o;

using expected_ue_behaviour_ext_ies_container = protocol_ext_container_empty_l;

// ExpectedUEBehaviour ::= SEQUENCE
struct expected_ue_behaviour_s {
  bool                                    ext                                    = false;
  bool                                    expected_ue_activity_behaviour_present = false;
  bool                                    expected_ho_interv_present             = false;
  bool                                    expected_ue_mob_present                = false;
  bool                                    ie_exts_present                        = false;
  expected_ue_activity_behaviour_s        expected_ue_activity_behaviour;
  expected_ho_interv_e                    expected_ho_interv;
  expected_ue_mob_e                       expected_ue_mob;
  expected_ue_moving_trajectory_l         expected_ue_moving_trajectory;
  expected_ue_behaviour_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using cn_assisted_ran_tuning_ext_ies_container = protocol_ext_container_empty_l;

// CNAssistedRANTuning ::= SEQUENCE
struct cn_assisted_ran_tuning_s {
  bool                                     ext                           = false;
  bool                                     expected_ue_behaviour_present = false;
  bool                                     ie_exts_present               = false;
  expected_ue_behaviour_s                  expected_ue_behaviour;
  cn_assisted_ran_tuning_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CNTypeRestrictionsForEquivalentItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using cn_type_restricts_for_equivalent_item_ext_ies_o = protocol_ext_empty_o;

using cn_type_restricts_for_equivalent_item_ext_ies_container = protocol_ext_container_empty_l;

// CNTypeRestrictionsForEquivalentItem ::= SEQUENCE
struct cn_type_restricts_for_equivalent_item_s {
  struct cn_type_opts {
    enum options { epc_forbidden, five_gc_forbidden, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using cn_type_e_ = enumerated<cn_type_opts, true>;

  // member variables
  bool                                                    ext             = false;
  bool                                                    ie_exts_present = false;
  fixed_octstring<3, true>                                plmn_id;
  cn_type_e_                                              cn_type;
  cn_type_restricts_for_equivalent_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CNTypeRestrictionsForEquivalent ::= SEQUENCE (SIZE (1..15)) OF CNTypeRestrictionsForEquivalentItem
using cn_type_restricts_for_equivalent_l = dyn_array<cn_type_restricts_for_equivalent_item_s>;

// COUNTValueForPDCP-SN12-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using count_value_for_pdcp_sn12_ext_ies_o = protocol_ext_empty_o;

using count_value_for_pdcp_sn12_ext_ies_container = protocol_ext_container_empty_l;

// COUNTValueForPDCP-SN12 ::= SEQUENCE
struct count_value_for_pdcp_sn12_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  uint16_t                                    pdcp_sn12       = 0;
  uint32_t                                    hfn_pdcp_sn12   = 0;
  count_value_for_pdcp_sn12_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// COUNTValueForPDCP-SN18-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using count_value_for_pdcp_sn18_ext_ies_o = protocol_ext_empty_o;

using count_value_for_pdcp_sn18_ext_ies_container = protocol_ext_container_empty_l;

// COUNTValueForPDCP-SN18 ::= SEQUENCE
struct count_value_for_pdcp_sn18_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  uint32_t                                    pdcp_sn18       = 0;
  uint16_t                                    hfn_pdcp_sn18   = 0;
  count_value_for_pdcp_sn18_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CandidateCellID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using candidate_cell_id_ext_ies_o = protocol_ext_empty_o;

// CandidatePCI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using candidate_pci_ext_ies_o = protocol_ext_empty_o;

// CandidateCell-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using candidate_cell_ext_ies_o = protocol_ies_empty_o;

using candidate_cell_id_ext_ies_container = protocol_ext_container_empty_l;

// CandidateCellID ::= SEQUENCE
struct candidate_cell_id_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  nr_cgi_s                            candidate_cell_id;
  candidate_cell_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using candidate_pci_ext_ies_container = protocol_ext_container_empty_l;

// CandidatePCI ::= SEQUENCE
struct candidate_pci_s {
  bool                            ext                = false;
  bool                            ie_exts_present    = false;
  uint16_t                        candidate_pci      = 0;
  uint32_t                        candidate_nr_arfcn = 0;
  candidate_pci_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CandidateCell ::= CHOICE
struct candidate_cell_c {
  struct types_opts {
    enum options { candidate_cgi, candidate_pci, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  candidate_cell_c() = default;
  candidate_cell_c(const candidate_cell_c& other);
  candidate_cell_c& operator=(const candidate_cell_c& other);
  ~candidate_cell_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  candidate_cell_id_s& candidate_cgi()
  {
    assert_choice_type(types::candidate_cgi, type_, "CandidateCell");
    return c.get<candidate_cell_id_s>();
  }
  candidate_pci_s& candidate_pci()
  {
    assert_choice_type(types::candidate_pci, type_, "CandidateCell");
    return c.get<candidate_pci_s>();
  }
  protocol_ie_single_container_s<candidate_cell_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "CandidateCell");
    return c.get<protocol_ie_single_container_s<candidate_cell_ext_ies_o>>();
  }
  const candidate_cell_id_s& candidate_cgi() const
  {
    assert_choice_type(types::candidate_cgi, type_, "CandidateCell");
    return c.get<candidate_cell_id_s>();
  }
  const candidate_pci_s& candidate_pci() const
  {
    assert_choice_type(types::candidate_pci, type_, "CandidateCell");
    return c.get<candidate_pci_s>();
  }
  const protocol_ie_single_container_s<candidate_cell_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "CandidateCell");
    return c.get<protocol_ie_single_container_s<candidate_cell_ext_ies_o>>();
  }
  candidate_cell_id_s&                                      set_candidate_cgi();
  candidate_pci_s&                                          set_candidate_pci();
  protocol_ie_single_container_s<candidate_cell_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                           type_;
  choice_buffer_t<candidate_cell_id_s, candidate_pci_s, protocol_ie_single_container_s<candidate_cell_ext_ies_o>> c;

  void destroy_();
};

// CandidateCellItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using candidate_cell_item_ext_ies_o = protocol_ext_empty_o;

using candidate_cell_item_ext_ies_container = protocol_ext_container_empty_l;

// CandidateCellItem ::= SEQUENCE
struct candidate_cell_item_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  candidate_cell_c                      candidate_cell;
  candidate_cell_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CandidateCellList ::= SEQUENCE (SIZE (1..32)) OF CandidateCellItem
using candidate_cell_list_l = dyn_array<candidate_cell_item_s>;

// Cell-CAGInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using cell_cag_info_ext_ies_o = protocol_ext_empty_o;

// CellCAGList ::= SEQUENCE (SIZE (1..64)) OF BIT STRING (SIZE (32))
using cell_cag_list_l = dyn_array<fixed_bitstring<32, false, true>>;

using cell_cag_info_ext_ies_container = protocol_ext_container_empty_l;

// Cell-CAGInformation ::= SEQUENCE
struct cell_cag_info_s {
  bool                            ext             = false;
  bool                            ie_exts_present = false;
  ngran_cgi_c                     ngran_cgi;
  cell_cag_list_l                 cell_cag_list;
  cell_cag_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellIDListForRestart-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using cell_id_list_for_restart_ext_ies_o = protocol_ies_empty_o;

// EUTRA-CGIList ::= SEQUENCE (SIZE (1..256)) OF EUTRA-CGI
using eutra_cgi_list_l = dyn_array<eutra_cgi_s>;

// NR-CGIList ::= SEQUENCE (SIZE (1..16384)) OF NR-CGI
using nr_cgi_list_l = dyn_array<nr_cgi_s>;

// CellIDListForRestart ::= CHOICE
struct cell_id_list_for_restart_c {
  struct types_opts {
    enum options { eutra_cgi_listfor_restart, nr_cgi_listfor_restart, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  cell_id_list_for_restart_c() = default;
  cell_id_list_for_restart_c(const cell_id_list_for_restart_c& other);
  cell_id_list_for_restart_c& operator=(const cell_id_list_for_restart_c& other);
  ~cell_id_list_for_restart_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_cgi_list_l& eutra_cgi_listfor_restart()
  {
    assert_choice_type(types::eutra_cgi_listfor_restart, type_, "CellIDListForRestart");
    return c.get<eutra_cgi_list_l>();
  }
  nr_cgi_list_l& nr_cgi_listfor_restart()
  {
    assert_choice_type(types::nr_cgi_listfor_restart, type_, "CellIDListForRestart");
    return c.get<nr_cgi_list_l>();
  }
  protocol_ie_single_container_s<cell_id_list_for_restart_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "CellIDListForRestart");
    return c.get<protocol_ie_single_container_s<cell_id_list_for_restart_ext_ies_o>>();
  }
  const eutra_cgi_list_l& eutra_cgi_listfor_restart() const
  {
    assert_choice_type(types::eutra_cgi_listfor_restart, type_, "CellIDListForRestart");
    return c.get<eutra_cgi_list_l>();
  }
  const nr_cgi_list_l& nr_cgi_listfor_restart() const
  {
    assert_choice_type(types::nr_cgi_listfor_restart, type_, "CellIDListForRestart");
    return c.get<nr_cgi_list_l>();
  }
  const protocol_ie_single_container_s<cell_id_list_for_restart_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "CellIDListForRestart");
    return c.get<protocol_ie_single_container_s<cell_id_list_for_restart_ext_ies_o>>();
  }
  eutra_cgi_list_l&                                                   set_eutra_cgi_listfor_restart();
  nr_cgi_list_l&                                                      set_nr_cgi_listfor_restart();
  protocol_ie_single_container_s<cell_id_list_for_restart_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<eutra_cgi_list_l, nr_cgi_list_l, protocol_ie_single_container_s<cell_id_list_for_restart_ext_ies_o>>
      c;

  void destroy_();
};

// PrivacyIndicator ::= ENUMERATED
struct privacy_ind_opts {
  enum options { immediate_mdt, logged_mdt, /*...*/ nulltype } value;

  const char* to_string() const;
};
using privacy_ind_e = enumerated<privacy_ind_opts, true>;

// CellSize ::= ENUMERATED
struct cell_size_opts {
  enum options { verysmall, small, medium, large, /*...*/ nulltype } value;

  const char* to_string() const;
};
using cell_size_e = enumerated<cell_size_opts, true>;

// CellType-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using cell_type_ext_ies_o = protocol_ext_empty_o;

using cell_type_ext_ies_container = protocol_ext_container_empty_l;

// CellType ::= SEQUENCE
struct cell_type_s {
  bool                        ext             = false;
  bool                        ie_exts_present = false;
  cell_size_e                 cell_size;
  cell_type_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellsToActivateList ::= SEQUENCE (SIZE (1..16384)) OF NGRAN-CGI
using cells_to_activ_list_l = dyn_array<ngran_cgi_c>;

// CompositeAvailableCapacity-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using composite_available_capacity_ext_ies_o = protocol_ext_empty_o;

using composite_available_capacity_ext_ies_container = protocol_ext_container_empty_l;

// CompositeAvailableCapacity ::= SEQUENCE
struct composite_available_capacity_s {
  bool                                           ext                               = false;
  bool                                           cell_capacity_class_value_present = false;
  bool                                           ie_exts_present                   = false;
  uint8_t                                        cell_capacity_class_value         = 1;
  uint8_t                                        capacity_value                    = 0;
  composite_available_capacity_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ScheduledCommunicationTime-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using sched_communication_time_ext_ies_o = protocol_ext_empty_o;

// DL-CP-SecurityInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using dl_cp_security_info_ext_ies_o = protocol_ext_empty_o;

using sched_communication_time_ext_ies_container = protocol_ext_container_empty_l;

// ScheduledCommunicationTime ::= SEQUENCE
struct sched_communication_time_s {
  bool                                       ext                      = false;
  bool                                       dayof_week_present       = false;
  bool                                       timeof_day_start_present = false;
  bool                                       timeof_day_end_present   = false;
  bool                                       ie_exts_present          = false;
  fixed_bitstring<7, false, true>            dayof_week;
  uint32_t                                   timeof_day_start = 0;
  uint32_t                                   timeof_day_end   = 0;
  sched_communication_time_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-DifferentiationInfo-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ue_diff_info_ext_ies_o = protocol_ext_empty_o;

// CEmodeBrestricted ::= ENUMERATED
struct ce_mode_brestricted_opts {
  enum options { restricted, not_restricted, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ce_mode_brestricted_e = enumerated<ce_mode_brestricted_opts, true>;

using dl_cp_security_info_ext_ies_container = protocol_ext_container_empty_l;

// DL-CP-SecurityInformation ::= SEQUENCE
struct dl_cp_security_info_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  fixed_bitstring<16, false, true>      dl_nas_mac;
  dl_cp_security_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EndIndication ::= ENUMERATED
struct end_ind_opts {
  enum options { no_further_data, further_data_exists, /*...*/ nulltype } value;

  const char* to_string() const;
};
using end_ind_e = enumerated<end_ind_opts, true>;

// Enhanced-CoverageRestriction ::= ENUMERATED
struct enhanced_coverage_restrict_opts {
  enum options { restricted, /*...*/ nulltype } value;

  const char* to_string() const;
};
using enhanced_coverage_restrict_e = enumerated<enhanced_coverage_restrict_opts, true>;

using ue_diff_info_ext_ies_container = protocol_ext_container_empty_l;

// UE-DifferentiationInfo ::= SEQUENCE
struct ue_diff_info_s {
  struct periodic_communication_ind_opts {
    enum options { periodically, ondemand, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using periodic_communication_ind_e_ = enumerated<periodic_communication_ind_opts, true>;
  struct stationary_ind_opts {
    enum options { stationary, mobile, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using stationary_ind_e_ = enumerated<stationary_ind_opts, true>;
  struct traffic_profile_opts {
    enum options { single_packet, dual_packets, multiple_packets, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using traffic_profile_e_ = enumerated<traffic_profile_opts, true>;
  struct battery_ind_opts {
    enum options {
      battery_pwred,
      battery_pwred_not_rechargeable_or_replaceable,
      not_battery_pwred,
      /*...*/ nulltype
    } value;

    const char* to_string() const;
  };
  using battery_ind_e_ = enumerated<battery_ind_opts, true>;

  // member variables
  bool                           ext                                = false;
  bool                           periodic_communication_ind_present = false;
  bool                           periodic_time_present              = false;
  bool                           sched_communication_time_present   = false;
  bool                           stationary_ind_present             = false;
  bool                           traffic_profile_present            = false;
  bool                           battery_ind_present                = false;
  bool                           ie_exts_present                    = false;
  periodic_communication_ind_e_  periodic_communication_ind;
  uint16_t                       periodic_time = 1;
  sched_communication_time_s     sched_communication_time;
  stationary_ind_e_              stationary_ind;
  traffic_profile_e_             traffic_profile;
  battery_ind_e_                 battery_ind;
  ue_diff_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-Paging-Time-Window ::= ENUMERATED
struct eutra_paging_time_win_opts {
  enum options { s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using eutra_paging_time_win_e = enumerated<eutra_paging_time_win_opts, true>;

// EUTRA-Paging-eDRX-Cycle ::= ENUMERATED
struct eutra_paging_e_drx_cycle_opts {
  enum options {
    hfhalf,
    hf1,
    hf2,
    hf4,
    hf6,
    hf8,
    hf10,
    hf12,
    hf14,
    hf16,
    hf32,
    hf64,
    hf128,
    hf256,
    /*...*/ nulltype
  } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using eutra_paging_e_drx_cycle_e = enumerated<eutra_paging_e_drx_cycle_opts, true>;

// EUTRA-PagingeDRXInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using eutra_paginge_drx_info_ext_ies_o = protocol_ext_empty_o;

// NR-Paging-Time-Window ::= ENUMERATED
struct nr_paging_time_win_opts {
  enum options {
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
    s11,
    s12,
    s13,
    s14,
    s15,
    s16,
    // ...
    s17,
    s18,
    s19,
    s20,
    s21,
    s22,
    s23,
    s24,
    s25,
    s26,
    s27,
    s28,
    s29,
    s30,
    s31,
    s32,
    nulltype
  } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using nr_paging_time_win_e = enumerated<nr_paging_time_win_opts, true, 16>;

// NR-Paging-eDRX-Cycle ::= ENUMERATED
struct nr_paging_e_drx_cycle_opts {
  enum options {
    hfquarter,
    hfhalf,
    hf1,
    hf2,
    hf4,
    hf8,
    hf16,
    hf32,
    hf64,
    hf128,
    hf256,
    hf512,
    hf1024,
    /*...*/ nulltype
  } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using nr_paging_e_drx_cycle_e = enumerated<nr_paging_e_drx_cycle_opts, true>;

// NR-PagingeDRXInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using nr_paginge_drx_info_ext_ies_o = protocol_ext_empty_o;

// PEIPSassistanceInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pe_ip_sassist_info_ext_ies_o = protocol_ext_empty_o;

// TAIListForInactiveItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using tai_list_for_inactive_item_ext_ies_o = protocol_ext_empty_o;

// UERadioCapabilityForPaging-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct ue_radio_cap_for_paging_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { ue_radio_cap_for_paging_of_nb_iot, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ue_radio_cap_for_paging_of_nb_iot; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    unbounded_octstring<true>&       ue_radio_cap_for_paging_of_nb_iot() { return c; }
    const unbounded_octstring<true>& ue_radio_cap_for_paging_of_nb_iot() const { return c; }

  private:
    unbounded_octstring<true> c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using eutra_paginge_drx_info_ext_ies_container = protocol_ext_container_empty_l;

// EUTRA-PagingeDRXInformation ::= SEQUENCE
struct eutra_paginge_drx_info_s {
  bool                                     ext                           = false;
  bool                                     eutra_paging_time_win_present = false;
  bool                                     ie_exts_present               = false;
  eutra_paging_e_drx_cycle_e               eutra_paging_e_drx_cycle;
  eutra_paging_time_win_e                  eutra_paging_time_win;
  eutra_paginge_drx_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MicoAllPLMN ::= ENUMERATED
struct mico_all_plmn_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using mico_all_plmn_e = enumerated<mico_all_plmn_opts, true>;

using nr_paginge_drx_info_ext_ies_container = protocol_ext_container_empty_l;

// NR-PagingeDRXInformation ::= SEQUENCE
struct nr_paginge_drx_info_s {
  bool                                  ext                        = false;
  bool                                  nr_paging_time_win_present = false;
  bool                                  ie_exts_present            = false;
  nr_paging_e_drx_cycle_e               nr_paging_e_drx_cycle;
  nr_paging_time_win_e                  nr_paging_time_win;
  nr_paginge_drx_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pe_ip_sassist_info_ext_ies_container = protocol_ext_container_empty_l;

// PEIPSassistanceInformation ::= SEQUENCE
struct pe_ip_sassist_info_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  uint8_t                              cnsubgroup_id   = 0;
  pe_ip_sassist_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingCauseIndicationForVoiceService ::= ENUMERATED
struct paging_cause_ind_for_voice_service_opts {
  enum options { supported, /*...*/ nulltype } value;

  const char* to_string() const;
};
using paging_cause_ind_for_voice_service_e = enumerated<paging_cause_ind_for_voice_service_opts, true>;

using tai_list_for_inactive_item_ext_ies_container = protocol_ext_container_empty_l;

// TAIListForInactiveItem ::= SEQUENCE
struct tai_list_for_inactive_item_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  tai_s                                        tai;
  tai_list_for_inactive_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEIdentityIndexValue-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using ue_id_idx_value_ext_ies_o = protocol_ies_empty_o;

// UERadioCapabilityForPaging ::= SEQUENCE
struct ue_radio_cap_for_paging_s {
  bool                                                        ext = false;
  unbounded_octstring<true>                                   ue_radio_cap_for_paging_of_nr;
  unbounded_octstring<true>                                   ue_radio_cap_for_paging_of_eutra;
  protocol_ext_container_l<ue_radio_cap_for_paging_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CoreNetworkAssistanceInformationForInactive-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct core_network_assist_info_for_inactive_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        eutra_paginge_drx_info,
        extended_ue_id_idx_value,
        ue_radio_cap_for_paging,
        mico_all_plmn,
        nr_paginge_drx_info,
        paging_cause_ind_for_voice_service,
        pe_ip_sassist_info,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    eutra_paginge_drx_info_s&                   eutra_paginge_drx_info();
    fixed_bitstring<16, false, true>&           extended_ue_id_idx_value();
    ue_radio_cap_for_paging_s&                  ue_radio_cap_for_paging();
    mico_all_plmn_e&                            mico_all_plmn();
    nr_paginge_drx_info_s&                      nr_paginge_drx_info();
    paging_cause_ind_for_voice_service_e&       paging_cause_ind_for_voice_service();
    pe_ip_sassist_info_s&                       pe_ip_sassist_info();
    const eutra_paginge_drx_info_s&             eutra_paginge_drx_info() const;
    const fixed_bitstring<16, false, true>&     extended_ue_id_idx_value() const;
    const ue_radio_cap_for_paging_s&            ue_radio_cap_for_paging() const;
    const mico_all_plmn_e&                      mico_all_plmn() const;
    const nr_paginge_drx_info_s&                nr_paginge_drx_info() const;
    const paging_cause_ind_for_voice_service_e& paging_cause_ind_for_voice_service() const;
    const pe_ip_sassist_info_s&                 pe_ip_sassist_info() const;

  private:
    types type_;
    choice_buffer_t<eutra_paginge_drx_info_s,
                    fixed_bitstring<16, false, true>,
                    nr_paginge_drx_info_s,
                    pe_ip_sassist_info_s,
                    ue_radio_cap_for_paging_s>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MICOModeIndication ::= ENUMERATED
struct mico_mode_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using mico_mode_ind_e = enumerated<mico_mode_ind_opts, true>;

// PagingDRX ::= ENUMERATED
struct paging_drx_opts {
  enum options { v32, v64, v128, v256, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using paging_drx_e = enumerated<paging_drx_opts, true>;

// TAIListForInactive ::= SEQUENCE (SIZE (1..16)) OF TAIListForInactiveItem
using tai_list_for_inactive_l = dyn_array<tai_list_for_inactive_item_s>;

// UEIdentityIndexValue ::= CHOICE
struct ue_id_idx_value_c {
  struct types_opts {
    enum options { idx_len10, choice_exts, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ue_id_idx_value_c() = default;
  ue_id_idx_value_c(const ue_id_idx_value_c& other);
  ue_id_idx_value_c& operator=(const ue_id_idx_value_c& other);
  ~ue_id_idx_value_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<10, false, true>& idx_len10()
  {
    assert_choice_type(types::idx_len10, type_, "UEIdentityIndexValue");
    return c.get<fixed_bitstring<10, false, true>>();
  }
  protocol_ie_single_container_s<ue_id_idx_value_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "UEIdentityIndexValue");
    return c.get<protocol_ie_single_container_s<ue_id_idx_value_ext_ies_o>>();
  }
  const fixed_bitstring<10, false, true>& idx_len10() const
  {
    assert_choice_type(types::idx_len10, type_, "UEIdentityIndexValue");
    return c.get<fixed_bitstring<10, false, true>>();
  }
  const protocol_ie_single_container_s<ue_id_idx_value_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "UEIdentityIndexValue");
    return c.get<protocol_ie_single_container_s<ue_id_idx_value_ext_ies_o>>();
  }
  fixed_bitstring<10, false, true>&                          set_idx_len10();
  protocol_ie_single_container_s<ue_id_idx_value_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                        type_;
  choice_buffer_t<fixed_bitstring<10, false, true>, protocol_ie_single_container_s<ue_id_idx_value_ext_ies_o>> c;

  void destroy_();
};

struct core_network_assist_info_for_inactive_ext_ies_container {
  bool                                 eutra_paginge_drx_info_present             = false;
  bool                                 extended_ue_id_idx_value_present           = false;
  bool                                 ue_radio_cap_for_paging_present            = false;
  bool                                 mico_all_plmn_present                      = false;
  bool                                 nr_paginge_drx_info_present                = false;
  bool                                 paging_cause_ind_for_voice_service_present = false;
  bool                                 pe_ip_sassist_info_present                 = false;
  eutra_paginge_drx_info_s             eutra_paginge_drx_info;
  fixed_bitstring<16, false, true>     extended_ue_id_idx_value;
  ue_radio_cap_for_paging_s            ue_radio_cap_for_paging;
  mico_all_plmn_e                      mico_all_plmn;
  nr_paginge_drx_info_s                nr_paginge_drx_info;
  paging_cause_ind_for_voice_service_e paging_cause_ind_for_voice_service;
  pe_ip_sassist_info_s                 pe_ip_sassist_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CoreNetworkAssistanceInformationForInactive ::= SEQUENCE
struct core_network_assist_info_for_inactive_s {
  bool                                                    ext                           = false;
  bool                                                    ue_specific_drx_present       = false;
  bool                                                    mico_mode_ind_present         = false;
  bool                                                    expected_ue_behaviour_present = false;
  bool                                                    ie_exts_present               = false;
  ue_id_idx_value_c                                       ue_id_idx_value;
  paging_drx_e                                            ue_specific_drx;
  fixed_bitstring<8, false, true>                         periodic_regist_upd_timer;
  mico_mode_ind_e                                         mico_mode_ind;
  tai_list_for_inactive_l                                 tai_list_for_inactive;
  expected_ue_behaviour_s                                 expected_ue_behaviour;
  core_network_assist_info_for_inactive_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DAPSRequestInfo-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using daps_request_info_ext_ies_o = protocol_ext_empty_o;

using daps_request_info_ext_ies_container = protocol_ext_container_empty_l;

// DAPSRequestInfo ::= SEQUENCE
struct daps_request_info_s {
  struct da_psi_ndicator_opts {
    enum options { daps_ho_required, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using da_psi_ndicator_e_ = enumerated<da_psi_ndicator_opts, true>;

  // member variables
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  da_psi_ndicator_e_                  da_psi_ndicator;
  daps_request_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DAPSResponseInfo-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using daps_resp_info_ext_ies_o = protocol_ext_empty_o;

using daps_resp_info_ext_ies_container = protocol_ext_container_empty_l;

// DAPSResponseInfo ::= SEQUENCE
struct daps_resp_info_s {
  struct dapsrespind_opts {
    enum options { daps_ho_accepted, daps_ho_not_accepted, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using dapsrespind_e_ = enumerated<dapsrespind_opts, true>;

  // member variables
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  dapsrespind_e_                   dapsrespind;
  daps_resp_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DAPSResponseInfoItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using daps_resp_info_item_ext_ies_o = protocol_ext_empty_o;

using daps_resp_info_item_ext_ies_container = protocol_ext_container_empty_l;

// DAPSResponseInfoItem ::= SEQUENCE
struct daps_resp_info_item_s {
  bool                                  ext            = false;
  bool                                  ie_ext_present = false;
  uint8_t                               drb_id         = 1;
  daps_resp_info_s                      daps_resp_info;
  daps_resp_info_item_ext_ies_container ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DAPSResponseInfoList ::= SEQUENCE (SIZE (1..32)) OF DAPSResponseInfoItem
using daps_resp_info_list_l = dyn_array<daps_resp_info_item_s>;

// DRBStatusDL12-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using drb_status_dl12_ext_ies_o = protocol_ext_empty_o;

// DRBStatusDL18-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using drb_status_dl18_ext_ies_o = protocol_ext_empty_o;

// DRBStatusDL-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using drb_status_dl_ext_ies_o = protocol_ies_empty_o;

using drb_status_dl12_ext_ies_container = protocol_ext_container_empty_l;

// DRBStatusDL12 ::= SEQUENCE
struct drb_status_dl12_s {
  bool                              ext            = false;
  bool                              ie_ext_present = false;
  count_value_for_pdcp_sn12_s       dl_count_value;
  drb_status_dl12_ext_ies_container ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_status_dl18_ext_ies_container = protocol_ext_container_empty_l;

// DRBStatusDL18 ::= SEQUENCE
struct drb_status_dl18_s {
  bool                              ext            = false;
  bool                              ie_ext_present = false;
  count_value_for_pdcp_sn18_s       dl_count_value;
  drb_status_dl18_ext_ies_container ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBStatusDL ::= CHOICE
struct drb_status_dl_c {
  struct types_opts {
    enum options { drb_status_dl12, drb_status_dl18, choice_exts, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  drb_status_dl_c() = default;
  drb_status_dl_c(const drb_status_dl_c& other);
  drb_status_dl_c& operator=(const drb_status_dl_c& other);
  ~drb_status_dl_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  drb_status_dl12_s& drb_status_dl12()
  {
    assert_choice_type(types::drb_status_dl12, type_, "DRBStatusDL");
    return c.get<drb_status_dl12_s>();
  }
  drb_status_dl18_s& drb_status_dl18()
  {
    assert_choice_type(types::drb_status_dl18, type_, "DRBStatusDL");
    return c.get<drb_status_dl18_s>();
  }
  protocol_ie_single_container_s<drb_status_dl_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "DRBStatusDL");
    return c.get<protocol_ie_single_container_s<drb_status_dl_ext_ies_o>>();
  }
  const drb_status_dl12_s& drb_status_dl12() const
  {
    assert_choice_type(types::drb_status_dl12, type_, "DRBStatusDL");
    return c.get<drb_status_dl12_s>();
  }
  const drb_status_dl18_s& drb_status_dl18() const
  {
    assert_choice_type(types::drb_status_dl18, type_, "DRBStatusDL");
    return c.get<drb_status_dl18_s>();
  }
  const protocol_ie_single_container_s<drb_status_dl_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "DRBStatusDL");
    return c.get<protocol_ie_single_container_s<drb_status_dl_ext_ies_o>>();
  }
  drb_status_dl12_s&                                       set_drb_status_dl12();
  drb_status_dl18_s&                                       set_drb_status_dl18();
  protocol_ie_single_container_s<drb_status_dl_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                          type_;
  choice_buffer_t<drb_status_dl12_s, drb_status_dl18_s, protocol_ie_single_container_s<drb_status_dl_ext_ies_o>> c;

  void destroy_();
};

// DRBStatusUL12-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using drb_status_ul12_ext_ies_o = protocol_ext_empty_o;

// DRBStatusUL18-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using drb_status_ul18_ext_ies_o = protocol_ext_empty_o;

// DRBStatusUL-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using drb_status_ul_ext_ies_o = protocol_ies_empty_o;

using drb_status_ul12_ext_ies_container = protocol_ext_container_empty_l;

// DRBStatusUL12 ::= SEQUENCE
struct drb_status_ul12_s {
  bool                                    ext                                    = false;
  bool                                    receive_status_of_ul_pdcp_sdus_present = false;
  bool                                    ie_ext_present                         = false;
  count_value_for_pdcp_sn12_s             ul_count_value;
  bounded_bitstring<1, 2048, false, true> receive_status_of_ul_pdcp_sdus;
  drb_status_ul12_ext_ies_container       ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using drb_status_ul18_ext_ies_container = protocol_ext_container_empty_l;

// DRBStatusUL18 ::= SEQUENCE
struct drb_status_ul18_s {
  bool                                      ext                                    = false;
  bool                                      receive_status_of_ul_pdcp_sdus_present = false;
  bool                                      ie_ext_present                         = false;
  count_value_for_pdcp_sn18_s               ul_count_value;
  bounded_bitstring<1, 131072, false, true> receive_status_of_ul_pdcp_sdus;
  drb_status_ul18_ext_ies_container         ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBStatusUL ::= CHOICE
struct drb_status_ul_c {
  struct types_opts {
    enum options { drb_status_ul12, drb_status_ul18, choice_exts, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  drb_status_ul_c() = default;
  drb_status_ul_c(const drb_status_ul_c& other);
  drb_status_ul_c& operator=(const drb_status_ul_c& other);
  ~drb_status_ul_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  drb_status_ul12_s& drb_status_ul12()
  {
    assert_choice_type(types::drb_status_ul12, type_, "DRBStatusUL");
    return c.get<drb_status_ul12_s>();
  }
  drb_status_ul18_s& drb_status_ul18()
  {
    assert_choice_type(types::drb_status_ul18, type_, "DRBStatusUL");
    return c.get<drb_status_ul18_s>();
  }
  protocol_ie_single_container_s<drb_status_ul_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "DRBStatusUL");
    return c.get<protocol_ie_single_container_s<drb_status_ul_ext_ies_o>>();
  }
  const drb_status_ul12_s& drb_status_ul12() const
  {
    assert_choice_type(types::drb_status_ul12, type_, "DRBStatusUL");
    return c.get<drb_status_ul12_s>();
  }
  const drb_status_ul18_s& drb_status_ul18() const
  {
    assert_choice_type(types::drb_status_ul18, type_, "DRBStatusUL");
    return c.get<drb_status_ul18_s>();
  }
  const protocol_ie_single_container_s<drb_status_ul_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "DRBStatusUL");
    return c.get<protocol_ie_single_container_s<drb_status_ul_ext_ies_o>>();
  }
  drb_status_ul12_s&                                       set_drb_status_ul12();
  drb_status_ul18_s&                                       set_drb_status_ul18();
  protocol_ie_single_container_s<drb_status_ul_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                          type_;
  choice_buffer_t<drb_status_ul12_s, drb_status_ul18_s, protocol_ie_single_container_s<drb_status_ul_ext_ies_o>> c;

  void destroy_();
};

// DRBsSubjectToEarlyStatusTransfer-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using drbs_subject_to_early_status_transfer_item_ext_ies_o = protocol_ext_empty_o;

using drbs_subject_to_early_status_transfer_item_ext_ies_container = protocol_ext_container_empty_l;

// DRBsSubjectToEarlyStatusTransfer-Item ::= SEQUENCE
struct drbs_subject_to_early_status_transfer_item_s {
  bool                                                         ext            = false;
  bool                                                         ie_ext_present = false;
  uint8_t                                                      drb_id         = 1;
  drb_status_dl_c                                              first_dl_count;
  drbs_subject_to_early_status_transfer_item_ext_ies_container ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBsSubjectToEarlyStatusTransfer-List ::= SEQUENCE (SIZE (1..32)) OF DRBsSubjectToEarlyStatusTransfer-Item
using drbs_subject_to_early_status_transfer_list_l = dyn_array<drbs_subject_to_early_status_transfer_item_s>;

// DRBsSubjectToStatusTransferItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct drbs_subject_to_status_transfer_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { old_associated_qos_flow_list_ul_endmarkerexpected, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::old_associated_qos_flow_list_ul_endmarkerexpected; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    associated_qos_flow_list_l&       old_associated_qos_flow_list_ul_endmarkerexpected() { return c; }
    const associated_qos_flow_list_l& old_associated_qos_flow_list_ul_endmarkerexpected() const { return c; }

  private:
    associated_qos_flow_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBsSubjectToStatusTransferItem ::= SEQUENCE
struct drbs_subject_to_status_transfer_item_s {
  bool                                                                     ext    = false;
  uint8_t                                                                  drb_id = 1;
  drb_status_ul_c                                                          drb_status_ul;
  drb_status_dl_c                                                          drb_status_dl;
  protocol_ext_container_l<drbs_subject_to_status_transfer_item_ext_ies_o> ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBsSubjectToStatusTransferList ::= SEQUENCE (SIZE (1..32)) OF DRBsSubjectToStatusTransferItem
using drbs_subject_to_status_transfer_list_l = dyn_array<drbs_subject_to_status_transfer_item_s>;

// DRBsToQosFlowsMappingItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct drbs_to_qos_flows_map_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { daps_request_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::daps_request_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    daps_request_info_s&       daps_request_info() { return c; }
    const daps_request_info_s& daps_request_info() const { return c; }

  private:
    daps_request_info_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBsToQosFlowsMappingItem ::= SEQUENCE
struct drbs_to_qos_flows_map_item_s {
  bool                                                           ext    = false;
  uint8_t                                                        drb_id = 1;
  associated_qos_flow_list_l                                     associated_qos_flow_list;
  protocol_ext_container_l<drbs_to_qos_flows_map_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRBsToQosFlowsMappingList ::= SEQUENCE (SIZE (1..32)) OF DRBsToQosFlowsMappingItem
using drbs_to_qos_flows_map_list_l = dyn_array<drbs_to_qos_flows_map_item_s>;

// DataForwardingResponseDRBItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using data_forwarding_resp_drb_item_ext_ies_o = protocol_ext_empty_o;

using data_forwarding_resp_drb_item_ext_ies_container = protocol_ext_container_empty_l;

// DataForwardingResponseDRBItem ::= SEQUENCE
struct data_forwarding_resp_drb_item_s {
  bool                                            ext                               = false;
  bool                                            dl_forwarding_up_tnl_info_present = false;
  bool                                            ul_forwarding_up_tnl_info_present = false;
  bool                                            ie_exts_present                   = false;
  uint8_t                                         drb_id                            = 1;
  up_transport_layer_info_c                       dl_forwarding_up_tnl_info;
  up_transport_layer_info_c                       ul_forwarding_up_tnl_info;
  data_forwarding_resp_drb_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DataForwardingResponseDRBList ::= SEQUENCE (SIZE (1..32)) OF DataForwardingResponseDRBItem
using data_forwarding_resp_drb_list_l = dyn_array<data_forwarding_resp_drb_item_s>;

// DataForwardingResponseERABListItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using data_forwarding_resp_erab_list_item_ext_ies_o = protocol_ext_empty_o;

using data_forwarding_resp_erab_list_item_ext_ies_container = protocol_ext_container_empty_l;

// DataForwardingResponseERABListItem ::= SEQUENCE
struct data_forwarding_resp_erab_list_item_s {
  bool                                                  ext             = false;
  bool                                                  ie_exts_present = false;
  uint8_t                                               erab_id         = 0;
  up_transport_layer_info_c                             dl_forwarding_up_tnl_info;
  data_forwarding_resp_erab_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DataForwardingResponseERABList ::= SEQUENCE (SIZE (1..256)) OF DataForwardingResponseERABListItem
using data_forwarding_resp_erab_list_l = dyn_array<data_forwarding_resp_erab_list_item_s>;

// ExtendedRATRestrictionInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using extended_rat_restrict_info_ext_ies_o = protocol_ext_empty_o;

using extended_rat_restrict_info_ext_ies_container = protocol_ext_container_empty_l;

// ExtendedRATRestrictionInformation ::= SEQUENCE
struct extended_rat_restrict_info_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  fixed_bitstring<8, true, true>               primary_rat_restrict;
  fixed_bitstring<8, true, true>               secondary_rat_restrict;
  extended_rat_restrict_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PNI-NPN-MobilityInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pni_npn_mob_info_ext_ies_o = protocol_ext_empty_o;

// SNPN-MobilityInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using sn_pn_mob_info_ext_ies_o = protocol_ext_empty_o;

// ForbiddenAreaInformation-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using forbidden_area_info_item_ext_ies_o = protocol_ext_empty_o;

// ForbiddenTACs ::= SEQUENCE (SIZE (1..4096)) OF OCTET STRING (SIZE (3))
using forbidden_tacs_l = dyn_array<fixed_octstring<3, true>>;

// NPN-MobilityInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using npn_mob_info_ext_ies_o = protocol_ies_empty_o;

// NotAllowedTACs ::= SEQUENCE (SIZE (1..16)) OF OCTET STRING (SIZE (3))
using not_allowed_tacs_l = bounded_array<fixed_octstring<3, true>, 16>;

using pni_npn_mob_info_ext_ies_container = protocol_ext_container_empty_l;

// PNI-NPN-MobilityInformation ::= SEQUENCE
struct pni_npn_mob_info_s {
  bool                               ext             = false;
  bool                               ie_exts_present = false;
  allowed_pni_npn_list_l             allowed_pni_npi_list;
  pni_npn_mob_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RATRestrictions-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct rat_restricts_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { extended_rat_restrict_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::extended_rat_restrict_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    extended_rat_restrict_info_s&       extended_rat_restrict_info() { return c; }
    const extended_rat_restrict_info_s& extended_rat_restrict_info() const { return c; }

  private:
    extended_rat_restrict_info_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using sn_pn_mob_info_ext_ies_container = protocol_ext_container_empty_l;

// SNPN-MobilityInformation ::= SEQUENCE
struct sn_pn_mob_info_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_bitstring<44, false, true> serving_n_id;
  sn_pn_mob_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServiceAreaInformation-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using service_area_info_item_ext_ies_o = protocol_ext_empty_o;

// TargetNSSAI-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using target_nssai_item_ext_ies_o = protocol_ext_empty_o;

// CNTypeRestrictionsForServing ::= ENUMERATED
struct cn_type_restricts_for_serving_opts {
  enum options { epc_forbidden, /*...*/ nulltype } value;

  const char* to_string() const;
};
using cn_type_restricts_for_serving_e = enumerated<cn_type_restricts_for_serving_opts, true>;

using forbidden_area_info_item_ext_ies_container = protocol_ext_container_empty_l;

// ForbiddenAreaInformation-Item ::= SEQUENCE
struct forbidden_area_info_item_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  fixed_octstring<3, true>                   plmn_id;
  forbidden_tacs_l                           forbidden_tacs;
  forbidden_area_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPN-MobilityInformation ::= CHOICE
struct npn_mob_info_c {
  struct types_opts {
    enum options { sn_pn_mob_info, pni_npn_mob_info, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  npn_mob_info_c() = default;
  npn_mob_info_c(const npn_mob_info_c& other);
  npn_mob_info_c& operator=(const npn_mob_info_c& other);
  ~npn_mob_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  sn_pn_mob_info_s& sn_pn_mob_info()
  {
    assert_choice_type(types::sn_pn_mob_info, type_, "NPN-MobilityInformation");
    return c.get<sn_pn_mob_info_s>();
  }
  pni_npn_mob_info_s& pni_npn_mob_info()
  {
    assert_choice_type(types::pni_npn_mob_info, type_, "NPN-MobilityInformation");
    return c.get<pni_npn_mob_info_s>();
  }
  protocol_ie_single_container_s<npn_mob_info_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "NPN-MobilityInformation");
    return c.get<protocol_ie_single_container_s<npn_mob_info_ext_ies_o>>();
  }
  const sn_pn_mob_info_s& sn_pn_mob_info() const
  {
    assert_choice_type(types::sn_pn_mob_info, type_, "NPN-MobilityInformation");
    return c.get<sn_pn_mob_info_s>();
  }
  const pni_npn_mob_info_s& pni_npn_mob_info() const
  {
    assert_choice_type(types::pni_npn_mob_info, type_, "NPN-MobilityInformation");
    return c.get<pni_npn_mob_info_s>();
  }
  const protocol_ie_single_container_s<npn_mob_info_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "NPN-MobilityInformation");
    return c.get<protocol_ie_single_container_s<npn_mob_info_ext_ies_o>>();
  }
  sn_pn_mob_info_s&                                       set_sn_pn_mob_info();
  pni_npn_mob_info_s&                                     set_pni_npn_mob_info();
  protocol_ie_single_container_s<npn_mob_info_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                         type_;
  choice_buffer_t<pni_npn_mob_info_s, protocol_ie_single_container_s<npn_mob_info_ext_ies_o>, sn_pn_mob_info_s> c;

  void destroy_();
};

// RATRestrictions-Item ::= SEQUENCE
struct rat_restricts_item_s {
  bool                                                   ext = false;
  fixed_octstring<3, true>                               plmn_id;
  fixed_bitstring<8, true, true>                         rat_restrict_info;
  protocol_ext_container_l<rat_restricts_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using service_area_info_item_ext_ies_container = protocol_ext_container_empty_l;

// ServiceAreaInformation-Item ::= SEQUENCE
struct service_area_info_item_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  fixed_octstring<3, true>                 plmn_id;
  allowed_tacs_l                           allowed_tacs;
  not_allowed_tacs_l                       not_allowed_tacs;
  service_area_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using target_nssai_item_ext_ies_container = protocol_ext_container_empty_l;

// TargetNSSAI-Item ::= SEQUENCE
struct target_nssai_item_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  s_nssai_s                           s_nssai;
  target_nssai_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EquivalentPLMNs ::= SEQUENCE (SIZE (1..15)) OF OCTET STRING (SIZE (3))
using equivalent_plmns_l = bounded_array<fixed_octstring<3, true>, 15>;

// ForbiddenAreaInformation ::= SEQUENCE (SIZE (1..16)) OF ForbiddenAreaInformation-Item
using forbidden_area_info_l = dyn_array<forbidden_area_info_item_s>;

// MobilityRestrictionList-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct mob_restrict_list_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        last_eutran_plmn_id,
        cn_type_restricts_for_serving,
        cn_type_restricts_for_equivalent,
        npn_mob_info,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_octstring<3, true>&                 last_eutran_plmn_id();
    cn_type_restricts_for_serving_e&          cn_type_restricts_for_serving();
    cn_type_restricts_for_equivalent_l&       cn_type_restricts_for_equivalent();
    npn_mob_info_c&                           npn_mob_info();
    const fixed_octstring<3, true>&           last_eutran_plmn_id() const;
    const cn_type_restricts_for_serving_e&    cn_type_restricts_for_serving() const;
    const cn_type_restricts_for_equivalent_l& cn_type_restricts_for_equivalent() const;
    const npn_mob_info_c&                     npn_mob_info() const;

  private:
    types                                                                                         type_;
    choice_buffer_t<cn_type_restricts_for_equivalent_l, fixed_octstring<3, true>, npn_mob_info_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RATRestrictions ::= SEQUENCE (SIZE (1..16)) OF RATRestrictions-Item
using rat_restricts_l = dyn_array<rat_restricts_item_s>;

// ServiceAreaInformation ::= SEQUENCE (SIZE (1..16)) OF ServiceAreaInformation-Item
using service_area_info_l = dyn_array<service_area_info_item_s>;

// TargetNSSAI ::= SEQUENCE (SIZE (1..8)) OF TargetNSSAI-Item
using target_nssai_l = dyn_array<target_nssai_item_s>;

// TargetNSSAIInformation-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using target_nssai_info_item_ext_ies_o = protocol_ext_empty_o;

// UEAggregateMaximumBitRate-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ue_aggr_max_bit_rate_ext_ies_o = protocol_ext_empty_o;

struct mob_restrict_list_ext_ies_container {
  bool                               last_eutran_plmn_id_present              = false;
  bool                               cn_type_restricts_for_serving_present    = false;
  bool                               cn_type_restricts_for_equivalent_present = false;
  bool                               npn_mob_info_present                     = false;
  fixed_octstring<3, true>           last_eutran_plmn_id;
  cn_type_restricts_for_serving_e    cn_type_restricts_for_serving;
  cn_type_restricts_for_equivalent_l cn_type_restricts_for_equivalent;
  npn_mob_info_c                     npn_mob_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityRestrictionList ::= SEQUENCE
struct mob_restrict_list_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  fixed_octstring<3, true>            serving_plmn;
  equivalent_plmns_l                  equivalent_plmns;
  rat_restricts_l                     rat_restricts;
  forbidden_area_info_l               forbidden_area_info;
  service_area_info_l                 service_area_info;
  mob_restrict_list_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRVCCOperationPossible ::= ENUMERATED
struct srvcc_operation_possible_opts {
  enum options { possible, not_possible, /*...*/ nulltype } value;

  const char* to_string() const;
};
using srvcc_operation_possible_e = enumerated<srvcc_operation_possible_opts, true>;

using target_nssai_info_item_ext_ies_container = protocol_ext_container_empty_l;

// TargetNSSAIInformation ::= SEQUENCE
struct target_nssai_info_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  target_nssai_l                           target_nssai;
  uint16_t                                 idx_to_rfsp = 1;
  target_nssai_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ue_aggr_max_bit_rate_ext_ies_container = protocol_ext_container_empty_l;

// UEAggregateMaximumBitRate ::= SEQUENCE
struct ue_aggr_max_bit_rate_s {
  bool                                   ext                     = false;
  bool                                   ie_exts_present         = false;
  uint64_t                               ue_aggr_max_bit_rate_dl = 0;
  uint64_t                               ue_aggr_max_bit_rate_ul = 0;
  ue_aggr_max_bit_rate_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityInfoRequest ::= ENUMERATED
struct ue_cap_info_request_opts {
  enum options { requested, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ue_cap_info_request_e = enumerated<ue_cap_info_request_opts, true>;

// EUTRAN-CompositeAvailableCapacityGroup-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using eutran_composite_available_capacity_group_ext_ies_o = protocol_ext_empty_o;

// EUTRAN-RadioResourceStatus-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using eutran_rr_status_ext_ies_o = protocol_ext_empty_o;

// NGRAN-RadioResourceStatus-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ngran_rr_status_ext_ies_o = protocol_ext_empty_o;

// EUTRAN-CellReportItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using eutran_cell_report_item_ext_ies_o = protocol_ext_empty_o;

// EUTRAN-CellToReportItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using eutran_cell_to_report_item_ext_ies_o = protocol_ext_empty_o;

using eutran_composite_available_capacity_group_ext_ies_container = protocol_ext_container_empty_l;

// EUTRAN-CompositeAvailableCapacityGroup ::= SEQUENCE
struct eutran_composite_available_capacity_group_s {
  bool                                                        ext             = false;
  bool                                                        ie_exts_present = false;
  composite_available_capacity_s                              dl_composite_available_capacity;
  composite_available_capacity_s                              ul_composite_available_capacity;
  eutran_composite_available_capacity_group_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using eutran_rr_status_ext_ies_container = protocol_ext_container_empty_l;

// EUTRAN-RadioResourceStatus ::= SEQUENCE
struct eutran_rr_status_s {
  bool                               ext                              = false;
  bool                               dl_sched_pdcch_cce_usage_present = false;
  bool                               ul_sched_pdcch_cce_usage_present = false;
  bool                               ie_exts_present                  = false;
  uint8_t                            dl_gbr_prb_usage                 = 0;
  uint8_t                            ul_gbr_prb_usage                 = 0;
  uint8_t                            dl_non_gbr_prb_usage             = 0;
  uint8_t                            ul_non_gbr_prb_usage             = 0;
  uint8_t                            dl_total_prb_usage               = 0;
  uint8_t                            ul_total_prb_usage               = 0;
  uint8_t                            dl_sched_pdcch_cce_usage         = 0;
  uint8_t                            ul_sched_pdcch_cce_usage         = 0;
  eutran_rr_status_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGRAN-CellReportItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ngran_cell_report_item_ext_ies_o = protocol_ext_empty_o;

// NGRAN-CellToReportItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ngran_cell_to_report_item_ext_ies_o = protocol_ext_empty_o;

using ngran_rr_status_ext_ies_container = protocol_ext_container_empty_l;

// NGRAN-RadioResourceStatus ::= SEQUENCE
struct ngran_rr_status_s {
  bool                              ext                           = false;
  bool                              ie_exts_present               = false;
  uint8_t                           dl_gbr_prb_usage_for_mimo     = 0;
  uint8_t                           ul_gbr_prb_usage_for_mimo     = 0;
  uint8_t                           dl_non_gbr_prb_usage_for_mimo = 0;
  uint8_t                           ul_non_gbr_prb_usage_for_mimo = 0;
  uint8_t                           dl_total_prb_usage_for_mimo   = 0;
  uint8_t                           ul_total_prb_usage_for_mimo   = 0;
  ngran_rr_status_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using eutran_cell_report_item_ext_ies_container = protocol_ext_container_empty_l;

// EUTRAN-CellReportItem ::= SEQUENCE
struct eutran_cell_report_item_s {
  bool                                        ext                           = false;
  bool                                        eutran_nof_active_ues_present = false;
  bool                                        eutran_noof_rrc_conns_present = false;
  bool                                        eutran_rr_status_present      = false;
  bool                                        ie_exts_present               = false;
  eutra_cgi_s                                 ecgi;
  eutran_composite_available_capacity_group_s eutran_composite_available_capacity_group;
  uint32_t                                    eutran_nof_active_ues = 0;
  uint32_t                                    eutran_noof_rrc_conns = 1;
  eutran_rr_status_s                          eutran_rr_status;
  eutran_cell_report_item_ext_ies_container   ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using eutran_cell_to_report_item_ext_ies_container = protocol_ext_container_empty_l;

// EUTRAN-CellToReportItem ::= SEQUENCE
struct eutran_cell_to_report_item_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  eutra_cgi_s                                  ecgi;
  eutran_cell_to_report_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ngran_cell_report_item_ext_ies_container = protocol_ext_container_empty_l;

// NGRAN-CellReportItem ::= SEQUENCE
struct ngran_cell_report_item_s {
  bool                                        ext                          = false;
  bool                                        ngran_nof_active_ues_present = false;
  bool                                        ngran_noof_rrc_conns_present = false;
  bool                                        ngran_rr_status_present      = false;
  bool                                        ie_exts_present              = false;
  ngran_cgi_c                                 ngran_cgi;
  eutran_composite_available_capacity_group_s ngran_composite_available_capacity_group;
  uint32_t                                    ngran_nof_active_ues = 0;
  uint32_t                                    ngran_noof_rrc_conns = 1;
  ngran_rr_status_s                           ngran_rr_status;
  ngran_cell_report_item_ext_ies_container    ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ngran_cell_to_report_item_ext_ies_container = protocol_ext_container_empty_l;

// NGRAN-CellToReportItem ::= SEQUENCE
struct ngran_cell_to_report_item_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  ngran_cgi_c                                 ngran_cgi;
  ngran_cell_to_report_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRAN-CellReportList ::= SEQUENCE (SIZE (1..256)) OF EUTRAN-CellReportItem
using eutran_cell_report_list_l = dyn_array<eutran_cell_report_item_s>;

// EUTRAN-CellToReportList ::= SEQUENCE (SIZE (1..256)) OF EUTRAN-CellToReportItem
using eutran_cell_to_report_list_l = dyn_array<eutran_cell_to_report_item_s>;

// EUTRAN-ReportingStatusIEs-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using eutran_report_status_ies_ext_ies_o = protocol_ext_empty_o;

// EUTRAN-ReportingSystemIEs-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using eutran_report_sys_ies_ext_ies_o = protocol_ext_empty_o;

// EventBasedReportingIEs-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using event_based_report_ies_ext_ies_o = protocol_ext_empty_o;

// NGRAN-CellReportList ::= SEQUENCE (SIZE (1..256)) OF NGRAN-CellReportItem
using ngran_cell_report_list_l = dyn_array<ngran_cell_report_item_s>;

// NGRAN-CellToReportList ::= SEQUENCE (SIZE (1..256)) OF NGRAN-CellToReportItem
using ngran_cell_to_report_list_l = dyn_array<ngran_cell_to_report_item_s>;

// NGRAN-ReportingStatusIEs-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ngran_report_status_ies_ext_ies_o = protocol_ext_empty_o;

// NGRAN-ReportingSystemIEs-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ngran_report_sys_ies_ext_ies_o = protocol_ext_empty_o;

// NotificationCell-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using notif_cell_item_ext_ies_o = protocol_ext_empty_o;

// NumberOfMeasurementReportingLevels ::= ENUMERATED
struct nof_meas_report_levels_opts {
  enum options { n2, n3, n4, n5, n10, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using nof_meas_report_levels_e = enumerated<nof_meas_report_levels_opts, true>;

// PeriodicReportingIEs-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using periodic_report_ies_ext_ies_o = protocol_ext_empty_o;

// ReportingPeriodicity ::= ENUMERATED
struct report_periodicity_opts {
  enum options { stop, single, ms1000, ms2000, ms5000, ms10000, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using report_periodicity_e = enumerated<report_periodicity_opts, true>;

// SCTP-TLAs ::= SEQUENCE (SIZE (1..2)) OF BIT STRING (SIZE (1..160,...))
using sctp_tlas_l = bounded_array<bounded_bitstring<1, 160, true, true>, 2>;

// SuccessfulHandoverReport-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using successful_ho_report_item_ext_ies_o = protocol_ext_empty_o;

// UERLFReportContainer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using ue_rlf_report_container_ext_ies_o = protocol_ies_empty_o;

// ENB-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using enb_id_ext_ies_o = protocol_ies_empty_o;

using eutran_report_status_ies_ext_ies_container = protocol_ext_container_empty_l;

// EUTRAN-ReportingStatusIEs ::= SEQUENCE
struct eutran_report_status_ies_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  eutran_cell_report_list_l                  eutran_cell_report_list;
  eutran_report_status_ies_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using eutran_report_sys_ies_ext_ies_container = protocol_ext_container_empty_l;

// EUTRAN-ReportingSystemIEs ::= SEQUENCE
struct eutran_report_sys_ies_s {
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  eutran_cell_to_report_list_l            eutran_cell_to_report_list;
  eutran_report_sys_ies_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using event_based_report_ies_ext_ies_container = protocol_ext_container_empty_l;

// EventBasedReportingIEs ::= SEQUENCE
struct event_based_report_ies_s {
  bool                                     ext                     = false;
  bool                                     ie_exts_present         = false;
  uint8_t                                  intersys_res_thres_low  = 0;
  uint8_t                                  intersys_res_thres_high = 0;
  nof_meas_report_levels_e                 nof_meas_report_levels;
  event_based_report_ies_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntersystemUnnecessaryHO-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using intersys_unnecessary_ho_ext_ies_o = protocol_ext_empty_o;

using ngran_report_status_ies_ext_ies_container = protocol_ext_container_empty_l;

// NGRAN-ReportingStatusIEs ::= SEQUENCE
struct ngran_report_status_ies_s {
  bool                                      ext             = false;
  bool                                      ie_exts_present = false;
  ngran_cell_report_list_l                  ngran_cell_report_list;
  ngran_report_status_ies_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ngran_report_sys_ies_ext_ies_container = protocol_ext_container_empty_l;

// NGRAN-ReportingSystemIEs ::= SEQUENCE
struct ngran_report_sys_ies_s {
  bool                                   ext             = false;
  bool                                   ie_exts_present = false;
  ngran_cell_to_report_list_l            ngran_cell_to_report_list;
  ngran_report_sys_ies_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using notif_cell_item_ext_ies_container = protocol_ext_container_empty_l;

// NotificationCell-Item ::= SEQUENCE
struct notif_cell_item_s {
  struct notify_flag_opts {
    enum options { activ, deactiv, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using notify_flag_e_ = enumerated<notify_flag_opts, true>;

  // member variables
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  ngran_cgi_c                       ngran_cgi;
  notify_flag_e_                    notify_flag;
  notif_cell_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using periodic_report_ies_ext_ies_container = protocol_ext_container_empty_l;

// PeriodicReportingIEs ::= SEQUENCE
struct periodic_report_ies_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  report_periodicity_e                  report_periodicity;
  periodic_report_ies_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportType-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using report_type_ext_ies_o = protocol_ies_empty_o;

// ReportingSystem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using report_sys_ext_ies_o = protocol_ies_empty_o;

// ResourceStatusReportingSystem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using res_status_report_sys_ext_ies_o = protocol_ies_empty_o;

using successful_ho_report_item_ext_ies_container = protocol_ext_container_empty_l;

// SuccessfulHandoverReport-Item ::= SEQUENCE
struct successful_ho_report_item_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  unbounded_octstring<true>                   successful_ho_report_container;
  successful_ho_report_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TooearlyIntersystemHO-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using tooearly_intersys_ho_ext_ies_o = protocol_ext_empty_o;

// UERLFReportContainer ::= CHOICE
struct ue_rlf_report_container_c {
  struct types_opts {
    enum options { nr, lte, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ue_rlf_report_container_c() = default;
  ue_rlf_report_container_c(const ue_rlf_report_container_c& other);
  ue_rlf_report_container_c& operator=(const ue_rlf_report_container_c& other);
  ~ue_rlf_report_container_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  unbounded_octstring<true>& nr()
  {
    assert_choice_type(types::nr, type_, "UERLFReportContainer");
    return c.get<unbounded_octstring<true>>();
  }
  unbounded_octstring<true>& lte()
  {
    assert_choice_type(types::lte, type_, "UERLFReportContainer");
    return c.get<unbounded_octstring<true>>();
  }
  protocol_ie_single_container_s<ue_rlf_report_container_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "UERLFReportContainer");
    return c.get<protocol_ie_single_container_s<ue_rlf_report_container_ext_ies_o>>();
  }
  const unbounded_octstring<true>& nr() const
  {
    assert_choice_type(types::nr, type_, "UERLFReportContainer");
    return c.get<unbounded_octstring<true>>();
  }
  const unbounded_octstring<true>& lte() const
  {
    assert_choice_type(types::lte, type_, "UERLFReportContainer");
    return c.get<unbounded_octstring<true>>();
  }
  const protocol_ie_single_container_s<ue_rlf_report_container_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "UERLFReportContainer");
    return c.get<protocol_ie_single_container_s<ue_rlf_report_container_ext_ies_o>>();
  }
  unbounded_octstring<true>&                                         set_nr();
  unbounded_octstring<true>&                                         set_lte();
  protocol_ie_single_container_s<ue_rlf_report_container_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                         type_;
  choice_buffer_t<protocol_ie_single_container_s<ue_rlf_report_container_ext_ies_o>, unbounded_octstring<true>> c;

  void destroy_();
};

// XnExtTLA-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct xn_ext_tla_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { sctp_tlas, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::sctp_tlas; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sctp_tlas_l&       sctp_tlas() { return c; }
    const sctp_tlas_l& sctp_tlas() const { return c; }

  private:
    sctp_tlas_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// XnGTP-TLAs ::= SEQUENCE (SIZE (1..16)) OF BIT STRING (SIZE (1..160,...))
using xn_gtp_tlas_l = bounded_array<bounded_bitstring<1, 160, true, true>, 16>;

// ENB-ID ::= CHOICE
struct enb_id_c {
  struct types_opts {
    enum options { macro_enb_id, home_enb_id, short_macro_enb_id, long_macro_enb_id, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  enb_id_c() = default;
  enb_id_c(const enb_id_c& other);
  enb_id_c& operator=(const enb_id_c& other);
  ~enb_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<20, false, true>& macro_enb_id()
  {
    assert_choice_type(types::macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<20, false, true>>();
  }
  fixed_bitstring<28, false, true>& home_enb_id()
  {
    assert_choice_type(types::home_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<28, false, true>>();
  }
  fixed_bitstring<18, false, true>& short_macro_enb_id()
  {
    assert_choice_type(types::short_macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<18, false, true>>();
  }
  fixed_bitstring<21, false, true>& long_macro_enb_id()
  {
    assert_choice_type(types::long_macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<21, false, true>>();
  }
  protocol_ie_single_container_s<enb_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "ENB-ID");
    return c.get<protocol_ie_single_container_s<enb_id_ext_ies_o>>();
  }
  const fixed_bitstring<20, false, true>& macro_enb_id() const
  {
    assert_choice_type(types::macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<20, false, true>>();
  }
  const fixed_bitstring<28, false, true>& home_enb_id() const
  {
    assert_choice_type(types::home_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<28, false, true>>();
  }
  const fixed_bitstring<18, false, true>& short_macro_enb_id() const
  {
    assert_choice_type(types::short_macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<18, false, true>>();
  }
  const fixed_bitstring<21, false, true>& long_macro_enb_id() const
  {
    assert_choice_type(types::long_macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<21, false, true>>();
  }
  const protocol_ie_single_container_s<enb_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "ENB-ID");
    return c.get<protocol_ie_single_container_s<enb_id_ext_ies_o>>();
  }
  fixed_bitstring<20, false, true>&                 set_macro_enb_id();
  fixed_bitstring<28, false, true>&                 set_home_enb_id();
  fixed_bitstring<18, false, true>&                 set_short_macro_enb_id();
  fixed_bitstring<21, false, true>&                 set_long_macro_enb_id();
  protocol_ie_single_container_s<enb_id_ext_ies_o>& set_choice_exts();

private:
  types                                                                                               type_;
  choice_buffer_t<fixed_bitstring<28, false, true>, protocol_ie_single_container_s<enb_id_ext_ies_o>> c;

  void destroy_();
};

// EPS-TAI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using eps_tai_ext_ies_o = protocol_ext_empty_o;

// FailureIndication-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using fail_ind_ext_ies_o = protocol_ext_empty_o;

// GlobalENB-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using global_enb_id_ext_ies_o = protocol_ext_empty_o;

// HOReport-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ho_report_ext_ies_o = protocol_ext_empty_o;

// InterSystemHandoverReportType-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using inter_sys_ho_report_type_ext_ies_o = protocol_ies_empty_o;

// IntersystemCellActivationReply-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using intersys_cell_activation_reply_ext_ies_o = protocol_ext_empty_o;

// IntersystemCellActivationRequest-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using intersys_cell_activation_request_ext_ies_o = protocol_ext_empty_o;

// IntersystemCellStateIndication-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using intersys_cell_state_ind_ext_ies_o = protocol_ext_empty_o;

// IntersystemResourceStatusReply-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using intersys_res_status_reply_ext_ies_o = protocol_ext_empty_o;

// IntersystemResourceStatusReport-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using intersys_res_status_report_ext_ies_o = protocol_ext_empty_o;

// IntersystemResourceStatusRequest-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using intersys_res_status_request_ext_ies_o = protocol_ext_empty_o;

using intersys_unnecessary_ho_ext_ies_container = protocol_ext_container_empty_l;

// IntersystemUnnecessaryHO ::= SEQUENCE
struct intersys_unnecessary_ho_s {
  struct early_iratho_opts {
    enum options { true_value, false_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using early_iratho_e_ = enumerated<early_iratho_opts, true>;

  // member variables
  bool                                      ext             = false;
  bool                                      ie_exts_present = false;
  ngran_cgi_c                               sourcecell_id;
  eutra_cgi_s                               targetcell_id;
  early_iratho_e_                           early_iratho;
  candidate_cell_list_l                     candidate_cell_list;
  intersys_unnecessary_ho_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NotificationCellList ::= SEQUENCE (SIZE (1..16384)) OF NotificationCell-Item
using notif_cell_list_l = dyn_array<notif_cell_item_s>;

// ReportType ::= CHOICE
struct report_type_c {
  struct types_opts {
    enum options { event_based_report, periodic_report, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  report_type_c() = default;
  report_type_c(const report_type_c& other);
  report_type_c& operator=(const report_type_c& other);
  ~report_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  event_based_report_ies_s& event_based_report()
  {
    assert_choice_type(types::event_based_report, type_, "ReportType");
    return c.get<event_based_report_ies_s>();
  }
  periodic_report_ies_s& periodic_report()
  {
    assert_choice_type(types::periodic_report, type_, "ReportType");
    return c.get<periodic_report_ies_s>();
  }
  protocol_ie_single_container_s<report_type_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "ReportType");
    return c.get<protocol_ie_single_container_s<report_type_ext_ies_o>>();
  }
  const event_based_report_ies_s& event_based_report() const
  {
    assert_choice_type(types::event_based_report, type_, "ReportType");
    return c.get<event_based_report_ies_s>();
  }
  const periodic_report_ies_s& periodic_report() const
  {
    assert_choice_type(types::periodic_report, type_, "ReportType");
    return c.get<periodic_report_ies_s>();
  }
  const protocol_ie_single_container_s<report_type_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "ReportType");
    return c.get<protocol_ie_single_container_s<report_type_ext_ies_o>>();
  }
  event_based_report_ies_s&                              set_event_based_report();
  periodic_report_ies_s&                                 set_periodic_report();
  protocol_ie_single_container_s<report_type_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<event_based_report_ies_s,
                  periodic_report_ies_s,
                  protocol_ie_single_container_s<report_type_ext_ies_o>>
      c;

  void destroy_();
};

// ReportingSystem ::= CHOICE
struct report_sys_c {
  struct types_opts {
    enum options { eutran, ngran, no_report, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  report_sys_c() = default;
  report_sys_c(const report_sys_c& other);
  report_sys_c& operator=(const report_sys_c& other);
  ~report_sys_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutran_report_sys_ies_s& eutran()
  {
    assert_choice_type(types::eutran, type_, "ReportingSystem");
    return c.get<eutran_report_sys_ies_s>();
  }
  ngran_report_sys_ies_s& ngran()
  {
    assert_choice_type(types::ngran, type_, "ReportingSystem");
    return c.get<ngran_report_sys_ies_s>();
  }
  protocol_ie_single_container_s<report_sys_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "ReportingSystem");
    return c.get<protocol_ie_single_container_s<report_sys_ext_ies_o>>();
  }
  const eutran_report_sys_ies_s& eutran() const
  {
    assert_choice_type(types::eutran, type_, "ReportingSystem");
    return c.get<eutran_report_sys_ies_s>();
  }
  const ngran_report_sys_ies_s& ngran() const
  {
    assert_choice_type(types::ngran, type_, "ReportingSystem");
    return c.get<ngran_report_sys_ies_s>();
  }
  const protocol_ie_single_container_s<report_sys_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "ReportingSystem");
    return c.get<protocol_ie_single_container_s<report_sys_ext_ies_o>>();
  }
  eutran_report_sys_ies_s&                              set_eutran();
  ngran_report_sys_ies_s&                               set_ngran();
  void                                                  set_no_report();
  protocol_ie_single_container_s<report_sys_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<eutran_report_sys_ies_s, ngran_report_sys_ies_s, protocol_ie_single_container_s<report_sys_ext_ies_o>>
      c;

  void destroy_();
};

// ResourceStatusReportingSystem ::= CHOICE
struct res_status_report_sys_c {
  struct types_opts {
    enum options { eutran_report_status, ngran_report_status, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  res_status_report_sys_c() = default;
  res_status_report_sys_c(const res_status_report_sys_c& other);
  res_status_report_sys_c& operator=(const res_status_report_sys_c& other);
  ~res_status_report_sys_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutran_report_status_ies_s& eutran_report_status()
  {
    assert_choice_type(types::eutran_report_status, type_, "ResourceStatusReportingSystem");
    return c.get<eutran_report_status_ies_s>();
  }
  ngran_report_status_ies_s& ngran_report_status()
  {
    assert_choice_type(types::ngran_report_status, type_, "ResourceStatusReportingSystem");
    return c.get<ngran_report_status_ies_s>();
  }
  protocol_ie_single_container_s<res_status_report_sys_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "ResourceStatusReportingSystem");
    return c.get<protocol_ie_single_container_s<res_status_report_sys_ext_ies_o>>();
  }
  const eutran_report_status_ies_s& eutran_report_status() const
  {
    assert_choice_type(types::eutran_report_status, type_, "ResourceStatusReportingSystem");
    return c.get<eutran_report_status_ies_s>();
  }
  const ngran_report_status_ies_s& ngran_report_status() const
  {
    assert_choice_type(types::ngran_report_status, type_, "ResourceStatusReportingSystem");
    return c.get<ngran_report_status_ies_s>();
  }
  const protocol_ie_single_container_s<res_status_report_sys_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "ResourceStatusReportingSystem");
    return c.get<protocol_ie_single_container_s<res_status_report_sys_ext_ies_o>>();
  }
  eutran_report_status_ies_s&                                      set_eutran_report_status();
  ngran_report_status_ies_s&                                       set_ngran_report_status();
  protocol_ie_single_container_s<res_status_report_sys_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<eutran_report_status_ies_s,
                  ngran_report_status_ies_s,
                  protocol_ie_single_container_s<res_status_report_sys_ext_ies_o>>
      c;

  void destroy_();
};

// SuccessfulHandoverReportList ::= SEQUENCE (SIZE (1..64)) OF SuccessfulHandoverReport-Item
using successful_ho_report_list_l = dyn_array<successful_ho_report_item_s>;

using tooearly_intersys_ho_ext_ies_container = protocol_ext_container_empty_l;

// TooearlyIntersystemHO ::= SEQUENCE
struct tooearly_intersys_ho_s {
  bool                                   ext                             = false;
  bool                                   ue_rlf_report_container_present = false;
  bool                                   ie_exts_present                 = false;
  eutra_cgi_s                            sourcecell_id;
  ngran_cgi_c                            failcell_id;
  ue_rlf_report_container_c              ue_rlf_report_container;
  tooearly_intersys_ho_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// XnExtTLA-Item ::= SEQUENCE
struct xn_ext_tla_item_s {
  bool                                                ext                = false;
  bool                                                ip_sec_tla_present = false;
  bounded_bitstring<1, 160, true, true>               ip_sec_tla;
  xn_gtp_tlas_l                                       gtp_tlas;
  protocol_ext_container_l<xn_ext_tla_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using eps_tai_ext_ies_container = protocol_ext_container_empty_l;

// EPS-TAI ::= SEQUENCE
struct eps_tai_s {
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  fixed_octstring<3, true>  plmn_id;
  fixed_octstring<2, true>  eps_tac;
  eps_tai_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using fail_ind_ext_ies_container = protocol_ext_container_empty_l;

// FailureIndication ::= SEQUENCE
struct fail_ind_s {
  bool                       ext             = false;
  bool                       ie_exts_present = false;
  ue_rlf_report_container_c  ue_rlf_report_container;
  fail_ind_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using global_enb_id_ext_ies_container = protocol_ext_container_empty_l;

// GlobalENB-ID ::= SEQUENCE
struct global_enb_id_s {
  bool                            ext             = false;
  bool                            ie_exts_present = false;
  fixed_octstring<3, true>        plmn_id;
  enb_id_c                        enb_id;
  global_enb_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ho_report_ext_ies_container = protocol_ext_container_empty_l;

// HOReport ::= SEQUENCE
struct ho_report_s {
  struct ho_report_type_opts {
    enum options { ho_too_early, ho_to_wrong_cell, intersys_ping_pong, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ho_report_type_e_ = enumerated<ho_report_type_opts, true>;

  // member variables
  bool                             ext                             = false;
  bool                             reestcell_cgi_present           = false;
  bool                             sourcecell_c_rnti_present       = false;
  bool                             targetcellin_e_utran_present    = false;
  bool                             mob_info_present                = false;
  bool                             ue_rlf_report_container_present = false;
  bool                             ie_exts_present                 = false;
  ho_report_type_e_                ho_report_type;
  cause_c                          ho_cause;
  ngran_cgi_c                      sourcecell_cgi;
  ngran_cgi_c                      targetcell_cgi;
  ngran_cgi_c                      reestcell_cgi;
  fixed_bitstring<16, false, true> sourcecell_c_rnti;
  eutra_cgi_s                      targetcellin_e_utran;
  fixed_bitstring<16, false, true> mob_info;
  ue_rlf_report_container_c        ue_rlf_report_container;
  ho_report_ext_ies_container      ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterSystemFailureIndication-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using inter_sys_fail_ind_ext_ies_o = protocol_ext_empty_o;

// InterSystemHOReport-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using inter_sys_ho_report_ext_ies_o = protocol_ext_empty_o;

// InterSystemHandoverReportType ::= CHOICE
struct inter_sys_ho_report_type_c {
  struct types_opts {
    enum options { tooearly_intersys_ho, intersys_unnecessary_ho, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  inter_sys_ho_report_type_c() = default;
  inter_sys_ho_report_type_c(const inter_sys_ho_report_type_c& other);
  inter_sys_ho_report_type_c& operator=(const inter_sys_ho_report_type_c& other);
  ~inter_sys_ho_report_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  tooearly_intersys_ho_s& tooearly_intersys_ho()
  {
    assert_choice_type(types::tooearly_intersys_ho, type_, "InterSystemHandoverReportType");
    return c.get<tooearly_intersys_ho_s>();
  }
  intersys_unnecessary_ho_s& intersys_unnecessary_ho()
  {
    assert_choice_type(types::intersys_unnecessary_ho, type_, "InterSystemHandoverReportType");
    return c.get<intersys_unnecessary_ho_s>();
  }
  protocol_ie_single_container_s<inter_sys_ho_report_type_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "InterSystemHandoverReportType");
    return c.get<protocol_ie_single_container_s<inter_sys_ho_report_type_ext_ies_o>>();
  }
  const tooearly_intersys_ho_s& tooearly_intersys_ho() const
  {
    assert_choice_type(types::tooearly_intersys_ho, type_, "InterSystemHandoverReportType");
    return c.get<tooearly_intersys_ho_s>();
  }
  const intersys_unnecessary_ho_s& intersys_unnecessary_ho() const
  {
    assert_choice_type(types::intersys_unnecessary_ho, type_, "InterSystemHandoverReportType");
    return c.get<intersys_unnecessary_ho_s>();
  }
  const protocol_ie_single_container_s<inter_sys_ho_report_type_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "InterSystemHandoverReportType");
    return c.get<protocol_ie_single_container_s<inter_sys_ho_report_type_ext_ies_o>>();
  }
  tooearly_intersys_ho_s&                                             set_tooearly_intersys_ho();
  intersys_unnecessary_ho_s&                                          set_intersys_unnecessary_ho();
  protocol_ie_single_container_s<inter_sys_ho_report_type_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<intersys_unnecessary_ho_s,
                  protocol_ie_single_container_s<inter_sys_ho_report_type_ext_ies_o>,
                  tooearly_intersys_ho_s>
      c;

  void destroy_();
};

using intersys_cell_activation_reply_ext_ies_container = protocol_ext_container_empty_l;

// IntersystemCellActivationReply ::= SEQUENCE
struct intersys_cell_activation_reply_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  activ_cell_list_l                                activ_cell_list;
  uint16_t                                         activation_id = 0;
  intersys_cell_activation_reply_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using intersys_cell_activation_request_ext_ies_container = protocol_ext_container_empty_l;

// IntersystemCellActivationRequest ::= SEQUENCE
struct intersys_cell_activation_request_s {
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  uint16_t                                           activation_id   = 0;
  cells_to_activ_list_l                              cells_to_activ_list;
  intersys_cell_activation_request_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using intersys_cell_state_ind_ext_ies_container = protocol_ext_container_empty_l;

// IntersystemCellStateIndication ::= SEQUENCE
struct intersys_cell_state_ind_s {
  bool                                      ext             = false;
  bool                                      ie_exts_present = false;
  notif_cell_list_l                         notif_cell_list;
  intersys_cell_state_ind_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using intersys_res_status_reply_ext_ies_container = protocol_ext_container_empty_l;

// IntersystemResourceStatusReply ::= SEQUENCE
struct intersys_res_status_reply_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  report_sys_c                                reportsys;
  intersys_res_status_reply_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using intersys_res_status_report_ext_ies_container = protocol_ext_container_empty_l;

// IntersystemResourceStatusReport ::= SEQUENCE
struct intersys_res_status_report_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  res_status_report_sys_c                      report_sys;
  intersys_res_status_report_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using intersys_res_status_request_ext_ies_container = protocol_ext_container_empty_l;

// IntersystemResourceStatusRequest ::= SEQUENCE
struct intersys_res_status_request_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  report_sys_c                                  report_sys;
  fixed_bitstring<32, false, true>              report_characteristics;
  report_type_c                                 report_type;
  intersys_res_status_request_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntersystemSONInformationReply-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using intersys_son_info_reply_ext_ies_o = protocol_ies_empty_o;

// IntersystemSONInformationRequest-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using intersys_son_info_request_ext_ies_o = protocol_ies_empty_o;

// IntersystemSONNGRANnodeID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using intersys_son_ngran_node_id_ext_ies_o = protocol_ext_empty_o;

// IntersystemSONeNBID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using intersys_son_enb_id_ext_ies_o = protocol_ext_empty_o;

// SONInformationReport-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct son_info_report_ext_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { successful_ho_report_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::successful_ho_report_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    successful_ho_report_list_l&       successful_ho_report_list() { return c; }
    const successful_ho_report_list_l& successful_ho_report_list() const { return c; }

  private:
    successful_ho_report_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// XnExtTLAs ::= SEQUENCE (SIZE (1..16)) OF XnExtTLA-Item
using xn_ext_tlas_l = dyn_array<xn_ext_tla_item_s>;

// XnTLAs ::= SEQUENCE (SIZE (1..2)) OF BIT STRING (SIZE (1..160,...))
using xn_tlas_l = bounded_array<bounded_bitstring<1, 160, true, true>, 2>;

// XnTNLConfigurationInfo-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using xn_tnl_cfg_info_ext_ies_o = protocol_ext_empty_o;

// FromEUTRANtoNGRAN-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using from_eutran_to_ngran_ext_ies_o = protocol_ext_empty_o;

// FromNGRANtoEUTRAN-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using from_ngran_to_eutran_ext_ies_o = protocol_ext_empty_o;

using inter_sys_fail_ind_ext_ies_container = protocol_ext_container_empty_l;

// InterSystemFailureIndication ::= SEQUENCE
struct inter_sys_fail_ind_s {
  bool                                 ext                             = false;
  bool                                 ue_rlf_report_container_present = false;
  bool                                 ie_exts_present                 = false;
  ue_rlf_report_container_c            ue_rlf_report_container;
  inter_sys_fail_ind_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using inter_sys_ho_report_ext_ies_container = protocol_ext_container_empty_l;

// InterSystemHOReport ::= SEQUENCE
struct inter_sys_ho_report_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  inter_sys_ho_report_type_c            ho_report_type;
  inter_sys_ho_report_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntersystemSONInformationReply ::= CHOICE
struct intersys_son_info_reply_c {
  struct types_opts {
    enum options { ngran_cell_activation, res_status, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  intersys_son_info_reply_c() = default;
  intersys_son_info_reply_c(const intersys_son_info_reply_c& other);
  intersys_son_info_reply_c& operator=(const intersys_son_info_reply_c& other);
  ~intersys_son_info_reply_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  intersys_cell_activation_reply_s& ngran_cell_activation()
  {
    assert_choice_type(types::ngran_cell_activation, type_, "IntersystemSONInformationReply");
    return c.get<intersys_cell_activation_reply_s>();
  }
  intersys_res_status_reply_s& res_status()
  {
    assert_choice_type(types::res_status, type_, "IntersystemSONInformationReply");
    return c.get<intersys_res_status_reply_s>();
  }
  protocol_ie_single_container_s<intersys_son_info_reply_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "IntersystemSONInformationReply");
    return c.get<protocol_ie_single_container_s<intersys_son_info_reply_ext_ies_o>>();
  }
  const intersys_cell_activation_reply_s& ngran_cell_activation() const
  {
    assert_choice_type(types::ngran_cell_activation, type_, "IntersystemSONInformationReply");
    return c.get<intersys_cell_activation_reply_s>();
  }
  const intersys_res_status_reply_s& res_status() const
  {
    assert_choice_type(types::res_status, type_, "IntersystemSONInformationReply");
    return c.get<intersys_res_status_reply_s>();
  }
  const protocol_ie_single_container_s<intersys_son_info_reply_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "IntersystemSONInformationReply");
    return c.get<protocol_ie_single_container_s<intersys_son_info_reply_ext_ies_o>>();
  }
  intersys_cell_activation_reply_s&                                  set_ngran_cell_activation();
  intersys_res_status_reply_s&                                       set_res_status();
  protocol_ie_single_container_s<intersys_son_info_reply_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<intersys_cell_activation_reply_s,
                  intersys_res_status_reply_s,
                  protocol_ie_single_container_s<intersys_son_info_reply_ext_ies_o>>
      c;

  void destroy_();
};

// IntersystemSONInformationReport-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct intersys_son_info_report_ext_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { energy_saving_ind, intersys_res_status_upd, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    intersys_cell_state_ind_s&          energy_saving_ind();
    intersys_res_status_report_s&       intersys_res_status_upd();
    const intersys_cell_state_ind_s&    energy_saving_ind() const;
    const intersys_res_status_report_s& intersys_res_status_upd() const;

  private:
    types                                                                    type_;
    choice_buffer_t<intersys_cell_state_ind_s, intersys_res_status_report_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// IntersystemSONInformationRequest ::= CHOICE
struct intersys_son_info_request_c {
  struct types_opts {
    enum options { ngran_cell_activation, res_status, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  intersys_son_info_request_c() = default;
  intersys_son_info_request_c(const intersys_son_info_request_c& other);
  intersys_son_info_request_c& operator=(const intersys_son_info_request_c& other);
  ~intersys_son_info_request_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  intersys_cell_activation_request_s& ngran_cell_activation()
  {
    assert_choice_type(types::ngran_cell_activation, type_, "IntersystemSONInformationRequest");
    return c.get<intersys_cell_activation_request_s>();
  }
  intersys_res_status_request_s& res_status()
  {
    assert_choice_type(types::res_status, type_, "IntersystemSONInformationRequest");
    return c.get<intersys_res_status_request_s>();
  }
  protocol_ie_single_container_s<intersys_son_info_request_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "IntersystemSONInformationRequest");
    return c.get<protocol_ie_single_container_s<intersys_son_info_request_ext_ies_o>>();
  }
  const intersys_cell_activation_request_s& ngran_cell_activation() const
  {
    assert_choice_type(types::ngran_cell_activation, type_, "IntersystemSONInformationRequest");
    return c.get<intersys_cell_activation_request_s>();
  }
  const intersys_res_status_request_s& res_status() const
  {
    assert_choice_type(types::res_status, type_, "IntersystemSONInformationRequest");
    return c.get<intersys_res_status_request_s>();
  }
  const protocol_ie_single_container_s<intersys_son_info_request_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "IntersystemSONInformationRequest");
    return c.get<protocol_ie_single_container_s<intersys_son_info_request_ext_ies_o>>();
  }
  intersys_cell_activation_request_s&                                  set_ngran_cell_activation();
  intersys_res_status_request_s&                                       set_res_status();
  protocol_ie_single_container_s<intersys_son_info_request_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<intersys_cell_activation_request_s,
                  intersys_res_status_request_s,
                  protocol_ie_single_container_s<intersys_son_info_request_ext_ies_o>>
      c;

  void destroy_();
};

using intersys_son_ngran_node_id_ext_ies_container = protocol_ext_container_empty_l;

// IntersystemSONNGRANnodeID ::= SEQUENCE
struct intersys_son_ngran_node_id_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  global_ran_node_id_c                         global_ran_node_id;
  tai_s                                        sel_tai;
  intersys_son_ngran_node_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using intersys_son_enb_id_ext_ies_container = protocol_ext_container_empty_l;

// IntersystemSONeNBID ::= SEQUENCE
struct intersys_son_enb_id_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  global_enb_id_s                       global_enb_id;
  eps_tai_s                             sel_eps_tai;
  intersys_son_enb_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SONInformationReply-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using son_info_reply_ext_ies_o = protocol_ext_empty_o;

// SONInformationReport ::= CHOICE
struct son_info_report_c {
  struct types_opts {
    enum options { fail_ind_info, ho_report_info, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  son_info_report_c() = default;
  son_info_report_c(const son_info_report_c& other);
  son_info_report_c& operator=(const son_info_report_c& other);
  ~son_info_report_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fail_ind_s& fail_ind_info()
  {
    assert_choice_type(types::fail_ind_info, type_, "SONInformationReport");
    return c.get<fail_ind_s>();
  }
  ho_report_s& ho_report_info()
  {
    assert_choice_type(types::ho_report_info, type_, "SONInformationReport");
    return c.get<ho_report_s>();
  }
  protocol_ie_single_container_s<son_info_report_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "SONInformationReport");
    return c.get<protocol_ie_single_container_s<son_info_report_ext_ies_o>>();
  }
  const fail_ind_s& fail_ind_info() const
  {
    assert_choice_type(types::fail_ind_info, type_, "SONInformationReport");
    return c.get<fail_ind_s>();
  }
  const ho_report_s& ho_report_info() const
  {
    assert_choice_type(types::ho_report_info, type_, "SONInformationReport");
    return c.get<ho_report_s>();
  }
  const protocol_ie_single_container_s<son_info_report_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "SONInformationReport");
    return c.get<protocol_ie_single_container_s<son_info_report_ext_ies_o>>();
  }
  fail_ind_s&                                                set_fail_ind_info();
  ho_report_s&                                               set_ho_report_info();
  protocol_ie_single_container_s<son_info_report_ext_ies_o>& set_choice_exts();

private:
  types                                                                                               type_;
  choice_buffer_t<fail_ind_s, ho_report_s, protocol_ie_single_container_s<son_info_report_ext_ies_o>> c;

  void destroy_();
};

using xn_tnl_cfg_info_ext_ies_container = protocol_ext_container_empty_l;

// XnTNLConfigurationInfo ::= SEQUENCE
struct xn_tnl_cfg_info_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  xn_tlas_l                         xn_transport_layer_addresses;
  xn_ext_tlas_l                     xn_extended_transport_layer_addresses;
  xn_tnl_cfg_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using from_eutran_to_ngran_ext_ies_container = protocol_ext_container_empty_l;

// FromEUTRANtoNGRAN ::= SEQUENCE
struct from_eutran_to_ngran_s {
  bool                                   ie_exts_present = false;
  intersys_son_enb_id_s                  source_enb_id;
  intersys_son_ngran_node_id_s           target_ngran_node_id;
  from_eutran_to_ngran_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using from_ngran_to_eutran_ext_ies_container = protocol_ext_container_empty_l;

// FromNGRANtoEUTRAN ::= SEQUENCE
struct from_ngran_to_eutran_s {
  bool                                   ie_exts_present = false;
  intersys_son_ngran_node_id_s           source_ngran_node_id;
  intersys_son_enb_id_s                  target_enb_id;
  from_ngran_to_eutran_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntersystemSONInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct intersys_son_info_ext_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { intersys_son_info_request, intersys_son_info_reply, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    intersys_son_info_request_c&       intersys_son_info_request();
    intersys_son_info_reply_c&         intersys_son_info_reply();
    const intersys_son_info_request_c& intersys_son_info_request() const;
    const intersys_son_info_reply_c&   intersys_son_info_reply() const;

  private:
    types                                                                   type_;
    choice_buffer_t<intersys_son_info_reply_c, intersys_son_info_request_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// IntersystemSONInformationReport ::= CHOICE
struct intersys_son_info_report_c {
  struct types_opts {
    enum options { ho_report_info, fail_ind_info, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  intersys_son_info_report_c() = default;
  intersys_son_info_report_c(const intersys_son_info_report_c& other);
  intersys_son_info_report_c& operator=(const intersys_son_info_report_c& other);
  ~intersys_son_info_report_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  inter_sys_ho_report_s& ho_report_info()
  {
    assert_choice_type(types::ho_report_info, type_, "IntersystemSONInformationReport");
    return c.get<inter_sys_ho_report_s>();
  }
  inter_sys_fail_ind_s& fail_ind_info()
  {
    assert_choice_type(types::fail_ind_info, type_, "IntersystemSONInformationReport");
    return c.get<inter_sys_fail_ind_s>();
  }
  protocol_ie_single_container_s<intersys_son_info_report_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "IntersystemSONInformationReport");
    return c.get<protocol_ie_single_container_s<intersys_son_info_report_ext_ies_o>>();
  }
  const inter_sys_ho_report_s& ho_report_info() const
  {
    assert_choice_type(types::ho_report_info, type_, "IntersystemSONInformationReport");
    return c.get<inter_sys_ho_report_s>();
  }
  const inter_sys_fail_ind_s& fail_ind_info() const
  {
    assert_choice_type(types::fail_ind_info, type_, "IntersystemSONInformationReport");
    return c.get<inter_sys_fail_ind_s>();
  }
  const protocol_ie_single_container_s<intersys_son_info_report_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "IntersystemSONInformationReport");
    return c.get<protocol_ie_single_container_s<intersys_son_info_report_ext_ies_o>>();
  }
  inter_sys_ho_report_s&                                              set_ho_report_info();
  inter_sys_fail_ind_s&                                               set_fail_ind_info();
  protocol_ie_single_container_s<intersys_son_info_report_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<inter_sys_fail_ind_s,
                  inter_sys_ho_report_s,
                  protocol_ie_single_container_s<intersys_son_info_report_ext_ies_o>>
      c;

  void destroy_();
};

// IntersystemSONTransferType-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using intersys_son_transfer_type_ext_ies_o = protocol_ies_empty_o;

// SONInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct son_info_ext_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { son_info_report, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::son_info_report; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    son_info_report_c&       son_info_report() { return c; }
    const son_info_report_c& son_info_report() const { return c; }

  private:
    son_info_report_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using son_info_reply_ext_ies_container = protocol_ext_container_empty_l;

// SONInformationReply ::= SEQUENCE
struct son_info_reply_s {
  bool                             ext                     = false;
  bool                             xn_tnl_cfg_info_present = false;
  bool                             ie_exts_present         = false;
  xn_tnl_cfg_info_s                xn_tnl_cfg_info;
  son_info_reply_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SONInformationRequest ::= ENUMERATED
struct son_info_request_opts {
  enum options { xn_tnl_cfg_info, /*...*/ nulltype } value;

  const char* to_string() const;
};
using son_info_request_e = enumerated<son_info_request_opts, true>;

// SourceRANNodeID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using source_ran_node_id_ext_ies_o = protocol_ext_empty_o;

// TargetRANNodeID-SON-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct target_ran_node_id_son_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { nr_cgi, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::nr_cgi; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nr_cgi_s&       nr_cgi() { return c; }
    const nr_cgi_s& nr_cgi() const { return c; }

  private:
    nr_cgi_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// IntersystemSONConfigurationTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using intersys_son_cfg_transfer_ext_ies_o = protocol_ext_empty_o;

// IntersystemSONInformation ::= CHOICE
struct intersys_son_info_c {
  struct types_opts {
    enum options { intersys_son_info_report, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  intersys_son_info_c() = default;
  intersys_son_info_c(const intersys_son_info_c& other);
  intersys_son_info_c& operator=(const intersys_son_info_c& other);
  ~intersys_son_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  intersys_son_info_report_c& intersys_son_info_report()
  {
    assert_choice_type(types::intersys_son_info_report, type_, "IntersystemSONInformation");
    return c.get<intersys_son_info_report_c>();
  }
  protocol_ie_single_container_s<intersys_son_info_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "IntersystemSONInformation");
    return c.get<protocol_ie_single_container_s<intersys_son_info_ext_ies_o>>();
  }
  const intersys_son_info_report_c& intersys_son_info_report() const
  {
    assert_choice_type(types::intersys_son_info_report, type_, "IntersystemSONInformation");
    return c.get<intersys_son_info_report_c>();
  }
  const protocol_ie_single_container_s<intersys_son_info_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "IntersystemSONInformation");
    return c.get<protocol_ie_single_container_s<intersys_son_info_ext_ies_o>>();
  }
  intersys_son_info_report_c&                                  set_intersys_son_info_report();
  protocol_ie_single_container_s<intersys_son_info_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                    type_;
  choice_buffer_t<intersys_son_info_report_c, protocol_ie_single_container_s<intersys_son_info_ext_ies_o>> c;

  void destroy_();
};

// IntersystemSONTransferType ::= CHOICE
struct intersys_son_transfer_type_c {
  struct types_opts {
    enum options { from_eutran_to_ngran, from_ngran_to_eutran, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  intersys_son_transfer_type_c() = default;
  intersys_son_transfer_type_c(const intersys_son_transfer_type_c& other);
  intersys_son_transfer_type_c& operator=(const intersys_son_transfer_type_c& other);
  ~intersys_son_transfer_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  from_eutran_to_ngran_s& from_eutran_to_ngran()
  {
    assert_choice_type(types::from_eutran_to_ngran, type_, "IntersystemSONTransferType");
    return c.get<from_eutran_to_ngran_s>();
  }
  from_ngran_to_eutran_s& from_ngran_to_eutran()
  {
    assert_choice_type(types::from_ngran_to_eutran, type_, "IntersystemSONTransferType");
    return c.get<from_ngran_to_eutran_s>();
  }
  protocol_ie_single_container_s<intersys_son_transfer_type_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "IntersystemSONTransferType");
    return c.get<protocol_ie_single_container_s<intersys_son_transfer_type_ext_ies_o>>();
  }
  const from_eutran_to_ngran_s& from_eutran_to_ngran() const
  {
    assert_choice_type(types::from_eutran_to_ngran, type_, "IntersystemSONTransferType");
    return c.get<from_eutran_to_ngran_s>();
  }
  const from_ngran_to_eutran_s& from_ngran_to_eutran() const
  {
    assert_choice_type(types::from_ngran_to_eutran, type_, "IntersystemSONTransferType");
    return c.get<from_ngran_to_eutran_s>();
  }
  const protocol_ie_single_container_s<intersys_son_transfer_type_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "IntersystemSONTransferType");
    return c.get<protocol_ie_single_container_s<intersys_son_transfer_type_ext_ies_o>>();
  }
  from_eutran_to_ngran_s&                                               set_from_eutran_to_ngran();
  from_ngran_to_eutran_s&                                               set_from_ngran_to_eutran();
  protocol_ie_single_container_s<intersys_son_transfer_type_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<from_eutran_to_ngran_s,
                  from_ngran_to_eutran_s,
                  protocol_ie_single_container_s<intersys_son_transfer_type_ext_ies_o>>
      c;

  void destroy_();
};

// SONConfigurationTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using son_cfg_transfer_ext_ies_o = protocol_ext_empty_o;

// SONInformation ::= CHOICE
struct son_info_c {
  struct types_opts {
    enum options { son_info_request, son_info_reply, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  son_info_c() = default;
  son_info_c(const son_info_c& other);
  son_info_c& operator=(const son_info_c& other);
  ~son_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  son_info_request_e& son_info_request()
  {
    assert_choice_type(types::son_info_request, type_, "SONInformation");
    return c.get<son_info_request_e>();
  }
  son_info_reply_s& son_info_reply()
  {
    assert_choice_type(types::son_info_reply, type_, "SONInformation");
    return c.get<son_info_reply_s>();
  }
  protocol_ie_single_container_s<son_info_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "SONInformation");
    return c.get<protocol_ie_single_container_s<son_info_ext_ies_o>>();
  }
  const son_info_request_e& son_info_request() const
  {
    assert_choice_type(types::son_info_request, type_, "SONInformation");
    return c.get<son_info_request_e>();
  }
  const son_info_reply_s& son_info_reply() const
  {
    assert_choice_type(types::son_info_reply, type_, "SONInformation");
    return c.get<son_info_reply_s>();
  }
  const protocol_ie_single_container_s<son_info_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "SONInformation");
    return c.get<protocol_ie_single_container_s<son_info_ext_ies_o>>();
  }
  son_info_request_e&                                 set_son_info_request();
  son_info_reply_s&                                   set_son_info_reply();
  protocol_ie_single_container_s<son_info_ext_ies_o>& set_choice_exts();

private:
  types                                                                                 type_;
  choice_buffer_t<protocol_ie_single_container_s<son_info_ext_ies_o>, son_info_reply_s> c;

  void destroy_();
};

using source_ran_node_id_ext_ies_container = protocol_ext_container_empty_l;

// SourceRANNodeID ::= SEQUENCE
struct source_ran_node_id_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  global_ran_node_id_c                 global_ran_node_id;
  tai_s                                sel_tai;
  source_ran_node_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TargetRANNodeID-SON ::= SEQUENCE
struct target_ran_node_id_son_s {
  bool                                                       ext = false;
  global_ran_node_id_c                                       global_ran_node_id;
  tai_s                                                      sel_tai;
  protocol_ext_container_l<target_ran_node_id_son_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using intersys_son_cfg_transfer_ext_ies_container = protocol_ext_container_empty_l;

// IntersystemSONConfigurationTransfer ::= SEQUENCE
struct intersys_son_cfg_transfer_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  intersys_son_transfer_type_c                transfer_type;
  intersys_son_info_c                         intersys_son_info;
  intersys_son_cfg_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using son_cfg_transfer_ext_ies_container = protocol_ext_container_empty_l;

// SONConfigurationTransfer ::= SEQUENCE
struct son_cfg_transfer_s {
  bool                               ext                     = false;
  bool                               xn_tnl_cfg_info_present = false;
  bool                               ie_exts_present         = false;
  target_ran_node_id_son_s           target_ran_node_id_son;
  source_ran_node_id_s               source_ran_node_id;
  son_info_c                         son_info;
  xn_tnl_cfg_info_s                  xn_tnl_cfg_info;
  son_cfg_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FirstDLCount-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using first_dl_count_ext_ies_o = protocol_ext_empty_o;

using first_dl_count_ext_ies_container = protocol_ext_container_empty_l;

// FirstDLCount ::= SEQUENCE
struct first_dl_count_s {
  bool                                         ext            = false;
  bool                                         ie_ext_present = false;
  drbs_subject_to_early_status_transfer_list_l drbs_subject_to_early_status_transfer;
  first_dl_count_ext_ies_container             ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ProcedureStageChoice-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using proc_stage_choice_ext_ies_o = protocol_ies_empty_o;

// EarlyStatusTransfer-TransparentContainer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using early_status_transfer_transparent_container_ext_ies_o = protocol_ext_empty_o;

// ProcedureStageChoice ::= CHOICE
struct proc_stage_choice_c {
  struct types_opts {
    enum options { first_dl_count, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  proc_stage_choice_c() = default;
  proc_stage_choice_c(const proc_stage_choice_c& other);
  proc_stage_choice_c& operator=(const proc_stage_choice_c& other);
  ~proc_stage_choice_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  first_dl_count_s& first_dl_count()
  {
    assert_choice_type(types::first_dl_count, type_, "ProcedureStageChoice");
    return c.get<first_dl_count_s>();
  }
  protocol_ie_single_container_s<proc_stage_choice_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "ProcedureStageChoice");
    return c.get<protocol_ie_single_container_s<proc_stage_choice_ext_ies_o>>();
  }
  const first_dl_count_s& first_dl_count() const
  {
    assert_choice_type(types::first_dl_count, type_, "ProcedureStageChoice");
    return c.get<first_dl_count_s>();
  }
  const protocol_ie_single_container_s<proc_stage_choice_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "ProcedureStageChoice");
    return c.get<protocol_ie_single_container_s<proc_stage_choice_ext_ies_o>>();
  }
  first_dl_count_s&                                            set_first_dl_count();
  protocol_ie_single_container_s<proc_stage_choice_ext_ies_o>& set_choice_exts();

private:
  types                                                                                          type_;
  choice_buffer_t<first_dl_count_s, protocol_ie_single_container_s<proc_stage_choice_ext_ies_o>> c;

  void destroy_();
};

using early_status_transfer_transparent_container_ext_ies_container = protocol_ext_container_empty_l;

// EarlyStatusTransfer-TransparentContainer ::= SEQUENCE
struct early_status_transfer_transparent_container_s {
  bool                                                          ext             = false;
  bool                                                          ie_exts_present = false;
  proc_stage_choice_c                                           proc_stage;
  early_status_transfer_transparent_container_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANStatusTransfer-TransparentContainer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ran_status_transfer_transparent_container_ext_ies_o = protocol_ext_empty_o;

using ran_status_transfer_transparent_container_ext_ies_container = protocol_ext_container_empty_l;

// RANStatusTransfer-TransparentContainer ::= SEQUENCE
struct ran_status_transfer_transparent_container_s {
  bool                                                        ext             = false;
  bool                                                        ie_exts_present = false;
  drbs_subject_to_status_transfer_list_l                      drbs_subject_to_status_transfer_list;
  ran_status_transfer_transparent_container_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RIMInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using rim_info_ext_ies_o = protocol_ext_empty_o;

// TargetRANNodeID-RIM-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using target_ran_node_id_rim_ext_ies_o = protocol_ext_empty_o;

using rim_info_ext_ies_container = protocol_ext_container_empty_l;

// RIMInformation ::= SEQUENCE
struct rim_info_s {
  struct rim_rs_detection_opts {
    enum options { rs_detected, rs_disappeared, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using rim_rs_detection_e_ = enumerated<rim_rs_detection_opts, true>;

  // member variables
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_bitstring<22, false, true> target_gnb_set_id;
  rim_rs_detection_e_              rim_rs_detection;
  rim_info_ext_ies_container       ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RIMInformationTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using rim_info_transfer_ext_ies_o = protocol_ext_empty_o;

using target_ran_node_id_rim_ext_ies_container = protocol_ext_container_empty_l;

// TargetRANNodeID-RIM ::= SEQUENCE
struct target_ran_node_id_rim_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  global_ran_node_id_c                     global_ran_node_id;
  tai_s                                    sel_tai;
  target_ran_node_id_rim_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using rim_info_transfer_ext_ies_container = protocol_ext_container_empty_l;

// RIMInformationTransfer ::= SEQUENCE
struct rim_info_transfer_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  target_ran_node_id_rim_s            target_ran_node_id_rim;
  source_ran_node_id_s                source_ran_node_id;
  rim_info_s                          rim_info;
  rim_info_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DelayCritical ::= ENUMERATED
struct delay_crit_opts {
  enum options { delay_crit, non_delay_crit, /*...*/ nulltype } value;

  const char* to_string() const;
};
using delay_crit_e = enumerated<delay_crit_opts, true>;

// Dynamic5QIDescriptor-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
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
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
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
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
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
  bool                                 ext                           = false;
  bool                                 five_qi_present               = false;
  bool                                 delay_crit_present            = false;
  bool                                 averaging_win_present         = false;
  bool                                 max_data_burst_volume_present = false;
  bool                                 ie_exts_present               = false;
  uint8_t                              prio_level_qos                = 1;
  uint16_t                             packet_delay_budget           = 0;
  packet_error_rate_s                  packet_error_rate;
  uint16_t                             five_qi = 0;
  delay_crit_e                         delay_crit;
  uint16_t                             averaging_win         = 0;
  uint16_t                             max_data_burst_volume = 0;
  dyn_5qi_descriptor_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLForwarding ::= ENUMERATED
struct dl_forwarding_opts {
  enum options { dl_forwarding_proposed, /*...*/ nulltype } value;

  const char* to_string() const;
};
using dl_forwarding_e = enumerated<dl_forwarding_opts, true>;

// E-RABInformationItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct erab_info_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { source_tnla_ddr_info, source_node_tnla_ddr_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bounded_bitstring<1, 160, true, true>&       source_tnla_ddr_info();
    bounded_bitstring<1, 160, true, true>&       source_node_tnla_ddr_info();
    const bounded_bitstring<1, 160, true, true>& source_tnla_ddr_info() const;
    const bounded_bitstring<1, 160, true, true>& source_node_tnla_ddr_info() const;

  private:
    types                                                  type_;
    choice_buffer_t<bounded_bitstring<1, 160, true, true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct erab_info_item_ext_ies_container {
  bool                                  source_tnla_ddr_info_present      = false;
  bool                                  source_node_tnla_ddr_info_present = false;
  bounded_bitstring<1, 160, true, true> source_tnla_ddr_info;
  bounded_bitstring<1, 160, true, true> source_node_tnla_ddr_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABInformationItem ::= SEQUENCE
struct erab_info_item_s {
  bool                             ext                   = false;
  bool                             dl_forwarding_present = false;
  bool                             ie_exts_present       = false;
  uint8_t                          erab_id               = 0;
  dl_forwarding_e                  dl_forwarding;
  erab_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-RABInformationList ::= SEQUENCE (SIZE (1..256)) OF E-RABInformationItem
using erab_info_list_l = dyn_array<erab_info_item_s>;

// EUTRA-CGIListForWarning ::= SEQUENCE (SIZE (1..65535)) OF EUTRA-CGI
using eutra_cgi_list_for_warning_l = dyn_array<eutra_cgi_s>;

// EmergencyAreaIDList ::= SEQUENCE (SIZE (1..65535)) OF OCTET STRING (SIZE (3))
using emergency_area_id_list_l = dyn_array<fixed_octstring<3, true>>;

// EmergencyAreaIDListForRestart ::= SEQUENCE (SIZE (1..256)) OF OCTET STRING (SIZE (3))
using emergency_area_id_list_for_restart_l = dyn_array<fixed_octstring<3, true>>;

// EmergencyFallbackIndicator-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using emergency_fallback_ind_ext_ies_o = protocol_ext_empty_o;

// EmergencyFallbackRequestIndicator ::= ENUMERATED
struct emergency_fallback_request_ind_opts {
  enum options { emergency_fallback_requested, /*...*/ nulltype } value;

  const char* to_string() const;
};
using emergency_fallback_request_ind_e = enumerated<emergency_fallback_request_ind_opts, true>;

// EmergencyServiceTargetCN ::= ENUMERATED
struct emergency_service_target_cn_opts {
  enum options { five_gc, epc, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using emergency_service_target_cn_e = enumerated<emergency_service_target_cn_opts, true>;

using emergency_fallback_ind_ext_ies_container = protocol_ext_container_empty_l;

// EmergencyFallbackIndicator ::= SEQUENCE
struct emergency_fallback_ind_s {
  bool                                     ext                                 = false;
  bool                                     emergency_service_target_cn_present = false;
  bool                                     ie_exts_present                     = false;
  emergency_fallback_request_ind_e         emergency_fallback_request_ind;
  emergency_service_target_cn_e            emergency_service_target_cn;
  emergency_fallback_ind_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FiveG-S-TMSI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using five_g_s_tmsi_ext_ies_o = protocol_ext_empty_o;

using five_g_s_tmsi_ext_ies_container = protocol_ext_container_empty_l;

// FiveG-S-TMSI ::= SEQUENCE
struct five_g_s_tmsi_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_bitstring<10, false, true> amf_set_id;
  fixed_bitstring<6, false, true>  amf_pointer;
  fixed_octstring<4, true>         five_g_tmsi;
  five_g_s_tmsi_ext_ies_container  ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementThresholdL1LoggedMDT-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using meas_thres_l1_logged_mdt_ext_ies_o = protocol_ies_empty_o;

// EventL1LoggedMDTConfig-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using event_l1_logged_mdt_cfg_ext_ies_o = protocol_ext_empty_o;

// MeasurementThresholdL1LoggedMDT ::= CHOICE
struct meas_thres_l1_logged_mdt_c {
  struct types_opts {
    enum options { thres_rsrp, thres_rsrq, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  meas_thres_l1_logged_mdt_c() = default;
  meas_thres_l1_logged_mdt_c(const meas_thres_l1_logged_mdt_c& other);
  meas_thres_l1_logged_mdt_c& operator=(const meas_thres_l1_logged_mdt_c& other);
  ~meas_thres_l1_logged_mdt_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& thres_rsrp()
  {
    assert_choice_type(types::thres_rsrp, type_, "MeasurementThresholdL1LoggedMDT");
    return c.get<uint8_t>();
  }
  uint8_t& thres_rsrq()
  {
    assert_choice_type(types::thres_rsrq, type_, "MeasurementThresholdL1LoggedMDT");
    return c.get<uint8_t>();
  }
  protocol_ie_single_container_s<meas_thres_l1_logged_mdt_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "MeasurementThresholdL1LoggedMDT");
    return c.get<protocol_ie_single_container_s<meas_thres_l1_logged_mdt_ext_ies_o>>();
  }
  const uint8_t& thres_rsrp() const
  {
    assert_choice_type(types::thres_rsrp, type_, "MeasurementThresholdL1LoggedMDT");
    return c.get<uint8_t>();
  }
  const uint8_t& thres_rsrq() const
  {
    assert_choice_type(types::thres_rsrq, type_, "MeasurementThresholdL1LoggedMDT");
    return c.get<uint8_t>();
  }
  const protocol_ie_single_container_s<meas_thres_l1_logged_mdt_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "MeasurementThresholdL1LoggedMDT");
    return c.get<protocol_ie_single_container_s<meas_thres_l1_logged_mdt_ext_ies_o>>();
  }
  uint8_t&                                                            set_thres_rsrp();
  uint8_t&                                                            set_thres_rsrq();
  protocol_ie_single_container_s<meas_thres_l1_logged_mdt_ext_ies_o>& set_choice_exts();

private:
  types                                                                               type_;
  choice_buffer_t<protocol_ie_single_container_s<meas_thres_l1_logged_mdt_ext_ies_o>> c;

  void destroy_();
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
using time_to_trigger_e = enumerated<time_to_trigger_opts>;

using event_l1_logged_mdt_cfg_ext_ies_container = protocol_ext_container_empty_l;

// EventL1LoggedMDTConfig ::= SEQUENCE
struct event_l1_logged_mdt_cfg_s {
  bool                                      ext             = false;
  bool                                      ie_exts_present = false;
  meas_thres_l1_logged_mdt_c                l1_thres;
  uint8_t                                   hysteresis = 0;
  time_to_trigger_e                         time_to_trigger;
  event_l1_logged_mdt_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTrigger-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using event_trigger_ext_ies_o = protocol_ies_empty_o;

// EventTrigger ::= CHOICE
struct event_trigger_c {
  struct out_of_coverage_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using out_of_coverage_e_ = enumerated<out_of_coverage_opts, true>;
  struct types_opts {
    enum options { out_of_coverage, event_l1_logged_mdt_cfg, choice_exts, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  event_trigger_c() = default;
  event_trigger_c(const event_trigger_c& other);
  event_trigger_c& operator=(const event_trigger_c& other);
  ~event_trigger_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  out_of_coverage_e_& out_of_coverage()
  {
    assert_choice_type(types::out_of_coverage, type_, "EventTrigger");
    return c.get<out_of_coverage_e_>();
  }
  event_l1_logged_mdt_cfg_s& event_l1_logged_mdt_cfg()
  {
    assert_choice_type(types::event_l1_logged_mdt_cfg, type_, "EventTrigger");
    return c.get<event_l1_logged_mdt_cfg_s>();
  }
  protocol_ie_single_container_s<event_trigger_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "EventTrigger");
    return c.get<protocol_ie_single_container_s<event_trigger_ext_ies_o>>();
  }
  const out_of_coverage_e_& out_of_coverage() const
  {
    assert_choice_type(types::out_of_coverage, type_, "EventTrigger");
    return c.get<out_of_coverage_e_>();
  }
  const event_l1_logged_mdt_cfg_s& event_l1_logged_mdt_cfg() const
  {
    assert_choice_type(types::event_l1_logged_mdt_cfg, type_, "EventTrigger");
    return c.get<event_l1_logged_mdt_cfg_s>();
  }
  const protocol_ie_single_container_s<event_trigger_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "EventTrigger");
    return c.get<protocol_ie_single_container_s<event_trigger_ext_ies_o>>();
  }
  out_of_coverage_e_&                                      set_out_of_coverage();
  event_l1_logged_mdt_cfg_s&                               set_event_l1_logged_mdt_cfg();
  protocol_ie_single_container_s<event_trigger_ext_ies_o>& set_choice_exts();

private:
  types                                                                                               type_;
  choice_buffer_t<event_l1_logged_mdt_cfg_s, protocol_ie_single_container_s<event_trigger_ext_ies_o>> c;

  void destroy_();
};

// ExcessPacketDelayThresholdItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using excess_packet_delay_thres_item_ext_ies_o = protocol_ext_empty_o;

// ExcessPacketDelayThresholdValue ::= ENUMERATED
struct excess_packet_delay_thres_value_opts {
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
    // ...
    nulltype
  } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using excess_packet_delay_thres_value_e = enumerated<excess_packet_delay_thres_value_opts, true>;

using excess_packet_delay_thres_item_ext_ies_container = protocol_ext_container_empty_l;

// ExcessPacketDelayThresholdItem ::= SEQUENCE
struct excess_packet_delay_thres_item_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  uint16_t                                         five_qi         = 0;
  excess_packet_delay_thres_value_e                excess_packet_delay_thres_value;
  excess_packet_delay_thres_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ExcessPacketDelayThresholdConfiguration ::= SEQUENCE (SIZE (1..255)) OF ExcessPacketDelayThresholdItem
using excess_packet_delay_thres_cfg_l = dyn_array<excess_packet_delay_thres_item_s>;

// Extended-RANNodeName-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using extended_ran_node_name_ext_ies_o = protocol_ext_empty_o;

using extended_ran_node_name_ext_ies_container = protocol_ext_container_empty_l;

// Extended-RANNodeName ::= SEQUENCE
struct extended_ran_node_name_s {
  bool                                     ext                                  = false;
  bool                                     ran_node_name_visible_string_present = false;
  bool                                     ran_node_name_utf8_string_present    = false;
  bool                                     ie_exts_present                      = false;
  visible_string<1, 150, true, true>       ran_node_name_visible_string;
  utf8_string<1, 150, true, true>          ran_node_name_utf8_string;
  extended_ran_node_name_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FiveG-ProSeAuthorized-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using five_g_pro_se_authorized_ext_ies_o = protocol_ext_empty_o;

// FiveGProSeDirectCommunication ::= ENUMERATED
struct five_g_pro_se_direct_communication_opts {
  enum options { authorized, not_authorized, /*...*/ nulltype } value;

  const char* to_string() const;
};
using five_g_pro_se_direct_communication_e = enumerated<five_g_pro_se_direct_communication_opts, true>;

// FiveGProSeDirectDiscovery ::= ENUMERATED
struct five_g_pro_se_direct_discovery_opts {
  enum options { authorized, not_authorized, /*...*/ nulltype } value;

  const char* to_string() const;
};
using five_g_pro_se_direct_discovery_e = enumerated<five_g_pro_se_direct_discovery_opts, true>;

// FiveGProSeLayer2RemoteUE ::= ENUMERATED
struct five_g_pro_se_layer2_remote_ue_opts {
  enum options { authorized, not_authorized, /*...*/ nulltype } value;

  const char* to_string() const;
};
using five_g_pro_se_layer2_remote_ue_e = enumerated<five_g_pro_se_layer2_remote_ue_opts, true>;

// FiveGProSeLayer2UEtoNetworkRelay ::= ENUMERATED
struct five_g_pro_se_layer2_ue_to_network_relay_opts {
  enum options { authorized, not_authorized, /*...*/ nulltype } value;

  const char* to_string() const;
};
using five_g_pro_se_layer2_ue_to_network_relay_e = enumerated<five_g_pro_se_layer2_ue_to_network_relay_opts, true>;

// FiveGProSeLayer3UEtoNetworkRelay ::= ENUMERATED
struct five_g_pro_se_layer3_ue_to_network_relay_opts {
  enum options { authorized, not_authorized, /*...*/ nulltype } value;

  const char* to_string() const;
};
using five_g_pro_se_layer3_ue_to_network_relay_e = enumerated<five_g_pro_se_layer3_ue_to_network_relay_opts, true>;

using five_g_pro_se_authorized_ext_ies_container = protocol_ext_container_empty_l;

// FiveG-ProSeAuthorized ::= SEQUENCE
struct five_g_pro_se_authorized_s {
  bool                                       ext                                              = false;
  bool                                       five_g_pro_se_direct_discovery_present           = false;
  bool                                       five_g_pro_se_direct_communication_present       = false;
  bool                                       five_g_pro_se_layer2_ue_to_network_relay_present = false;
  bool                                       five_g_pro_se_layer3_ue_to_network_relay_present = false;
  bool                                       five_g_pro_se_layer2_remote_ue_present           = false;
  bool                                       ie_exts_present                                  = false;
  five_g_pro_se_direct_discovery_e           five_g_pro_se_direct_discovery;
  five_g_pro_se_direct_communication_e       five_g_pro_se_direct_communication;
  five_g_pro_se_layer2_ue_to_network_relay_e five_g_pro_se_layer2_ue_to_network_relay;
  five_g_pro_se_layer3_ue_to_network_relay_e five_g_pro_se_layer3_ue_to_network_relay;
  five_g_pro_se_layer2_remote_ue_e           five_g_pro_se_layer2_remote_ue;
  five_g_pro_se_authorized_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FiveGProSePC5FlowBitRates-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using five_g_pro_se_pc5_flow_bit_rates_ext_ies_o = protocol_ext_empty_o;

using five_g_pro_se_pc5_flow_bit_rates_ext_ies_container = protocol_ext_container_empty_l;

// FiveGProSePC5FlowBitRates ::= SEQUENCE
struct five_g_pro_se_pc5_flow_bit_rates_s {
  bool                                               ext                                  = false;
  bool                                               ie_exts_present                      = false;
  uint64_t                                           five_gpro_seguaranteed_flow_bit_rate = 0;
  uint64_t                                           five_gpro_semax_flow_bit_rate        = 0;
  five_g_pro_se_pc5_flow_bit_rates_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FiveGProSePC5QoSFlowItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using five_g_pro_se_pc5_qos_flow_item_ext_ies_o = protocol_ext_empty_o;

// Range ::= ENUMERATED
struct range_opts {
  enum options { m50, m80, m180, m200, m350, m400, m500, m700, m1000, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using range_e = enumerated<range_opts, true>;

using five_g_pro_se_pc5_qos_flow_item_ext_ies_container = protocol_ext_container_empty_l;

// FiveGProSePC5QoSFlowItem ::= SEQUENCE
struct five_g_pro_se_pc5_qos_flow_item_s {
  bool                                              ext                                    = false;
  bool                                              five_gpro_sepc5_flow_bit_rates_present = false;
  bool                                              five_gpro_serange_present              = false;
  bool                                              ie_exts_present                        = false;
  uint16_t                                          five_gpro_sep_qi                       = 0;
  five_g_pro_se_pc5_flow_bit_rates_s                five_gpro_sepc5_flow_bit_rates;
  range_e                                           five_gpro_serange;
  five_g_pro_se_pc5_qos_flow_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FiveG-ProSePC5QoSParameters-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using five_g_pro_se_pc5_qos_params_ext_ies_o = protocol_ext_empty_o;

// FiveGProSePC5QoSFlowList ::= SEQUENCE (SIZE (1..2048)) OF FiveGProSePC5QoSFlowItem
using five_g_pro_se_pc5_qos_flow_list_l = dyn_array<five_g_pro_se_pc5_qos_flow_item_s>;

using five_g_pro_se_pc5_qos_params_ext_ies_container = protocol_ext_container_empty_l;

// FiveG-ProSePC5QoSParameters ::= SEQUENCE
struct five_g_pro_se_pc5_qos_params_s {
  bool                                           ext                                          = false;
  bool                                           five_g_pro_sepc5_link_aggr_bit_rates_present = false;
  bool                                           ie_exts_present                              = false;
  five_g_pro_se_pc5_qos_flow_list_l              five_g_pro_sepc5_qos_flow_list;
  uint64_t                                       five_g_pro_sepc5_link_aggr_bit_rates = 0;
  five_g_pro_se_pc5_qos_params_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GBR-QosInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct gbr_qos_info_ext_ies_o {
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

// NotificationControl ::= ENUMERATED
struct notif_ctrl_opts {
  enum options { notif_requested, /*...*/ nulltype } value;

  const char* to_string() const;
};
using notif_ctrl_e = enumerated<notif_ctrl_opts, true>;

// GBR-QosInformation ::= SEQUENCE
struct gbr_qos_info_s {
  bool                                             ext                             = false;
  bool                                             notif_ctrl_present              = false;
  bool                                             max_packet_loss_rate_dl_present = false;
  bool                                             max_packet_loss_rate_ul_present = false;
  uint64_t                                         max_flow_bit_rate_dl            = 0;
  uint64_t                                         max_flow_bit_rate_ul            = 0;
  uint64_t                                         guaranteed_flow_bit_rate_dl     = 0;
  uint64_t                                         guaranteed_flow_bit_rate_ul     = 0;
  notif_ctrl_e                                     notif_ctrl;
  uint16_t                                         max_packet_loss_rate_dl = 0;
  uint16_t                                         max_packet_loss_rate_ul = 0;
  protocol_ext_container_l<gbr_qos_info_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalCable-ID-new-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using global_cable_id_new_ext_ies_o = protocol_ext_empty_o;

using global_cable_id_new_ext_ies_container = protocol_ext_container_empty_l;

// GlobalCable-ID-new ::= SEQUENCE
struct global_cable_id_new_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  unbounded_octstring<true>             global_cable_id;
  tai_s                                 tai;
  global_cable_id_new_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalLine-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct global_line_id_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { tai, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::tai; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    tai_s&       tai() { return c; }
    const tai_s& tai() const { return c; }

  private:
    tai_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// LineType ::= ENUMERATED
struct line_type_opts {
  enum options { dsl, pon, /*...*/ nulltype } value;

  const char* to_string() const;
};
using line_type_e = enumerated<line_type_opts, true>;

// GlobalLine-ID ::= SEQUENCE
struct global_line_id_s {
  bool                                               ext               = false;
  bool                                               line_type_present = false;
  unbounded_octstring<true>                          global_line_id;
  line_type_e                                        line_type;
  protocol_ext_container_l<global_line_id_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HFCNode-ID-new-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using hfc_node_id_new_ext_ies_o = protocol_ext_empty_o;

using hfc_node_id_new_ext_ies_container = protocol_ext_container_empty_l;

// HFCNode-ID-new ::= SEQUENCE
struct hfc_node_id_new_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  unbounded_octstring<true>         hfc_node_id;
  tai_s                             tai;
  hfc_node_id_new_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceHandoverItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_ho_item_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceToReleaseItemHOCmd-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_to_release_item_ho_cmd_ext_ies_o = protocol_ext_empty_o;

using pdu_session_res_ho_item_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceHandoverItem ::= SEQUENCE
struct pdu_session_res_ho_item_s {
  bool                                      ext             = false;
  bool                                      ie_exts_present = false;
  uint16_t                                  pdu_session_id  = 0;
  unbounded_octstring<true>                 ho_cmd_transfer;
  pdu_session_res_ho_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_to_release_item_ho_cmd_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceToReleaseItemHOCmd ::= SEQUENCE
struct pdu_session_res_to_release_item_ho_cmd_s {
  bool                                                     ext             = false;
  bool                                                     ie_exts_present = false;
  uint16_t                                                 pdu_session_id  = 0;
  unbounded_octstring<true>                                ho_prep_unsuccessful_transfer;
  pdu_session_res_to_release_item_ho_cmd_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverType ::= ENUMERATED
struct handov_type_opts {
  enum options { intra5gs, fivegs_to_eps, eps_to_5gs, /*...*/ fivegs_to_utran, nulltype } value;

  const char* to_string() const;
};
using handov_type_e = enumerated<handov_type_opts, true, 1>;

// PDUSessionResourceHandoverList ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceHandoverItem
using pdu_session_res_ho_list_l = dyn_array<pdu_session_res_ho_item_s>;

// PDUSessionResourceToReleaseListHOCmd ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceToReleaseItemHOCmd
using pdu_session_res_to_release_list_ho_cmd_l = dyn_array<pdu_session_res_to_release_item_ho_cmd_s>;

// QosFlowPerTNLInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using qos_flow_per_tnl_info_ext_ies_o = protocol_ext_empty_o;

using qos_flow_per_tnl_info_ext_ies_container = protocol_ext_container_empty_l;

// QosFlowPerTNLInformation ::= SEQUENCE
struct qos_flow_per_tnl_info_s {
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  up_transport_layer_info_c               up_transport_layer_info;
  associated_qos_flow_list_l              associated_qos_flow_list;
  qos_flow_per_tnl_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowPerTNLInformationItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using qos_flow_per_tnl_info_item_ext_ies_o = protocol_ext_empty_o;

// QosFlowWithCauseItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using qos_flow_with_cause_item_ext_ies_o = protocol_ext_empty_o;

// UPTransportLayerInformationItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct up_transport_layer_info_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { common_network_instance, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::common_network_instance; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    unbounded_octstring<true>&       common_network_instance() { return c; }
    const unbounded_octstring<true>& common_network_instance() const { return c; }

  private:
    unbounded_octstring<true> c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using qos_flow_per_tnl_info_item_ext_ies_container = protocol_ext_container_empty_l;

// QosFlowPerTNLInformationItem ::= SEQUENCE
struct qos_flow_per_tnl_info_item_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  qos_flow_per_tnl_info_s                      qos_flow_per_tnl_info;
  qos_flow_per_tnl_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowToBeForwardedItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using qos_flow_to_be_forwarded_item_ext_ies_o = protocol_ext_empty_o;

using qos_flow_with_cause_item_ext_ies_container = protocol_ext_container_empty_l;

// QosFlowWithCauseItem ::= SEQUENCE
struct qos_flow_with_cause_item_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  uint8_t                                    qos_flow_id     = 0;
  cause_c                                    cause;
  qos_flow_with_cause_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UPTransportLayerInformationItem ::= SEQUENCE
struct up_transport_layer_info_item_s {
  bool                                                             ext = false;
  up_transport_layer_info_c                                        ngu_up_tnl_info;
  protocol_ext_container_l<up_transport_layer_info_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowListWithCause ::= SEQUENCE (SIZE (1..64)) OF QosFlowWithCauseItem
using qos_flow_list_with_cause_l = dyn_array<qos_flow_with_cause_item_s>;

// QosFlowPerTNLInformationList ::= SEQUENCE (SIZE (1..3)) OF QosFlowPerTNLInformationItem
using qos_flow_per_tnl_info_list_l = dyn_array<qos_flow_per_tnl_info_item_s>;

using qos_flow_to_be_forwarded_item_ext_ies_container = protocol_ext_container_empty_l;

// QosFlowToBeForwardedItem ::= SEQUENCE
struct qos_flow_to_be_forwarded_item_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  uint8_t                                         qos_flow_id     = 0;
  qos_flow_to_be_forwarded_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UPTransportLayerInformationList ::= SEQUENCE (SIZE (1..3)) OF UPTransportLayerInformationItem
using up_transport_layer_info_list_l = dyn_array<up_transport_layer_info_item_s>;

// HandoverCommandTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct ho_cmd_transfer_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        add_dl_forwarding_up_tnl_info,
        ul_forwarding_up_tnl_info,
        add_ul_forwarding_up_tnl_info,
        data_forwarding_resp_erab_list,
        qos_flow_failed_to_setup_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_flow_per_tnl_info_list_l&           add_dl_forwarding_up_tnl_info();
    up_transport_layer_info_c&              ul_forwarding_up_tnl_info();
    up_transport_layer_info_list_l&         add_ul_forwarding_up_tnl_info();
    data_forwarding_resp_erab_list_l&       data_forwarding_resp_erab_list();
    qos_flow_list_with_cause_l&             qos_flow_failed_to_setup_list();
    const qos_flow_per_tnl_info_list_l&     add_dl_forwarding_up_tnl_info() const;
    const up_transport_layer_info_c&        ul_forwarding_up_tnl_info() const;
    const up_transport_layer_info_list_l&   add_ul_forwarding_up_tnl_info() const;
    const data_forwarding_resp_erab_list_l& data_forwarding_resp_erab_list() const;
    const qos_flow_list_with_cause_l&       qos_flow_failed_to_setup_list() const;

  private:
    types type_;
    choice_buffer_t<data_forwarding_resp_erab_list_l,
                    qos_flow_list_with_cause_l,
                    qos_flow_per_tnl_info_list_l,
                    up_transport_layer_info_c,
                    up_transport_layer_info_list_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// QosFlowToBeForwardedList ::= SEQUENCE (SIZE (1..64)) OF QosFlowToBeForwardedItem
using qos_flow_to_be_forwarded_list_l = dyn_array<qos_flow_to_be_forwarded_item_s>;

struct ho_cmd_transfer_ext_ies_container {
  bool                             add_dl_forwarding_up_tnl_info_present  = false;
  bool                             ul_forwarding_up_tnl_info_present      = false;
  bool                             add_ul_forwarding_up_tnl_info_present  = false;
  bool                             data_forwarding_resp_erab_list_present = false;
  bool                             qos_flow_failed_to_setup_list_present  = false;
  qos_flow_per_tnl_info_list_l     add_dl_forwarding_up_tnl_info;
  up_transport_layer_info_c        ul_forwarding_up_tnl_info;
  up_transport_layer_info_list_l   add_ul_forwarding_up_tnl_info;
  data_forwarding_resp_erab_list_l data_forwarding_resp_erab_list;
  qos_flow_list_with_cause_l       qos_flow_failed_to_setup_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverCommandTransfer ::= SEQUENCE
struct ho_cmd_transfer_s {
  bool                              ext                               = false;
  bool                              dl_forwarding_up_tnl_info_present = false;
  bool                              ie_exts_present                   = false;
  up_transport_layer_info_c         dl_forwarding_up_tnl_info;
  qos_flow_to_be_forwarded_list_l   qos_flow_to_be_forwarded_list;
  data_forwarding_resp_drb_list_l   data_forwarding_resp_drb_list;
  ho_cmd_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NRNTNTAIInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using nr_ntn_tai_info_ext_ies_o = protocol_ext_empty_o;

// TACListInNRNTN ::= SEQUENCE (SIZE (1..12)) OF OCTET STRING (SIZE (3))
using tac_list_in_nr_ntn_l = bounded_array<fixed_octstring<3, true>, 12>;

using nr_ntn_tai_info_ext_ies_container = protocol_ext_container_empty_l;

// NRNTNTAIInformation ::= SEQUENCE
struct nr_ntn_tai_info_s {
  bool                              ext                                       = false;
  bool                              ue_location_derived_tac_in_nr_ntn_present = false;
  bool                              ie_exts_present                           = false;
  fixed_octstring<3, true>          serving_plmn;
  tac_list_in_nr_ntn_l              tac_list_in_nr_ntn;
  fixed_octstring<3, true>          ue_location_derived_tac_in_nr_ntn;
  nr_ntn_tai_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UserLocationInformationTNGF-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct user_location_info_tngf_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { tai, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::tai; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    tai_s&       tai() { return c; }
    const tai_s& tai() const { return c; }

  private:
    tai_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UserLocationInformationTWIF-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct user_location_info_twif_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { tai, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::tai; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    tai_s&       tai() { return c; }
    const tai_s& tai() const { return c; }

  private:
    tai_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UserLocationInformationW-AGF-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct user_location_info_w_agf_ext_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { global_cable_id, hfc_node_id_new, global_cable_id_new, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    unbounded_octstring<true>&       global_cable_id();
    hfc_node_id_new_s&               hfc_node_id_new();
    global_cable_id_new_s&           global_cable_id_new();
    const unbounded_octstring<true>& global_cable_id() const;
    const hfc_node_id_new_s&         hfc_node_id_new() const;
    const global_cable_id_new_s&     global_cable_id_new() const;

  private:
    types                                                                                type_;
    choice_buffer_t<global_cable_id_new_s, hfc_node_id_new_s, unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UserLocationInformationEUTRA-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct user_location_info_eutra_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { pscell_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::pscell_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ngran_cgi_c&       pscell_info() { return c; }
    const ngran_cgi_c& pscell_info() const { return c; }

  private:
    ngran_cgi_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UserLocationInformationN3IWF-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct user_location_info_n3_iwf_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { tai, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::tai; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    tai_s&       tai() { return c; }
    const tai_s& tai() const { return c; }

  private:
    tai_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UserLocationInformationNR-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct user_location_info_nr_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { pscell_info, n_id, nr_ntn_tai_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ngran_cgi_c&                            pscell_info();
    fixed_bitstring<44, false, true>&       n_id();
    nr_ntn_tai_info_s&                      nr_ntn_tai_info();
    const ngran_cgi_c&                      pscell_info() const;
    const fixed_bitstring<44, false, true>& n_id() const;
    const nr_ntn_tai_info_s&                nr_ntn_tai_info() const;

  private:
    types                                                                             type_;
    choice_buffer_t<fixed_bitstring<44, false, true>, ngran_cgi_c, nr_ntn_tai_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UserLocationInformationTNGF ::= SEQUENCE
struct user_location_info_tngf_s {
  bool                                                        ext              = false;
  bool                                                        port_num_present = false;
  unbounded_octstring<true>                                   tnap_id;
  bounded_bitstring<1, 160, true, true>                       ip_address;
  fixed_octstring<2, true>                                    port_num;
  protocol_ext_container_l<user_location_info_tngf_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UserLocationInformationTWIF ::= SEQUENCE
struct user_location_info_twif_s {
  bool                                                        ext              = false;
  bool                                                        port_num_present = false;
  unbounded_octstring<true>                                   twap_id;
  bounded_bitstring<1, 160, true, true>                       ip_address;
  fixed_octstring<2, true>                                    port_num;
  protocol_ext_container_l<user_location_info_twif_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UserLocationInformationW-AGF ::= CHOICE
struct user_location_info_w_agf_c {
  struct types_opts {
    enum options { global_line_id, hfc_node_id, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  user_location_info_w_agf_c() = default;
  user_location_info_w_agf_c(const user_location_info_w_agf_c& other);
  user_location_info_w_agf_c& operator=(const user_location_info_w_agf_c& other);
  ~user_location_info_w_agf_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  global_line_id_s& global_line_id()
  {
    assert_choice_type(types::global_line_id, type_, "UserLocationInformationW-AGF");
    return c.get<global_line_id_s>();
  }
  unbounded_octstring<true>& hfc_node_id()
  {
    assert_choice_type(types::hfc_node_id, type_, "UserLocationInformationW-AGF");
    return c.get<unbounded_octstring<true>>();
  }
  protocol_ie_single_container_s<user_location_info_w_agf_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "UserLocationInformationW-AGF");
    return c.get<protocol_ie_single_container_s<user_location_info_w_agf_ext_ies_o>>();
  }
  const global_line_id_s& global_line_id() const
  {
    assert_choice_type(types::global_line_id, type_, "UserLocationInformationW-AGF");
    return c.get<global_line_id_s>();
  }
  const unbounded_octstring<true>& hfc_node_id() const
  {
    assert_choice_type(types::hfc_node_id, type_, "UserLocationInformationW-AGF");
    return c.get<unbounded_octstring<true>>();
  }
  const protocol_ie_single_container_s<user_location_info_w_agf_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "UserLocationInformationW-AGF");
    return c.get<protocol_ie_single_container_s<user_location_info_w_agf_ext_ies_o>>();
  }
  global_line_id_s&                                                   set_global_line_id();
  unbounded_octstring<true>&                                          set_hfc_node_id();
  protocol_ie_single_container_s<user_location_info_w_agf_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<global_line_id_s,
                  protocol_ie_single_container_s<user_location_info_w_agf_ext_ies_o>,
                  unbounded_octstring<true>>
      c;

  void destroy_();
};

// UserLocationInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct user_location_info_ext_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { user_location_info_tngf, user_location_info_twif, user_location_info_w_agf, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    user_location_info_tngf_s&        user_location_info_tngf();
    user_location_info_twif_s&        user_location_info_twif();
    user_location_info_w_agf_c&       user_location_info_w_agf();
    const user_location_info_tngf_s&  user_location_info_tngf() const;
    const user_location_info_twif_s&  user_location_info_twif() const;
    const user_location_info_w_agf_c& user_location_info_w_agf() const;

  private:
    types                                                                                             type_;
    choice_buffer_t<user_location_info_tngf_s, user_location_info_twif_s, user_location_info_w_agf_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UserLocationInformationEUTRA ::= SEQUENCE
struct user_location_info_eutra_s {
  bool                                                         ext                = false;
  bool                                                         time_stamp_present = false;
  eutra_cgi_s                                                  eutra_cgi;
  tai_s                                                        tai;
  fixed_octstring<4, true>                                     time_stamp;
  protocol_ext_container_l<user_location_info_eutra_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UserLocationInformationN3IWF ::= SEQUENCE
struct user_location_info_n3_iwf_s {
  bool                                                          ext = false;
  bounded_bitstring<1, 160, true, true>                         ip_address;
  fixed_octstring<2, true>                                      port_num;
  protocol_ext_container_l<user_location_info_n3_iwf_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct user_location_info_nr_ext_ies_container {
  bool                             pscell_info_present     = false;
  bool                             n_id_present            = false;
  bool                             nr_ntn_tai_info_present = false;
  ngran_cgi_c                      pscell_info;
  fixed_bitstring<44, false, true> n_id;
  nr_ntn_tai_info_s                nr_ntn_tai_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UserLocationInformationNR ::= SEQUENCE
struct user_location_info_nr_s {
  bool                                    ext                = false;
  bool                                    time_stamp_present = false;
  bool                                    ie_exts_present    = false;
  nr_cgi_s                                nr_cgi;
  tai_s                                   tai;
  fixed_octstring<4, true>                time_stamp;
  user_location_info_nr_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NotifySourceNGRANNode ::= ENUMERATED
struct notify_source_ngran_node_opts {
  enum options { notify_source, /*...*/ nulltype } value;

  const char* to_string() const;
};
using notify_source_ngran_node_e = enumerated<notify_source_ngran_node_opts, true>;

// UserLocationInformation ::= CHOICE
struct user_location_info_c {
  struct types_opts {
    enum options {
      user_location_info_eutra,
      user_location_info_nr,
      user_location_info_n3_iwf,
      choice_exts,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  user_location_info_c() = default;
  user_location_info_c(const user_location_info_c& other);
  user_location_info_c& operator=(const user_location_info_c& other);
  ~user_location_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  user_location_info_eutra_s& user_location_info_eutra()
  {
    assert_choice_type(types::user_location_info_eutra, type_, "UserLocationInformation");
    return c.get<user_location_info_eutra_s>();
  }
  user_location_info_nr_s& user_location_info_nr()
  {
    assert_choice_type(types::user_location_info_nr, type_, "UserLocationInformation");
    return c.get<user_location_info_nr_s>();
  }
  user_location_info_n3_iwf_s& user_location_info_n3_iwf()
  {
    assert_choice_type(types::user_location_info_n3_iwf, type_, "UserLocationInformation");
    return c.get<user_location_info_n3_iwf_s>();
  }
  protocol_ie_single_container_s<user_location_info_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "UserLocationInformation");
    return c.get<protocol_ie_single_container_s<user_location_info_ext_ies_o>>();
  }
  const user_location_info_eutra_s& user_location_info_eutra() const
  {
    assert_choice_type(types::user_location_info_eutra, type_, "UserLocationInformation");
    return c.get<user_location_info_eutra_s>();
  }
  const user_location_info_nr_s& user_location_info_nr() const
  {
    assert_choice_type(types::user_location_info_nr, type_, "UserLocationInformation");
    return c.get<user_location_info_nr_s>();
  }
  const user_location_info_n3_iwf_s& user_location_info_n3_iwf() const
  {
    assert_choice_type(types::user_location_info_n3_iwf, type_, "UserLocationInformation");
    return c.get<user_location_info_n3_iwf_s>();
  }
  const protocol_ie_single_container_s<user_location_info_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "UserLocationInformation");
    return c.get<protocol_ie_single_container_s<user_location_info_ext_ies_o>>();
  }
  user_location_info_eutra_s&                                   set_user_location_info_eutra();
  user_location_info_nr_s&                                      set_user_location_info_nr();
  user_location_info_n3_iwf_s&                                  set_user_location_info_n3_iwf();
  protocol_ie_single_container_s<user_location_info_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<user_location_info_ext_ies_o>,
                  user_location_info_eutra_s,
                  user_location_info_n3_iwf_s,
                  user_location_info_nr_s>
      c;

  void destroy_();
};

// HandoverPreparationUnsuccessfulTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ho_prep_unsuccessful_transfer_ext_ies_o = protocol_ext_empty_o;

using ho_prep_unsuccessful_transfer_ext_ies_container = protocol_ext_container_empty_l;

// HandoverPreparationUnsuccessfulTransfer ::= SEQUENCE
struct ho_prep_unsuccessful_transfer_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  cause_c                                         cause;
  ho_prep_unsuccessful_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SensorNameConfig-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using sensor_name_cfg_ext_ies_o = protocol_ies_empty_o;

// ExtendedReportIntervalMDT ::= ENUMERATED
struct extended_report_interv_mdt_opts {
  enum options { ms20480, ms40960, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using extended_report_interv_mdt_e = enumerated<extended_report_interv_mdt_opts, true>;

// M1ThresholdType-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using m1_thres_type_ext_ies_o = protocol_ies_empty_o;

// SensorMeasConfigNameItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using sensor_meas_cfg_name_item_ext_ies_o = protocol_ext_empty_o;

// SensorNameConfig ::= CHOICE
struct sensor_name_cfg_c {
  struct uncompensated_barometric_cfg_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using uncompensated_barometric_cfg_e_ = enumerated<uncompensated_barometric_cfg_opts, true>;
  struct ue_speed_cfg_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ue_speed_cfg_e_ = enumerated<ue_speed_cfg_opts, true>;
  struct ue_orientation_cfg_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ue_orientation_cfg_e_ = enumerated<ue_orientation_cfg_opts, true>;
  struct types_opts {
    enum options { uncompensated_barometric_cfg, ue_speed_cfg, ue_orientation_cfg, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  sensor_name_cfg_c() = default;
  sensor_name_cfg_c(const sensor_name_cfg_c& other);
  sensor_name_cfg_c& operator=(const sensor_name_cfg_c& other);
  ~sensor_name_cfg_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uncompensated_barometric_cfg_e_& uncompensated_barometric_cfg()
  {
    assert_choice_type(types::uncompensated_barometric_cfg, type_, "SensorNameConfig");
    return c.get<uncompensated_barometric_cfg_e_>();
  }
  ue_speed_cfg_e_& ue_speed_cfg()
  {
    assert_choice_type(types::ue_speed_cfg, type_, "SensorNameConfig");
    return c.get<ue_speed_cfg_e_>();
  }
  ue_orientation_cfg_e_& ue_orientation_cfg()
  {
    assert_choice_type(types::ue_orientation_cfg, type_, "SensorNameConfig");
    return c.get<ue_orientation_cfg_e_>();
  }
  protocol_ie_single_container_s<sensor_name_cfg_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "SensorNameConfig");
    return c.get<protocol_ie_single_container_s<sensor_name_cfg_ext_ies_o>>();
  }
  const uncompensated_barometric_cfg_e_& uncompensated_barometric_cfg() const
  {
    assert_choice_type(types::uncompensated_barometric_cfg, type_, "SensorNameConfig");
    return c.get<uncompensated_barometric_cfg_e_>();
  }
  const ue_speed_cfg_e_& ue_speed_cfg() const
  {
    assert_choice_type(types::ue_speed_cfg, type_, "SensorNameConfig");
    return c.get<ue_speed_cfg_e_>();
  }
  const ue_orientation_cfg_e_& ue_orientation_cfg() const
  {
    assert_choice_type(types::ue_orientation_cfg, type_, "SensorNameConfig");
    return c.get<ue_orientation_cfg_e_>();
  }
  const protocol_ie_single_container_s<sensor_name_cfg_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "SensorNameConfig");
    return c.get<protocol_ie_single_container_s<sensor_name_cfg_ext_ies_o>>();
  }
  uncompensated_barometric_cfg_e_&                           set_uncompensated_barometric_cfg();
  ue_speed_cfg_e_&                                           set_ue_speed_cfg();
  ue_orientation_cfg_e_&                                     set_ue_orientation_cfg();
  protocol_ie_single_container_s<sensor_name_cfg_ext_ies_o>& set_choice_exts();

private:
  types                                                                      type_;
  choice_buffer_t<protocol_ie_single_container_s<sensor_name_cfg_ext_ies_o>> c;

  void destroy_();
};

// WLANMeasConfigNameItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using wlan_meas_cfg_name_item_ext_ies_o = protocol_ext_empty_o;

// IncludeBeamMeasurementsIndication ::= ENUMERATED
struct include_beam_meass_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using include_beam_meass_ind_e = enumerated<include_beam_meass_ind_opts, true>;

// M1PeriodicReporting-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct m1_periodic_report_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { extended_report_interv_mdt, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::extended_report_interv_mdt; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    extended_report_interv_mdt_e&       extended_report_interv_mdt() { return c; }
    const extended_report_interv_mdt_e& extended_report_interv_mdt() const { return c; }

  private:
    extended_report_interv_mdt_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// M1ThresholdEventA2-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using m1_thres_event_a2_ext_ies_o = protocol_ext_empty_o;

// M1ThresholdType ::= CHOICE
struct m1_thres_type_c {
  struct types_opts {
    enum options { thres_rsrp, thres_rsrq, thres_sinr, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  m1_thres_type_c() = default;
  m1_thres_type_c(const m1_thres_type_c& other);
  m1_thres_type_c& operator=(const m1_thres_type_c& other);
  ~m1_thres_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& thres_rsrp()
  {
    assert_choice_type(types::thres_rsrp, type_, "M1ThresholdType");
    return c.get<uint8_t>();
  }
  uint8_t& thres_rsrq()
  {
    assert_choice_type(types::thres_rsrq, type_, "M1ThresholdType");
    return c.get<uint8_t>();
  }
  uint8_t& thres_sinr()
  {
    assert_choice_type(types::thres_sinr, type_, "M1ThresholdType");
    return c.get<uint8_t>();
  }
  protocol_ie_single_container_s<m1_thres_type_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "M1ThresholdType");
    return c.get<protocol_ie_single_container_s<m1_thres_type_ext_ies_o>>();
  }
  const uint8_t& thres_rsrp() const
  {
    assert_choice_type(types::thres_rsrp, type_, "M1ThresholdType");
    return c.get<uint8_t>();
  }
  const uint8_t& thres_rsrq() const
  {
    assert_choice_type(types::thres_rsrq, type_, "M1ThresholdType");
    return c.get<uint8_t>();
  }
  const uint8_t& thres_sinr() const
  {
    assert_choice_type(types::thres_sinr, type_, "M1ThresholdType");
    return c.get<uint8_t>();
  }
  const protocol_ie_single_container_s<m1_thres_type_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "M1ThresholdType");
    return c.get<protocol_ie_single_container_s<m1_thres_type_ext_ies_o>>();
  }
  uint8_t&                                                 set_thres_rsrp();
  uint8_t&                                                 set_thres_rsrq();
  uint8_t&                                                 set_thres_sinr();
  protocol_ie_single_container_s<m1_thres_type_ext_ies_o>& set_choice_exts();

private:
  types                                                                    type_;
  choice_buffer_t<protocol_ie_single_container_s<m1_thres_type_ext_ies_o>> c;

  void destroy_();
};

// M4ReportAmountMDT ::= ENUMERATED
struct m4_report_amount_mdt_opts {
  enum options { r1, r2, r4, r8, r16, r32, r64, infinity, /*...*/ nulltype } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
using m4_report_amount_mdt_e = enumerated<m4_report_amount_mdt_opts, true>;

// M5ReportAmountMDT ::= ENUMERATED
struct m5_report_amount_mdt_opts {
  enum options { r1, r2, r4, r8, r16, r32, r64, infinity, /*...*/ nulltype } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
using m5_report_amount_mdt_e = enumerated<m5_report_amount_mdt_opts, true>;

// M6ReportAmountMDT ::= ENUMERATED
struct m6_report_amount_mdt_opts {
  enum options { r1, r2, r4, r8, r16, r32, r64, infinity, /*...*/ nulltype } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
using m6_report_amount_mdt_e = enumerated<m6_report_amount_mdt_opts, true>;

// M7ReportAmountMDT ::= ENUMERATED
struct m7_report_amount_mdt_opts {
  enum options { r1, r2, r4, r8, r16, r32, r64, infinity, /*...*/ nulltype } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
using m7_report_amount_mdt_e = enumerated<m7_report_amount_mdt_opts, true>;

// ReportAmountMDT ::= ENUMERATED
struct report_amount_mdt_opts {
  enum options { r1, r2, r4, r8, r16, r32, r64, rinfinity, nulltype } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
using report_amount_mdt_e = enumerated<report_amount_mdt_opts>;

// ReportIntervalMDT ::= ENUMERATED
struct report_interv_mdt_opts {
  enum options {
    ms120,
    ms240,
    ms480,
    ms640,
    ms1024,
    ms2048,
    ms5120,
    ms10240,
    min1,
    min6,
    min12,
    min30,
    min60,
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using report_interv_mdt_e = enumerated<report_interv_mdt_opts>;

using sensor_meas_cfg_name_item_ext_ies_container = protocol_ext_container_empty_l;

// SensorMeasConfigNameItem ::= SEQUENCE
struct sensor_meas_cfg_name_item_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  sensor_name_cfg_c                           sensor_name_cfg;
  sensor_meas_cfg_name_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using wlan_meas_cfg_name_item_ext_ies_container = protocol_ext_container_empty_l;

// WLANMeasConfigNameItem ::= SEQUENCE
struct wlan_meas_cfg_name_item_s {
  bool                                      ext             = false;
  bool                                      ie_exts_present = false;
  bounded_octstring<1, 32, true>            wlan_name;
  wlan_meas_cfg_name_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EarlyMeasurement ::= ENUMERATED
struct early_meas_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using early_meas_e = enumerated<early_meas_opts, true>;

// Links-to-log ::= ENUMERATED
struct links_to_log_opts {
  enum options { ul, dl, both_ul_and_dl, /*...*/ nulltype } value;

  const char* to_string() const;
};
using links_to_log_e = enumerated<links_to_log_opts, true>;

// LoggedMDTTrigger-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using logged_mdt_trigger_ext_ies_o = protocol_ies_empty_o;

// M1Configuration-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct m1_cfg_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { include_beam_meass_ind, beam_meass_report_cfg, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    include_beam_meass_ind_e&       include_beam_meass_ind();
    beam_meass_report_cfg_s&        beam_meass_report_cfg();
    const include_beam_meass_ind_e& include_beam_meass_ind() const;
    const beam_meass_report_cfg_s&  beam_meass_report_cfg() const;

  private:
    types                                    type_;
    choice_buffer_t<beam_meass_report_cfg_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// M1PeriodicReporting ::= SEQUENCE
struct m1_periodic_report_s {
  bool                                                   ext = false;
  report_interv_mdt_e                                    report_interv;
  report_amount_mdt_e                                    report_amount;
  protocol_ext_container_l<m1_periodic_report_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// M1ReportingTrigger ::= ENUMERATED
struct m1_report_trigger_opts {
  enum options { periodic, a2eventtriggered, a2eventtriggered_periodic, /*...*/ nulltype } value;

  const char* to_string() const;
};
using m1_report_trigger_e = enumerated<m1_report_trigger_opts, true>;

using m1_thres_event_a2_ext_ies_container = protocol_ext_container_empty_l;

// M1ThresholdEventA2 ::= SEQUENCE
struct m1_thres_event_a2_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  m1_thres_type_c                     m1_thres_type;
  m1_thres_event_a2_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// M4Configuration-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
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
    m4_report_amount_mdt_e&       m4_report_amount() { return c; }
    const m4_report_amount_mdt_e& m4_report_amount() const { return c; }

  private:
    m4_report_amount_mdt_e c;
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

// M5Configuration-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct m5_cfg_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { m5_report_amount, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::m5_report_amount; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    m5_report_amount_mdt_e&       m5_report_amount() { return c; }
    const m5_report_amount_mdt_e& m5_report_amount() const { return c; }

  private:
    m5_report_amount_mdt_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// M5period ::= ENUMERATED
struct m5period_opts {
  enum options { ms1024, ms2048, ms5120, ms10240, min1, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using m5period_e = enumerated<m5period_opts, true>;

// M6Configuration-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct m6_cfg_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { m6_report_amount, excess_packet_delay_thres_cfg, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    m6_report_amount_mdt_e&                m6_report_amount();
    excess_packet_delay_thres_cfg_l&       excess_packet_delay_thres_cfg();
    const m6_report_amount_mdt_e&          m6_report_amount() const;
    const excess_packet_delay_thres_cfg_l& excess_packet_delay_thres_cfg() const;

  private:
    types                                            type_;
    choice_buffer_t<excess_packet_delay_thres_cfg_l> c;

    void destroy_();
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

// M7Configuration-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
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
    m7_report_amount_mdt_e&       m7_report_amount() { return c; }
    const m7_report_amount_mdt_e& m7_report_amount() const { return c; }

  private:
    m7_report_amount_mdt_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MDT-Location-Info-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mdt_location_info_ext_ies_o = protocol_ext_empty_o;

// SensorMeasConfig ::= ENUMERATED
struct sensor_meas_cfg_opts {
  enum options { setup, /*...*/ nulltype } value;

  const char* to_string() const;
};
using sensor_meas_cfg_e = enumerated<sensor_meas_cfg_opts, true>;

// SensorMeasConfigNameList ::= SEQUENCE (SIZE (1..3)) OF SensorMeasConfigNameItem
using sensor_meas_cfg_name_list_l = dyn_array<sensor_meas_cfg_name_item_s>;

// SensorMeasurementConfiguration-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using sensor_meas_cfg_ext_ies_o = protocol_ext_empty_o;

// WLANMeasConfig ::= ENUMERATED
struct wlan_meas_cfg_opts {
  enum options { setup, /*...*/ nulltype } value;

  const char* to_string() const;
};
using wlan_meas_cfg_e = enumerated<wlan_meas_cfg_opts, true>;

// WLANMeasConfigNameList ::= SEQUENCE (SIZE (1..4)) OF WLANMeasConfigNameItem
using wlan_meas_cfg_name_list_l = dyn_array<wlan_meas_cfg_name_item_s>;

// WLANMeasurementConfiguration-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using wlan_meas_cfg_ext_ies_o = protocol_ext_empty_o;

// ImmediateMDTNr-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using immediate_mdt_nr_ext_ies_o = protocol_ext_empty_o;

// LoggedMDTNr-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct logged_mdt_nr_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { early_meas, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::early_meas; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    early_meas_e&       early_meas() { return c; }
    const early_meas_e& early_meas() const { return c; }

  private:
    early_meas_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// LoggedMDTTrigger ::= CHOICE
struct logged_mdt_trigger_c {
  struct types_opts {
    enum options { periodical, event_trigger, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  logged_mdt_trigger_c() = default;
  logged_mdt_trigger_c(const logged_mdt_trigger_c& other);
  logged_mdt_trigger_c& operator=(const logged_mdt_trigger_c& other);
  ~logged_mdt_trigger_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  event_trigger_c& event_trigger()
  {
    assert_choice_type(types::event_trigger, type_, "LoggedMDTTrigger");
    return c.get<event_trigger_c>();
  }
  protocol_ie_single_container_s<logged_mdt_trigger_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "LoggedMDTTrigger");
    return c.get<protocol_ie_single_container_s<logged_mdt_trigger_ext_ies_o>>();
  }
  const event_trigger_c& event_trigger() const
  {
    assert_choice_type(types::event_trigger, type_, "LoggedMDTTrigger");
    return c.get<event_trigger_c>();
  }
  const protocol_ie_single_container_s<logged_mdt_trigger_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "LoggedMDTTrigger");
    return c.get<protocol_ie_single_container_s<logged_mdt_trigger_ext_ies_o>>();
  }
  void                                                          set_periodical();
  event_trigger_c&                                              set_event_trigger();
  protocol_ie_single_container_s<logged_mdt_trigger_ext_ies_o>& set_choice_exts();

private:
  types                                                                                          type_;
  choice_buffer_t<event_trigger_c, protocol_ie_single_container_s<logged_mdt_trigger_ext_ies_o>> c;

  void destroy_();
};

// LoggingDuration ::= ENUMERATED
struct logging_dur_opts {
  enum options { m10, m20, m40, m60, m90, m120, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using logging_dur_e = enumerated<logging_dur_opts, true>;

// LoggingInterval ::= ENUMERATED
struct logging_interv_opts {
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
    // ...
    nulltype
  } value;
  typedef int32_t number_type;

  const char* to_string() const;
  int32_t     to_number() const;
};
using logging_interv_e = enumerated<logging_interv_opts, true>;

struct m1_cfg_ext_ies_container {
  bool                     include_beam_meass_ind_present = false;
  bool                     beam_meass_report_cfg_present  = false;
  include_beam_meass_ind_e include_beam_meass_ind;
  beam_meass_report_cfg_s  beam_meass_report_cfg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// M1Configuration ::= SEQUENCE
struct m1_cfg_s {
  bool                     ext                       = false;
  bool                     m1thres_event_a2_present  = false;
  bool                     m1periodic_report_present = false;
  bool                     ie_exts_present           = false;
  m1_report_trigger_e      m1report_trigger;
  m1_thres_event_a2_s      m1thres_event_a2;
  m1_periodic_report_s     m1periodic_report;
  m1_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

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

// M5Configuration ::= SEQUENCE
struct m5_cfg_s {
  bool                                       ext = false;
  m5period_e                                 m5period;
  links_to_log_e                             m5_links_to_log;
  protocol_ext_container_l<m5_cfg_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct m6_cfg_ext_ies_container {
  bool                            m6_report_amount_present              = false;
  bool                            excess_packet_delay_thres_cfg_present = false;
  m6_report_amount_mdt_e          m6_report_amount;
  excess_packet_delay_thres_cfg_l excess_packet_delay_thres_cfg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// M6Configuration ::= SEQUENCE
struct m6_cfg_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  m6report_interv_e        m6report_interv;
  links_to_log_e           m6_links_to_log;
  m6_cfg_ext_ies_container ie_exts;
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

using mdt_location_info_ext_ies_container = protocol_ext_container_empty_l;

// MDT-Location-Info ::= SEQUENCE
struct mdt_location_info_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  fixed_bitstring<8, false, true>     mdt_location_info;
  mdt_location_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using sensor_meas_cfg_ext_ies_container = protocol_ext_container_empty_l;

// SensorMeasurementConfiguration ::= SEQUENCE
struct sensor_meas_cfg_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  sensor_meas_cfg_e                 sensor_meas_cfg;
  sensor_meas_cfg_name_list_l       sensor_meas_cfg_name_list;
  sensor_meas_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using wlan_meas_cfg_ext_ies_container = protocol_ext_container_empty_l;

// WLANMeasurementConfiguration ::= SEQUENCE
struct wlan_meas_cfg_s {
  struct wlan_rssi_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using wlan_rssi_e_ = enumerated<wlan_rssi_opts, true>;
  struct wlan_rtt_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using wlan_rtt_e_ = enumerated<wlan_rtt_opts, true>;

  // member variables
  bool                            ext               = false;
  bool                            wlan_rssi_present = false;
  bool                            wlan_rtt_present  = false;
  bool                            ie_exts_present   = false;
  wlan_meas_cfg_e                 wlan_meas_cfg;
  wlan_meas_cfg_name_list_l       wlan_meas_cfg_name_list;
  wlan_rssi_e_                    wlan_rssi;
  wlan_rtt_e_                     wlan_rtt;
  wlan_meas_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using immediate_mdt_nr_ext_ies_container = protocol_ext_container_empty_l;

// ImmediateMDTNr ::= SEQUENCE
struct immediate_mdt_nr_s {
  bool                               ext                        = false;
  bool                               m1_cfg_present             = false;
  bool                               m4_cfg_present             = false;
  bool                               m5_cfg_present             = false;
  bool                               m6_cfg_present             = false;
  bool                               m7_cfg_present             = false;
  bool                               bluetooth_meas_cfg_present = false;
  bool                               wlan_meas_cfg_present      = false;
  bool                               mdt_location_info_present  = false;
  bool                               sensor_meas_cfg_present    = false;
  bool                               ie_exts_present            = false;
  fixed_bitstring<8, false, true>    meass_to_activ;
  m1_cfg_s                           m1_cfg;
  m4_cfg_s                           m4_cfg;
  m5_cfg_s                           m5_cfg;
  m6_cfg_s                           m6_cfg;
  m7_cfg_s                           m7_cfg;
  bluetooth_meas_cfg_s               bluetooth_meas_cfg;
  wlan_meas_cfg_s                    wlan_meas_cfg;
  mdt_location_info_s                mdt_location_info;
  sensor_meas_cfg_s                  sensor_meas_cfg;
  immediate_mdt_nr_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggedMDTNr ::= SEQUENCE
struct logged_mdt_nr_s {
  bool                                              ext                        = false;
  bool                                              bluetooth_meas_cfg_present = false;
  bool                                              wlan_meas_cfg_present      = false;
  bool                                              sensor_meas_cfg_present    = false;
  logging_interv_e                                  logging_interv;
  logging_dur_e                                     logging_dur;
  logged_mdt_trigger_c                              logged_mdt_trigger;
  bluetooth_meas_cfg_s                              bluetooth_meas_cfg;
  wlan_meas_cfg_s                                   wlan_meas_cfg;
  sensor_meas_cfg_s                                 sensor_meas_cfg;
  area_scope_of_neigh_cells_list_l                  area_scope_of_neigh_cells_list;
  protocol_ext_container_l<logged_mdt_nr_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MDTModeNr-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using mdt_mode_nr_ext_ies_o = protocol_ies_empty_o;

// MDT-Activation ::= ENUMERATED
struct mdt_activation_opts {
  enum options { immediate_mdt_only, logged_mdt_only, immediate_mdt_and_trace, /*...*/ nulltype } value;

  const char* to_string() const;
};
using mdt_activation_e = enumerated<mdt_activation_opts, true>;

// MDT-Configuration-EUTRA-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mdt_cfg_eutra_ext_ies_o = protocol_ext_empty_o;

// MDT-Configuration-NR-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mdt_cfg_nr_ext_ies_o = protocol_ext_empty_o;

// MDTModeNr ::= CHOICE
struct mdt_mode_nr_c {
  struct types_opts {
    enum options { immediate_mdt_nr, logged_mdt_nr, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  mdt_mode_nr_c() = default;
  mdt_mode_nr_c(const mdt_mode_nr_c& other);
  mdt_mode_nr_c& operator=(const mdt_mode_nr_c& other);
  ~mdt_mode_nr_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  immediate_mdt_nr_s& immediate_mdt_nr()
  {
    assert_choice_type(types::immediate_mdt_nr, type_, "MDTModeNr");
    return c.get<immediate_mdt_nr_s>();
  }
  logged_mdt_nr_s& logged_mdt_nr()
  {
    assert_choice_type(types::logged_mdt_nr, type_, "MDTModeNr");
    return c.get<logged_mdt_nr_s>();
  }
  protocol_ie_single_container_s<mdt_mode_nr_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "MDTModeNr");
    return c.get<protocol_ie_single_container_s<mdt_mode_nr_ext_ies_o>>();
  }
  const immediate_mdt_nr_s& immediate_mdt_nr() const
  {
    assert_choice_type(types::immediate_mdt_nr, type_, "MDTModeNr");
    return c.get<immediate_mdt_nr_s>();
  }
  const logged_mdt_nr_s& logged_mdt_nr() const
  {
    assert_choice_type(types::logged_mdt_nr, type_, "MDTModeNr");
    return c.get<logged_mdt_nr_s>();
  }
  const protocol_ie_single_container_s<mdt_mode_nr_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "MDTModeNr");
    return c.get<protocol_ie_single_container_s<mdt_mode_nr_ext_ies_o>>();
  }
  immediate_mdt_nr_s&                                    set_immediate_mdt_nr();
  logged_mdt_nr_s&                                       set_logged_mdt_nr();
  protocol_ie_single_container_s<mdt_mode_nr_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                       type_;
  choice_buffer_t<immediate_mdt_nr_s, logged_mdt_nr_s, protocol_ie_single_container_s<mdt_mode_nr_ext_ies_o>> c;

  void destroy_();
};

// MDTPLMNList ::= SEQUENCE (SIZE (1..16)) OF OCTET STRING (SIZE (3))
using mdt_plmn_list_l = bounded_array<fixed_octstring<3, true>, 16>;

// PC5FlowBitRates-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pc5_flow_bit_rates_ext_ies_o = protocol_ext_empty_o;

using mdt_cfg_eutra_ext_ies_container = protocol_ext_container_empty_l;

// MDT-Configuration-EUTRA ::= SEQUENCE
struct mdt_cfg_eutra_s {
  bool                            ext             = false;
  bool                            ie_exts_present = false;
  mdt_activation_e                mdt_activation;
  area_scope_of_mdt_eutra_c       area_scope_of_mdt;
  unbounded_octstring<true>       mdt_mode;
  mdt_plmn_list_l                 sig_based_mdt_plmn_list;
  mdt_cfg_eutra_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MDT-Configuration-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mdt_cfg_ext_ies_o = protocol_ext_empty_o;

using mdt_cfg_nr_ext_ies_container = protocol_ext_container_empty_l;

// MDT-Configuration-NR ::= SEQUENCE
struct mdt_cfg_nr_s {
  bool                         ext             = false;
  bool                         ie_exts_present = false;
  mdt_activation_e             mdt_activation;
  area_scope_of_mdt_nr_c       area_scope_of_mdt;
  mdt_mode_nr_c                mdt_mode_nr;
  mdt_plmn_list_l              sig_based_mdt_plmn_list;
  mdt_cfg_nr_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pc5_flow_bit_rates_ext_ies_container = protocol_ext_container_empty_l;

// PC5FlowBitRates ::= SEQUENCE
struct pc5_flow_bit_rates_s {
  bool                                 ext                      = false;
  bool                                 ie_exts_present          = false;
  uint64_t                             guaranteed_flow_bit_rate = 0;
  uint64_t                             max_flow_bit_rate        = 0;
  pc5_flow_bit_rates_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PC5QoSFlowItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pc5_qos_flow_item_ext_ies_o = protocol_ext_empty_o;

// LocationReportingAdditionalInfo ::= ENUMERATED
struct location_report_add_info_opts {
  enum options { include_pscell, /*...*/ nulltype } value;

  const char* to_string() const;
};
using location_report_add_info_e = enumerated<location_report_add_info_opts, true>;

using mdt_cfg_ext_ies_container = protocol_ext_container_empty_l;

// MDT-Configuration ::= SEQUENCE
struct mdt_cfg_s {
  bool                      ext                   = false;
  bool                      mdt_cfg_nr_present    = false;
  bool                      mdt_cfg_eutra_present = false;
  bool                      ie_exts_present       = false;
  mdt_cfg_nr_s              mdt_cfg_nr;
  mdt_cfg_eutra_s           mdt_cfg_eutra;
  mdt_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pc5_qos_flow_item_ext_ies_container = protocol_ext_container_empty_l;

// PC5QoSFlowItem ::= SEQUENCE
struct pc5_qos_flow_item_s {
  bool                                ext                        = false;
  bool                                pc5_flow_bit_rates_present = false;
  bool                                range_present              = false;
  bool                                ie_exts_present            = false;
  uint16_t                            pqi                        = 0;
  pc5_flow_bit_rates_s                pc5_flow_bit_rates;
  range_e                             range;
  pc5_qos_flow_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupItemHOReq-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_setup_item_ho_req_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { pdu_session_expected_ue_activity_behaviour, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::pdu_session_expected_ue_activity_behaviour; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    expected_ue_activity_behaviour_s&       pdu_session_expected_ue_activity_behaviour() { return c; }
    const expected_ue_activity_behaviour_s& pdu_session_expected_ue_activity_behaviour() const { return c; }

  private:
    expected_ue_activity_behaviour_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UESliceMaximumBitRateItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ue_slice_max_bit_rate_item_ext_ies_o = protocol_ext_empty_o;

// EventType ::= ENUMERATED
struct event_type_opts {
  enum options {
    direct,
    change_of_serve_cell,
    ue_presence_in_area_of_interest,
    stop_change_of_serve_cell,
    stop_ue_presence_in_area_of_interest,
    cancel_location_report_for_the_ue,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
using event_type_e = enumerated<event_type_opts, true>;

// LTEUE-Sidelink-Aggregate-MaximumBitrates-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using lte_ue_sidelink_aggr_max_bitrates_ext_ies_o = protocol_ext_empty_o;

// LTEV2XServicesAuthorized-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ltev2x_services_authorized_ext_ies_o = protocol_ext_empty_o;

// LocationReportingRequestType-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct location_report_request_type_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { location_report_add_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::location_report_add_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    location_report_add_info_e&       location_report_add_info() { return c; }
    const location_report_add_info_e& location_report_add_info() const { return c; }

  private:
    location_report_add_info_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NRUESidelinkAggregateMaximumBitrate-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using nr_ue_sidelink_aggr_max_bitrate_ext_ies_o = protocol_ext_empty_o;

// NRV2XServicesAuthorized-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using nr_v2x_services_authorized_ext_ies_o = protocol_ext_empty_o;

// PC5QoSFlowList ::= SEQUENCE (SIZE (1..2048)) OF PC5QoSFlowItem
using pc5_qos_flow_list_l = dyn_array<pc5_qos_flow_item_s>;

// PC5QoSParameters-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pc5_qos_params_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceSetupItemHOReq ::= SEQUENCE
struct pdu_session_res_setup_item_ho_req_s {
  bool                                                                  ext            = false;
  uint16_t                                                              pdu_session_id = 0;
  s_nssai_s                                                             s_nssai;
  unbounded_octstring<true>                                             ho_request_transfer;
  protocol_ext_container_l<pdu_session_res_setup_item_ho_req_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PedestrianUE ::= ENUMERATED
struct pedestrian_ue_opts {
  enum options { authorized, not_authorized, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pedestrian_ue_e = enumerated<pedestrian_ue_opts, true>;

// ReportArea ::= ENUMERATED
struct report_area_opts {
  enum options { cell, /*...*/ nulltype } value;

  const char* to_string() const;
};
using report_area_e = enumerated<report_area_opts, true>;

// SecurityContext-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using security_context_ext_ies_o = protocol_ext_empty_o;

// TimeSyncAssistanceInfo-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using time_sync_assist_info_ext_ies_o = protocol_ext_empty_o;

// TraceActivation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
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
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
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
    types                                                                             type_;
    choice_buffer_t<mdt_cfg_s, visible_string<0, MAX_ASN_STRING_LENGTH, false, true>> c;

    void destroy_();
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

// UESecurityCapabilities-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ue_security_cap_ext_ies_o = protocol_ext_empty_o;

using ue_slice_max_bit_rate_item_ext_ies_container = protocol_ext_container_empty_l;

// UESliceMaximumBitRateItem ::= SEQUENCE
struct ue_slice_max_bit_rate_item_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  s_nssai_s                                    s_nssai;
  uint64_t                                     ue_slice_max_bit_rate_dl = 0;
  uint64_t                                     ue_slice_max_bit_rate_ul = 0;
  ue_slice_max_bit_rate_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VehicleUE ::= ENUMERATED
struct vehicle_ue_opts {
  enum options { authorized, not_authorized, /*...*/ nulltype } value;

  const char* to_string() const;
};
using vehicle_ue_e = enumerated<vehicle_ue_opts, true>;

// IAB-Authorized ::= ENUMERATED
struct iab_authorized_opts {
  enum options { authorized, not_authorized, /*...*/ nulltype } value;

  const char* to_string() const;
};
using iab_authorized_e = enumerated<iab_authorized_opts, true>;

using lte_ue_sidelink_aggr_max_bitrates_ext_ies_container = protocol_ext_container_empty_l;

// LTEUESidelinkAggregateMaximumBitrate ::= SEQUENCE
struct lte_ue_sidelink_aggr_max_bitrate_s {
  bool                                                ext                           = false;
  bool                                                ie_exts_present               = false;
  uint64_t                                            ue_sidelink_aggr_max_bit_rate = 0;
  lte_ue_sidelink_aggr_max_bitrates_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ltev2x_services_authorized_ext_ies_container = protocol_ext_container_empty_l;

// LTEV2XServicesAuthorized ::= SEQUENCE
struct ltev2x_services_authorized_s {
  bool                                         ext                   = false;
  bool                                         vehicle_ue_present    = false;
  bool                                         pedestrian_ue_present = false;
  bool                                         ie_exts_present       = false;
  vehicle_ue_e                                 vehicle_ue;
  pedestrian_ue_e                              pedestrian_ue;
  ltev2x_services_authorized_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LocationReportingRequestType ::= SEQUENCE
struct location_report_request_type_s {
  bool                    ext                                            = false;
  bool                    location_report_ref_id_to_be_cancelled_present = false;
  event_type_e            event_type;
  report_area_e           report_area;
  area_of_interest_list_l area_of_interest_list;
  uint8_t                 location_report_ref_id_to_be_cancelled = 1;
  protocol_ext_container_l<location_report_request_type_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using nr_ue_sidelink_aggr_max_bitrate_ext_ies_container = protocol_ext_container_empty_l;

// NRUESidelinkAggregateMaximumBitrate ::= SEQUENCE
struct nr_ue_sidelink_aggr_max_bitrate_s {
  bool                                              ext                           = false;
  bool                                              ie_exts_present               = false;
  uint64_t                                          ue_sidelink_aggr_max_bit_rate = 0;
  nr_ue_sidelink_aggr_max_bitrate_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using nr_v2x_services_authorized_ext_ies_container = protocol_ext_container_empty_l;

// NRV2XServicesAuthorized ::= SEQUENCE
struct nr_v2x_services_authorized_s {
  bool                                         ext                   = false;
  bool                                         vehicle_ue_present    = false;
  bool                                         pedestrian_ue_present = false;
  bool                                         ie_exts_present       = false;
  vehicle_ue_e                                 vehicle_ue;
  pedestrian_ue_e                              pedestrian_ue;
  nr_v2x_services_authorized_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NewSecurityContextInd ::= ENUMERATED
struct new_security_context_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using new_security_context_ind_e = enumerated<new_security_context_ind_opts, true>;

using pc5_qos_params_ext_ies_container = protocol_ext_container_empty_l;

// PC5QoSParameters ::= SEQUENCE
struct pc5_qos_params_s {
  bool                             ext                             = false;
  bool                             pc5_link_aggr_bit_rates_present = false;
  bool                             ie_exts_present                 = false;
  pc5_qos_flow_list_l              pc5_qos_flow_list;
  uint64_t                         pc5_link_aggr_bit_rates = 0;
  pc5_qos_params_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupListHOReq ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceSetupItemHOReq
using pdu_session_res_setup_list_ho_req_l = dyn_array<pdu_session_res_setup_item_ho_req_s>;

// RRCInactiveTransitionReportRequest ::= ENUMERATED
struct rrc_inactive_transition_report_request_opts {
  enum options {
    subsequent_state_transition_report,
    single_rrc_connected_state_report,
    cancel_report,
    /*...*/ nulltype
  } value;

  const char* to_string() const;
};
using rrc_inactive_transition_report_request_e = enumerated<rrc_inactive_transition_report_request_opts, true>;

// RedirectionVoiceFallback ::= ENUMERATED
struct redirection_voice_fallback_opts {
  enum options { possible, not_possible, /*...*/ nulltype } value;

  const char* to_string() const;
};
using redirection_voice_fallback_e = enumerated<redirection_voice_fallback_opts, true>;

using security_context_ext_ies_container = protocol_ext_container_empty_l;

// SecurityContext ::= SEQUENCE
struct security_context_s {
  bool                               ext                     = false;
  bool                               ie_exts_present         = false;
  uint8_t                            next_hop_chaining_count = 0;
  fixed_bitstring<256, false, true>  next_hop_nh;
  security_context_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using time_sync_assist_info_ext_ies_container = protocol_ext_container_empty_l;

// TimeSyncAssistanceInfo ::= SEQUENCE
struct time_sync_assist_info_s {
  struct time_distribution_ind_opts {
    enum options { enabled, disabled, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using time_distribution_ind_e_ = enumerated<time_distribution_ind_opts, true>;

  // member variables
  bool                                    ext                               = false;
  bool                                    uu_time_sync_error_budget_present = false;
  bool                                    ie_exts_present                   = false;
  time_distribution_ind_e_                time_distribution_ind;
  uint32_t                                uu_time_sync_error_budget = 1;
  time_sync_assist_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
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
  fixed_octstring<8, true>              ngran_trace_id;
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

// UE-UP-CIoT-Support ::= ENUMERATED
struct ue_up_c_iot_support_opts {
  enum options { supported, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ue_up_c_iot_support_e = enumerated<ue_up_c_iot_support_opts, true>;

using ue_security_cap_ext_ies_container = protocol_ext_container_empty_l;

// UESecurityCapabilities ::= SEQUENCE
struct ue_security_cap_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  fixed_bitstring<16, true, true>   nr_encryption_algorithms;
  fixed_bitstring<16, true, true>   nr_integrity_protection_algorithms;
  fixed_bitstring<16, true, true>   eutr_aencryption_algorithms;
  fixed_bitstring<16, true, true>   eutr_aintegrity_protection_algorithms;
  ue_security_cap_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UESliceMaximumBitRateList ::= SEQUENCE (SIZE (1..8)) OF UESliceMaximumBitRateItem
using ue_slice_max_bit_rate_list_l = dyn_array<ue_slice_max_bit_rate_item_s>;

// PDUSessionResourceAdmittedItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_admitted_item_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceFailedToSetupItemHOAck-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_failed_to_setup_item_ho_ack_ext_ies_o = protocol_ext_empty_o;

// NPN-AccessInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using npn_access_info_ext_ies_o = protocol_ies_empty_o;

using pdu_session_res_admitted_item_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceAdmittedItem ::= SEQUENCE
struct pdu_session_res_admitted_item_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  uint16_t                                        pdu_session_id  = 0;
  unbounded_octstring<true>                       ho_request_ack_transfer;
  pdu_session_res_admitted_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_failed_to_setup_item_ho_ack_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceFailedToSetupItemHOAck ::= SEQUENCE
struct pdu_session_res_failed_to_setup_item_ho_ack_s {
  bool                                                          ext             = false;
  bool                                                          ie_exts_present = false;
  uint16_t                                                      pdu_session_id  = 0;
  unbounded_octstring<true>                                     ho_res_alloc_unsuccessful_transfer;
  pdu_session_res_failed_to_setup_item_ho_ack_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPN-AccessInformation ::= CHOICE
struct npn_access_info_c {
  struct types_opts {
    enum options { pni_npn_access_info, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  npn_access_info_c() = default;
  npn_access_info_c(const npn_access_info_c& other);
  npn_access_info_c& operator=(const npn_access_info_c& other);
  ~npn_access_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cell_cag_list_l& pni_npn_access_info()
  {
    assert_choice_type(types::pni_npn_access_info, type_, "NPN-AccessInformation");
    return c.get<cell_cag_list_l>();
  }
  protocol_ie_single_container_s<npn_access_info_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "NPN-AccessInformation");
    return c.get<protocol_ie_single_container_s<npn_access_info_ext_ies_o>>();
  }
  const cell_cag_list_l& pni_npn_access_info() const
  {
    assert_choice_type(types::pni_npn_access_info, type_, "NPN-AccessInformation");
    return c.get<cell_cag_list_l>();
  }
  const protocol_ie_single_container_s<npn_access_info_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "NPN-AccessInformation");
    return c.get<protocol_ie_single_container_s<npn_access_info_ext_ies_o>>();
  }
  cell_cag_list_l&                                           set_pni_npn_access_info();
  protocol_ie_single_container_s<npn_access_info_ext_ies_o>& set_choice_exts();

private:
  types                                                                                       type_;
  choice_buffer_t<cell_cag_list_l, protocol_ie_single_container_s<npn_access_info_ext_ies_o>> c;

  void destroy_();
};

// PDUSessionResourceAdmittedList ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceAdmittedItem
using pdu_session_res_admitted_list_l = dyn_array<pdu_session_res_admitted_item_s>;

// PDUSessionResourceFailedToSetupListHOAck ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceFailedToSetupItemHOAck
using pdu_session_res_failed_to_setup_list_ho_ack_l = dyn_array<pdu_session_res_failed_to_setup_item_ho_ack_s>;

// RedCapIndication ::= ENUMERATED
struct red_cap_ind_opts {
  enum options { redcap, /*...*/ nulltype } value;

  const char* to_string() const;
};
using red_cap_ind_e = enumerated<red_cap_ind_opts, true>;

// RSN ::= ENUMERATED
struct r_sn_opts {
  enum options { v1, v2, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using r_sn_e = enumerated<r_sn_opts, true>;

// RedundantPDUSessionInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
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

// ConfidentialityProtectionResult ::= ENUMERATED
struct confidentiality_protection_result_opts {
  enum options { performed, not_performed, /*...*/ nulltype } value;

  const char* to_string() const;
};
using confidentiality_protection_result_e = enumerated<confidentiality_protection_result_opts, true>;

// IntegrityProtectionResult ::= ENUMERATED
struct integrity_protection_result_opts {
  enum options { performed, not_performed, /*...*/ nulltype } value;

  const char* to_string() const;
};
using integrity_protection_result_e = enumerated<integrity_protection_result_opts, true>;

// MBS-SupportIndicator ::= ENUMERATED
struct mbs_support_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using mbs_support_ind_e = enumerated<mbs_support_ind_opts, true>;

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

// SecurityResult-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using security_result_ext_ies_o = protocol_ext_empty_o;

// HandoverRequestAcknowledgeTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct ho_request_ack_transfer_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        add_dl_up_tnl_info_for_ho_list,
        ul_forwarding_up_tnl_info,
        add_ul_forwarding_up_tnl_info,
        data_forwarding_resp_erab_list,
        redundant_dl_ngu_up_tnl_info,
        used_r_sn_info,
        global_ran_node_id,
        mbs_support_ind,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    add_dl_up_tnl_info_for_ho_list_l&       add_dl_up_tnl_info_for_ho_list();
    up_transport_layer_info_c&              ul_forwarding_up_tnl_info();
    up_transport_layer_info_list_l&         add_ul_forwarding_up_tnl_info();
    data_forwarding_resp_erab_list_l&       data_forwarding_resp_erab_list();
    up_transport_layer_info_c&              redundant_dl_ngu_up_tnl_info();
    redundant_pdu_session_info_s&           used_r_sn_info();
    global_ran_node_id_c&                   global_ran_node_id();
    mbs_support_ind_e&                      mbs_support_ind();
    const add_dl_up_tnl_info_for_ho_list_l& add_dl_up_tnl_info_for_ho_list() const;
    const up_transport_layer_info_c&        ul_forwarding_up_tnl_info() const;
    const up_transport_layer_info_list_l&   add_ul_forwarding_up_tnl_info() const;
    const data_forwarding_resp_erab_list_l& data_forwarding_resp_erab_list() const;
    const up_transport_layer_info_c&        redundant_dl_ngu_up_tnl_info() const;
    const redundant_pdu_session_info_s&     used_r_sn_info() const;
    const global_ran_node_id_c&             global_ran_node_id() const;
    const mbs_support_ind_e&                mbs_support_ind() const;

  private:
    types type_;
    choice_buffer_t<add_dl_up_tnl_info_for_ho_list_l,
                    data_forwarding_resp_erab_list_l,
                    global_ran_node_id_c,
                    redundant_pdu_session_info_s,
                    up_transport_layer_info_c,
                    up_transport_layer_info_list_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

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

struct ho_request_ack_transfer_ext_ies_container {
  bool                             add_dl_up_tnl_info_for_ho_list_present = false;
  bool                             ul_forwarding_up_tnl_info_present      = false;
  bool                             add_ul_forwarding_up_tnl_info_present  = false;
  bool                             data_forwarding_resp_erab_list_present = false;
  bool                             redundant_dl_ngu_up_tnl_info_present   = false;
  bool                             used_r_sn_info_present                 = false;
  bool                             global_ran_node_id_present             = false;
  bool                             mbs_support_ind_present                = false;
  add_dl_up_tnl_info_for_ho_list_l add_dl_up_tnl_info_for_ho_list;
  up_transport_layer_info_c        ul_forwarding_up_tnl_info;
  up_transport_layer_info_list_l   add_ul_forwarding_up_tnl_info;
  data_forwarding_resp_erab_list_l data_forwarding_resp_erab_list;
  up_transport_layer_info_c        redundant_dl_ngu_up_tnl_info;
  redundant_pdu_session_info_s     used_r_sn_info;
  global_ran_node_id_c             global_ran_node_id;
  mbs_support_ind_e                mbs_support_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverRequestAcknowledgeTransfer ::= SEQUENCE
struct ho_request_ack_transfer_s {
  bool                                      ext                               = false;
  bool                                      dl_forwarding_up_tnl_info_present = false;
  bool                                      security_result_present           = false;
  bool                                      ie_exts_present                   = false;
  up_transport_layer_info_c                 dl_ngu_up_tnl_info;
  up_transport_layer_info_c                 dl_forwarding_up_tnl_info;
  security_result_s                         security_result;
  qos_flow_list_with_data_forwarding_l      qos_flow_setup_resp_list;
  qos_flow_list_with_cause_l                qos_flow_failed_to_setup_list;
  data_forwarding_resp_drb_list_l           data_forwarding_resp_drb_list;
  ho_request_ack_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LAI-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using lai_ext_ies_o = protocol_ext_empty_o;

using lai_ext_ies_container = protocol_ext_container_empty_l;

// LAI ::= SEQUENCE
struct lai_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  fixed_octstring<3, true> plmn_id;
  fixed_octstring<2, true> lac;
  lai_ext_ies_container    ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TargetHomeENB-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using target_home_enb_id_ext_ies_o = protocol_ext_empty_o;

// TargetRNC-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using target_rnc_id_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceItemHORqd-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_item_ho_rqd_ext_ies_o = protocol_ext_empty_o;

using target_home_enb_id_ext_ies_container = protocol_ext_container_empty_l;

// TargetHomeENB-ID ::= SEQUENCE
struct target_home_enb_id_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  fixed_octstring<3, true>             plmn_id;
  fixed_bitstring<28, false, true>     home_enb_id;
  eps_tai_s                            sel_eps_tai;
  target_home_enb_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TargetRANNodeID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using target_ran_node_id_ext_ies_o = protocol_ext_empty_o;

using target_rnc_id_ext_ies_container = protocol_ext_container_empty_l;

// TargetRNC-ID ::= SEQUENCE
struct target_rnc_id_s {
  bool                            ext                     = false;
  bool                            extended_rnc_id_present = false;
  bool                            ie_exts_present         = false;
  lai_s                           lai;
  uint16_t                        rnc_id          = 0;
  uint32_t                        extended_rnc_id = 4096;
  target_rnc_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TargeteNB-ID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using target_enb_id_ext_ies_o = protocol_ext_empty_o;

using pdu_session_res_item_ho_rqd_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceItemHORqd ::= SEQUENCE
struct pdu_session_res_item_ho_rqd_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  uint16_t                                      pdu_session_id  = 0;
  unbounded_octstring<true>                     ho_required_transfer;
  pdu_session_res_item_ho_rqd_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TargetID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct target_id_ext_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { target_rnc_id, target_home_enb_id, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    target_rnc_id_s&            target_rnc_id();
    target_home_enb_id_s&       target_home_enb_id();
    const target_rnc_id_s&      target_rnc_id() const;
    const target_home_enb_id_s& target_home_enb_id() const;

  private:
    types                                                  type_;
    choice_buffer_t<target_home_enb_id_s, target_rnc_id_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using target_ran_node_id_ext_ies_container = protocol_ext_container_empty_l;

// TargetRANNodeID ::= SEQUENCE
struct target_ran_node_id_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  global_ran_node_id_c                 global_ran_node_id;
  tai_s                                sel_tai;
  target_ran_node_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using target_enb_id_ext_ies_container = protocol_ext_container_empty_l;

// TargeteNB-ID ::= SEQUENCE
struct target_enb_id_s {
  bool                            ext             = false;
  bool                            ie_exts_present = false;
  global_ng_enb_id_s              global_enb_id;
  eps_tai_s                       sel_eps_tai;
  target_enb_id_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DirectForwardingPathAvailability ::= ENUMERATED
struct direct_forwarding_path_availability_opts {
  enum options { direct_path_available, /*...*/ nulltype } value;

  const char* to_string() const;
};
using direct_forwarding_path_availability_e = enumerated<direct_forwarding_path_availability_opts, true>;

// PDUSessionResourceListHORqd ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceItemHORqd
using pdu_session_res_list_ho_rqd_l = dyn_array<pdu_session_res_item_ho_rqd_s>;

// TargetID ::= CHOICE
struct target_id_c {
  struct types_opts {
    enum options { target_ran_node_id, target_enb_id, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  target_id_c() = default;
  target_id_c(const target_id_c& other);
  target_id_c& operator=(const target_id_c& other);
  ~target_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  target_ran_node_id_s& target_ran_node_id()
  {
    assert_choice_type(types::target_ran_node_id, type_, "TargetID");
    return c.get<target_ran_node_id_s>();
  }
  target_enb_id_s& target_enb_id()
  {
    assert_choice_type(types::target_enb_id, type_, "TargetID");
    return c.get<target_enb_id_s>();
  }
  protocol_ie_single_container_s<target_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "TargetID");
    return c.get<protocol_ie_single_container_s<target_id_ext_ies_o>>();
  }
  const target_ran_node_id_s& target_ran_node_id() const
  {
    assert_choice_type(types::target_ran_node_id, type_, "TargetID");
    return c.get<target_ran_node_id_s>();
  }
  const target_enb_id_s& target_enb_id() const
  {
    assert_choice_type(types::target_enb_id, type_, "TargetID");
    return c.get<target_enb_id_s>();
  }
  const protocol_ie_single_container_s<target_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "TargetID");
    return c.get<protocol_ie_single_container_s<target_id_ext_ies_o>>();
  }
  target_ran_node_id_s&                                set_target_ran_node_id();
  target_enb_id_s&                                     set_target_enb_id();
  protocol_ie_single_container_s<target_id_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                       type_;
  choice_buffer_t<protocol_ie_single_container_s<target_id_ext_ies_o>, target_enb_id_s, target_ran_node_id_s> c;

  void destroy_();
};

// HandoverRequiredTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ho_required_transfer_ext_ies_o = protocol_ext_empty_o;

using ho_required_transfer_ext_ies_container = protocol_ext_container_empty_l;

// HandoverRequiredTransfer ::= SEQUENCE
struct ho_required_transfer_s {
  bool                                   ext                                         = false;
  bool                                   direct_forwarding_path_availability_present = false;
  bool                                   ie_exts_present                             = false;
  direct_forwarding_path_availability_e  direct_forwarding_path_availability;
  ho_required_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverResourceAllocationUnsuccessfulTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ho_res_alloc_unsuccessful_transfer_ext_ies_o = protocol_ext_empty_o;

using ho_res_alloc_unsuccessful_transfer_ext_ies_container = protocol_ext_container_empty_l;

// HandoverResourceAllocationUnsuccessfulTransfer ::= SEQUENCE
struct ho_res_alloc_unsuccessful_transfer_s {
  bool                                                 ext                      = false;
  bool                                                 crit_diagnostics_present = false;
  bool                                                 ie_exts_present          = false;
  cause_c                                              cause;
  crit_diagnostics_s                                   crit_diagnostics;
  ho_res_alloc_unsuccessful_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RecommendedRANNodeItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using recommended_ran_node_item_ext_ies_o = protocol_ext_empty_o;

using recommended_ran_node_item_ext_ies_container = protocol_ext_container_empty_l;

// RecommendedRANNodeItem ::= SEQUENCE
struct recommended_ran_node_item_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  amf_paging_target_c                         amf_paging_target;
  recommended_ran_node_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RecommendedRANNodeList ::= SEQUENCE (SIZE (1..16)) OF RecommendedRANNodeItem
using recommended_ran_node_list_l = dyn_array<recommended_ran_node_item_s>;

// RecommendedRANNodesForPaging-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using recommended_ran_nodes_for_paging_ext_ies_o = protocol_ext_empty_o;

// InfoOnRecommendedCellsAndRANNodesForPaging-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using info_on_recommended_cells_and_ran_nodes_for_paging_ext_ies_o = protocol_ext_empty_o;

using recommended_ran_nodes_for_paging_ext_ies_container = protocol_ext_container_empty_l;

// RecommendedRANNodesForPaging ::= SEQUENCE
struct recommended_ran_nodes_for_paging_s {
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  recommended_ran_node_list_l                        recommended_ran_node_list;
  recommended_ran_nodes_for_paging_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using info_on_recommended_cells_and_ran_nodes_for_paging_ext_ies_container = protocol_ext_container_empty_l;

// InfoOnRecommendedCellsAndRANNodesForPaging ::= SEQUENCE
struct info_on_recommended_cells_and_ran_nodes_for_paging_s {
  bool                                                                 ext             = false;
  bool                                                                 ie_exts_present = false;
  recommended_cells_for_paging_s                                       recommended_cells_for_paging;
  recommended_ran_nodes_for_paging_s                                   recommend_ran_nodes_for_paging;
  info_on_recommended_cells_and_ran_nodes_for_paging_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceFailedToSetupItemCxtFail-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_failed_to_setup_item_cxt_fail_ext_ies_o = protocol_ext_empty_o;

using pdu_session_res_failed_to_setup_item_cxt_fail_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceFailedToSetupItemCxtFail ::= SEQUENCE
struct pdu_session_res_failed_to_setup_item_cxt_fail_s {
  bool                                                            ext             = false;
  bool                                                            ie_exts_present = false;
  uint16_t                                                        pdu_session_id  = 0;
  unbounded_octstring<true>                                       pdu_session_res_setup_unsuccessful_transfer;
  pdu_session_res_failed_to_setup_item_cxt_fail_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceFailedToSetupListCxtFail ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceFailedToSetupItemCxtFail
using pdu_session_res_failed_to_setup_list_cxt_fail_l = dyn_array<pdu_session_res_failed_to_setup_item_cxt_fail_s>;

// SliceSupportQMC-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using slice_support_qmc_item_ext_ies_o = protocol_ext_empty_o;

// MDT-AlignmentInfo-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using mdt_align_info_ext_ies_o = protocol_ies_empty_o;

using slice_support_qmc_item_ext_ies_container = protocol_ext_container_empty_l;

// SliceSupportQMC-Item ::= SEQUENCE
struct slice_support_qmc_item_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  s_nssai_s                                s_nssai;
  slice_support_qmc_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MDT-AlignmentInfo ::= CHOICE
struct mdt_align_info_c {
  struct types_opts {
    enum options { s_based_mdt, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  mdt_align_info_c() = default;
  mdt_align_info_c(const mdt_align_info_c& other);
  mdt_align_info_c& operator=(const mdt_align_info_c& other);
  ~mdt_align_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_octstring<8, true>& s_based_mdt()
  {
    assert_choice_type(types::s_based_mdt, type_, "MDT-AlignmentInfo");
    return c.get<fixed_octstring<8, true>>();
  }
  protocol_ie_single_container_s<mdt_align_info_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "MDT-AlignmentInfo");
    return c.get<protocol_ie_single_container_s<mdt_align_info_ext_ies_o>>();
  }
  const fixed_octstring<8, true>& s_based_mdt() const
  {
    assert_choice_type(types::s_based_mdt, type_, "MDT-AlignmentInfo");
    return c.get<fixed_octstring<8, true>>();
  }
  const protocol_ie_single_container_s<mdt_align_info_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "MDT-AlignmentInfo");
    return c.get<protocol_ie_single_container_s<mdt_align_info_ext_ies_o>>();
  }
  fixed_octstring<8, true>&                                 set_s_based_mdt();
  protocol_ie_single_container_s<mdt_align_info_ext_ies_o>& set_choice_exts();

private:
  types                                                                                               type_;
  choice_buffer_t<fixed_octstring<8, true>, protocol_ie_single_container_s<mdt_align_info_ext_ies_o>> c;

  void destroy_();
};

// ServiceType ::= ENUMERATED
struct service_type_opts {
  enum options { streaming, mt_si, vr, /*...*/ nulltype } value;

  const char* to_string() const;
};
using service_type_e = enumerated<service_type_opts, true>;

// SliceSupportListQMC ::= SEQUENCE (SIZE (1..16)) OF SliceSupportQMC-Item
using slice_support_list_qmc_l = dyn_array<slice_support_qmc_item_s>;

// UEAppLayerMeasConfigInfo-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ue_app_layer_meas_cfg_info_ext_ies_o = protocol_ext_empty_o;

using ue_app_layer_meas_cfg_info_ext_ies_container = protocol_ext_container_empty_l;

// UEAppLayerMeasConfigInfo ::= SEQUENCE
struct ue_app_layer_meas_cfg_info_s {
  struct qo_e_meas_status_opts {
    enum options { ongoing, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using qo_e_meas_status_e_ = enumerated<qo_e_meas_status_opts, true>;

  // member variables
  bool                                         ext                                        = false;
  bool                                         qo_e_meas_status_present                   = false;
  bool                                         meas_cfg_app_layer_id_present              = false;
  bool                                         mdt_align_info_present                     = false;
  bool                                         available_ran_visible_qo_e_metrics_present = false;
  bool                                         ie_exts_present                            = false;
  fixed_octstring<6, true>                     qo_e_ref;
  service_type_e                               service_type;
  area_scope_of_qmc_c                          area_scope_of_qmc;
  bounded_bitstring<1, 160, true, true>        meas_coll_entity_ip_address;
  qo_e_meas_status_e_                          qo_e_meas_status;
  bounded_octstring<1, 8000, true>             container_for_app_layer_meas_cfg;
  uint8_t                                      meas_cfg_app_layer_id = 0;
  slice_support_list_qmc_l                     slice_support_list_qmc;
  mdt_align_info_c                             mdt_align_info;
  available_ran_visible_qo_e_metrics_s         available_ran_visible_qo_e_metrics;
  ue_app_layer_meas_cfg_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEAppLayerMeasInfoItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ue_app_layer_meas_info_item_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceSetupItemCxtReq-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_setup_item_cxt_req_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { pdu_session_expected_ue_activity_behaviour, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::pdu_session_expected_ue_activity_behaviour; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    expected_ue_activity_behaviour_s&       pdu_session_expected_ue_activity_behaviour() { return c; }
    const expected_ue_activity_behaviour_s& pdu_session_expected_ue_activity_behaviour() const { return c; }

  private:
    expected_ue_activity_behaviour_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using ue_app_layer_meas_info_item_ext_ies_container = protocol_ext_container_empty_l;

// UEAppLayerMeasInfoItem ::= SEQUENCE
struct ue_app_layer_meas_info_item_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  ue_app_layer_meas_cfg_info_s                  ue_app_layer_meas_cfg_info;
  ue_app_layer_meas_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupItemCxtReq ::= SEQUENCE
struct pdu_session_res_setup_item_cxt_req_s {
  bool                                                                   ext            = false;
  uint16_t                                                               pdu_session_id = 0;
  unbounded_octstring<true>                                              nas_pdu;
  s_nssai_s                                                              s_nssai;
  unbounded_octstring<true>                                              pdu_session_res_setup_request_transfer;
  protocol_ext_container_l<pdu_session_res_setup_item_cxt_req_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QMCConfigInfo-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using q_mcc_onfig_info_ext_ies_o = protocol_ext_empty_o;

// UEAppLayerMeasInfoList ::= SEQUENCE (SIZE (1..16)) OF UEAppLayerMeasInfoItem
using ue_app_layer_meas_info_list_l = dyn_array<ue_app_layer_meas_info_item_s>;

// PDUSessionResourceSetupListCxtReq ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceSetupItemCxtReq
using pdu_session_res_setup_list_cxt_req_l = dyn_array<pdu_session_res_setup_item_cxt_req_s>;

using q_mcc_onfig_info_ext_ies_container = protocol_ext_container_empty_l;

// QMCConfigInfo ::= SEQUENCE
struct q_mcc_onfig_info_s {
  bool                               ext             = false;
  bool                               ie_exts_present = false;
  ue_app_layer_meas_info_list_l      ue_app_layer_meas_info_list;
  q_mcc_onfig_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceFailedToSetupItemCxtRes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_failed_to_setup_item_cxt_res_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceSetupItemCxtRes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_setup_item_cxt_res_ext_ies_o = protocol_ext_empty_o;

using pdu_session_res_failed_to_setup_item_cxt_res_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceFailedToSetupItemCxtRes ::= SEQUENCE
struct pdu_session_res_failed_to_setup_item_cxt_res_s {
  bool                                                           ext             = false;
  bool                                                           ie_exts_present = false;
  uint16_t                                                       pdu_session_id  = 0;
  unbounded_octstring<true>                                      pdu_session_res_setup_unsuccessful_transfer;
  pdu_session_res_failed_to_setup_item_cxt_res_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_setup_item_cxt_res_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceSetupItemCxtRes ::= SEQUENCE
struct pdu_session_res_setup_item_cxt_res_s {
  bool                                                 ext             = false;
  bool                                                 ie_exts_present = false;
  uint16_t                                             pdu_session_id  = 0;
  unbounded_octstring<true>                            pdu_session_res_setup_resp_transfer;
  pdu_session_res_setup_item_cxt_res_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceFailedToSetupListCxtRes ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceFailedToSetupItemCxtRes
using pdu_session_res_failed_to_setup_list_cxt_res_l = dyn_array<pdu_session_res_failed_to_setup_item_cxt_res_s>;

// PDUSessionResourceSetupListCxtRes ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceSetupItemCxtRes
using pdu_session_res_setup_list_cxt_res_l = dyn_array<pdu_session_res_setup_item_cxt_res_s>;

// SourceToTarget-AMFInformationReroute-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using source_to_target_amf_info_reroute_ext_ies_o = protocol_ext_empty_o;

// AuthenticatedIndication ::= ENUMERATED
struct authenticated_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using authenticated_ind_e = enumerated<authenticated_ind_opts, true>;

// CEmodeBSupport-Indicator ::= ENUMERATED
struct ce_mode_b_support_ind_opts {
  enum options { supported, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ce_mode_b_support_ind_e = enumerated<ce_mode_b_support_ind_opts, true>;

// EDT-Session ::= ENUMERATED
struct edt_session_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using edt_session_e = enumerated<edt_session_opts, true>;

// IABNodeIndication ::= ENUMERATED
struct iab_node_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using iab_node_ind_e = enumerated<iab_node_ind_opts, true>;

// LTEM-Indication ::= ENUMERATED
struct ltem_ind_opts {
  enum options { lte_m, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ltem_ind_e = enumerated<ltem_ind_opts, true>;

// RRCEstablishmentCause ::= ENUMERATED
struct rrc_establishment_cause_opts {
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
    // ...
    not_available,
    mo_exception_data,
    nulltype
  } value;

  const char* to_string() const;
};
using rrc_establishment_cause_e = enumerated<rrc_establishment_cause_opts, true, 2>;

using source_to_target_amf_info_reroute_ext_ies_container = protocol_ext_container_empty_l;

// SourceToTarget-AMFInformationReroute ::= SEQUENCE
struct source_to_target_amf_info_reroute_s {
  bool                                                ext                            = false;
  bool                                                cfg_nssai_present              = false;
  bool                                                rejected_nssai_in_plmn_present = false;
  bool                                                rejected_nssai_in_ta_present   = false;
  bool                                                ie_exts_present                = false;
  fixed_octstring<128, true>                          cfg_nssai;
  fixed_octstring<32, true>                           rejected_nssai_in_plmn;
  fixed_octstring<32, true>                           rejected_nssai_in_ta;
  source_to_target_amf_info_reroute_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextRequest ::= ENUMERATED
struct ue_context_request_opts {
  enum options { requested, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ue_context_request_e = enumerated<ue_context_request_opts, true>;

// SliceOverloadItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using slice_overload_item_ext_ies_o = protocol_ext_empty_o;

// UE-PagingItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ue_paging_item_ext_ies_o = protocol_ext_empty_o;

// ConfiguredTACIndication ::= ENUMERATED
struct cfg_tac_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using cfg_tac_ind_e = enumerated<cfg_tac_ind_opts, true>;

// MBS-AreaTAIList ::= SEQUENCE (SIZE (1..16)) OF TAI
using mbs_area_tai_list_l = dyn_array<tai_s>;

// MulticastGroupPagingArea-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using multicast_group_paging_area_ext_ies_o = protocol_ext_empty_o;

// OverloadAction ::= ENUMERATED
struct overload_action_opts {
  enum options {
    reject_non_emergency_mo_dt,
    reject_rrc_cr_sig,
    permit_emergency_sessions_and_mobile_terminated_services_only,
    permit_high_prio_sessions_and_mobile_terminated_services_only,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
using overload_action_e = enumerated<overload_action_opts, true>;

// OverloadResponse-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using overload_resp_ext_ies_o = protocol_ies_empty_o;

// RAT-Information ::= ENUMERATED
struct rat_info_opts {
  enum options { unlicensed, nb_iot, /*...*/ nr_leo, nr_meo, nr_geo, nr_othersat, nulltype } value;

  const char* to_string() const;
};
using rat_info_e = enumerated<rat_info_opts, true, 4>;

using slice_overload_item_ext_ies_container = protocol_ext_container_empty_l;

// SliceOverloadItem ::= SEQUENCE
struct slice_overload_item_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  s_nssai_s                             s_nssai;
  slice_overload_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ue_paging_item_ext_ies_container = protocol_ext_container_empty_l;

// UE-PagingItem ::= SEQUENCE
struct ue_paging_item_s {
  bool                             ext                = false;
  bool                             paging_drx_present = false;
  bool                             ie_exts_present    = false;
  ue_id_idx_value_c                ue_id_idx_value;
  paging_drx_e                     paging_drx;
  ue_paging_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-associatedLogicalNG-connectionItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ue_associated_lc_ng_conn_item_ext_ies_o = protocol_ext_empty_o;

using multicast_group_paging_area_ext_ies_container = protocol_ext_container_empty_l;

// MulticastGroupPagingArea ::= SEQUENCE
struct multicast_group_paging_area_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  mbs_area_tai_list_l                           mbs_area_tai_list;
  multicast_group_paging_area_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastGroupPagingAreaItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using multicast_group_paging_area_item_ext_ies_o = protocol_ext_empty_o;

// NGRAN-TNLAssociationToRemoveItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ngran_tnl_assoc_to_rem_item_ext_ies_o = protocol_ext_empty_o;

// OverloadResponse ::= CHOICE
struct overload_resp_c {
  struct types_opts {
    enum options { overload_action, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  overload_resp_c() = default;
  overload_resp_c(const overload_resp_c& other);
  overload_resp_c& operator=(const overload_resp_c& other);
  ~overload_resp_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  overload_action_e& overload_action()
  {
    assert_choice_type(types::overload_action, type_, "OverloadResponse");
    return c.get<overload_action_e>();
  }
  protocol_ie_single_container_s<overload_resp_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "OverloadResponse");
    return c.get<protocol_ie_single_container_s<overload_resp_ext_ies_o>>();
  }
  const overload_action_e& overload_action() const
  {
    assert_choice_type(types::overload_action, type_, "OverloadResponse");
    return c.get<overload_action_e>();
  }
  const protocol_ie_single_container_s<overload_resp_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "OverloadResponse");
    return c.get<protocol_ie_single_container_s<overload_resp_ext_ies_o>>();
  }
  overload_action_e&                                       set_overload_action();
  protocol_ie_single_container_s<overload_resp_ext_ies_o>& set_choice_exts();

private:
  types                                                                    type_;
  choice_buffer_t<protocol_ie_single_container_s<overload_resp_ext_ies_o>> c;

  void destroy_();
};

// OverloadStartNSSAIItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using overload_start_nssai_item_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceFailedToModifyItemModCfm-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_failed_to_modify_item_mod_cfm_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceFailedToModifyItemModRes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_failed_to_modify_item_mod_res_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceFailedToResumeItemRESReq-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_failed_to_resume_item_res_req_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceFailedToResumeItemRESRes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_failed_to_resume_item_res_res_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceFailedToSetupItemPSReq-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_failed_to_setup_item_ps_req_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceFailedToSetupItemSURes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_failed_to_setup_item_su_res_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceItemCxtRelCpl-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_item_cxt_rel_cpl_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { pdu_session_res_release_resp_transfer, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::pdu_session_res_release_resp_transfer; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    unbounded_octstring<true>&       pdu_session_res_release_resp_transfer() { return c; }
    const unbounded_octstring<true>& pdu_session_res_release_resp_transfer() const { return c; }

  private:
    unbounded_octstring<true> c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceItemCxtRelReq-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_item_cxt_rel_req_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceModifyItemModCfm-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_modify_item_mod_cfm_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceModifyItemModInd-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_modify_item_mod_ind_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceModifyItemModReq-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_modify_item_mod_req_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { s_nssai, pdu_session_expected_ue_activity_behaviour, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    s_nssai_s&                              s_nssai();
    expected_ue_activity_behaviour_s&       pdu_session_expected_ue_activity_behaviour();
    const s_nssai_s&                        s_nssai() const;
    const expected_ue_activity_behaviour_s& pdu_session_expected_ue_activity_behaviour() const;

  private:
    types                                                        type_;
    choice_buffer_t<expected_ue_activity_behaviour_s, s_nssai_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceModifyItemModRes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_modify_item_mod_res_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceNotifyItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_notify_item_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceReleasedItemNot-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_released_item_not_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceReleasedItemPSAck-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_released_item_ps_ack_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceReleasedItemPSFail-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_released_item_ps_fail_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceReleasedItemRelRes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_released_item_rel_res_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceResumeItemRESReq-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_resume_item_res_req_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceResumeItemRESRes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_resume_item_res_res_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceSecondaryRATUsageItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_secondary_rat_usage_item_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceSetupItemSUReq-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_setup_item_su_req_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { pdu_session_expected_ue_activity_behaviour, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::pdu_session_expected_ue_activity_behaviour; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    expected_ue_activity_behaviour_s&       pdu_session_expected_ue_activity_behaviour() { return c; }
    const expected_ue_activity_behaviour_s& pdu_session_expected_ue_activity_behaviour() const { return c; }

  private:
    expected_ue_activity_behaviour_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceSetupItemSURes-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_setup_item_su_res_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceSuspendItemSUSReq-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_suspend_item_sus_req_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceSwitchedItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_switched_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { pdu_session_expected_ue_activity_behaviour, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::pdu_session_expected_ue_activity_behaviour; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    expected_ue_activity_behaviour_s&       pdu_session_expected_ue_activity_behaviour() { return c; }
    const expected_ue_activity_behaviour_s& pdu_session_expected_ue_activity_behaviour() const { return c; }

  private:
    expected_ue_activity_behaviour_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceToBeSwitchedDLItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_to_be_switched_dl_item_ext_ies_o = protocol_ext_empty_o;

// PDUSessionResourceToReleaseItemRelCmd-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_to_release_item_rel_cmd_ext_ies_o = protocol_ext_empty_o;

// SliceOverloadList ::= SEQUENCE (SIZE (1..1024)) OF SliceOverloadItem
using slice_overload_list_l = dyn_array<slice_overload_item_s>;

// SupportedTAItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct supported_ta_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { cfg_tac_ind, rat_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cfg_tac_ind_e&       cfg_tac_ind();
    rat_info_e&          rat_info();
    const cfg_tac_ind_e& cfg_tac_ind() const;
    const rat_info_e&    rat_info() const;

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TAIListForPagingItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using tai_list_for_paging_item_ext_ies_o = protocol_ext_empty_o;

// UE-NGAP-ID-pair-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ue_ngap_id_pair_ext_ies_o = protocol_ext_empty_o;

// UE-PagingList ::= SEQUENCE (SIZE (1..4096)) OF UE-PagingItem
using ue_paging_list_l = dyn_array<ue_paging_item_s>;

using ue_associated_lc_ng_conn_item_ext_ies_container = protocol_ext_container_empty_l;

// UE-associatedLogicalNG-connectionItem ::= SEQUENCE
struct ue_associated_lc_ng_conn_item_s {
  bool                                            ext                    = false;
  bool                                            amf_ue_ngap_id_present = false;
  bool                                            ran_ue_ngap_id_present = false;
  bool                                            ie_exts_present        = false;
  uint64_t                                        amf_ue_ngap_id         = 0;
  uint64_t                                        ran_ue_ngap_id         = 0;
  ue_associated_lc_ng_conn_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEPresence ::= ENUMERATED
struct ue_presence_opts {
  enum options { in, out, unknown, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ue_presence_e = enumerated<ue_presence_opts, true>;

// UEPresenceInAreaOfInterestItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ue_presence_in_area_of_interest_item_ext_ies_o = protocol_ext_empty_o;

using multicast_group_paging_area_item_ext_ies_container = protocol_ext_container_empty_l;

// MulticastGroupPagingAreaItem ::= SEQUENCE
struct multicast_group_paging_area_item_s {
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  multicast_group_paging_area_s                      multicast_group_paging_area;
  ue_paging_list_l                                   ue_paging_list;
  multicast_group_paging_area_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NB-IoT-Paging-TimeWindow ::= ENUMERATED
struct nb_iot_paging_time_win_opts {
  enum options { s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using nb_iot_paging_time_win_e = enumerated<nb_iot_paging_time_win_opts, true>;

// NB-IoT-Paging-eDRXCycle ::= ENUMERATED
struct nb_iot_paging_e_drx_cycle_opts {
  enum options {
    hf2,
    hf4,
    hf6,
    hf8,
    hf10,
    hf12,
    hf14,
    hf16,
    hf32,
    hf64,
    hf128,
    hf256,
    hf512,
    hf1024,
    /*...*/ nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using nb_iot_paging_e_drx_cycle_e = enumerated<nb_iot_paging_e_drx_cycle_opts, true>;

// NB-IoT-Paging-eDRXInfo-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using nb_iot_paging_e_drx_info_ext_ies_o = protocol_ext_empty_o;

using ngran_tnl_assoc_to_rem_item_ext_ies_container = protocol_ext_container_empty_l;

// NGRAN-TNLAssociationToRemoveItem ::= SEQUENCE
struct ngran_tnl_assoc_to_rem_item_s {
  bool                                          tnl_assoc_transport_layer_address_amf_present = false;
  bool                                          ie_exts_present                               = false;
  cp_transport_layer_info_c                     tnl_assoc_transport_layer_address;
  cp_transport_layer_info_c                     tnl_assoc_transport_layer_address_amf;
  ngran_tnl_assoc_to_rem_item_ext_ies_container ie_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-CGIListForWarning ::= SEQUENCE (SIZE (1..65535)) OF NR-CGI
using nr_cgi_list_for_warning_l = dyn_array<nr_cgi_s>;

using overload_start_nssai_item_ext_ies_container = protocol_ext_container_empty_l;

// OverloadStartNSSAIItem ::= SEQUENCE
struct overload_start_nssai_item_s {
  bool                                        ext                                      = false;
  bool                                        slice_overload_resp_present              = false;
  bool                                        slice_traffic_load_reduction_ind_present = false;
  bool                                        ie_exts_present                          = false;
  slice_overload_list_l                       slice_overload_list;
  overload_resp_c                             slice_overload_resp;
  uint8_t                                     slice_traffic_load_reduction_ind = 1;
  overload_start_nssai_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_failed_to_modify_item_mod_cfm_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceFailedToModifyItemModCfm ::= SEQUENCE
struct pdu_session_res_failed_to_modify_item_mod_cfm_s {
  bool                                                            ext             = false;
  bool                                                            ie_exts_present = false;
  uint16_t                                                        pdu_session_id  = 0;
  unbounded_octstring<true>                                       pdu_session_res_modify_ind_unsuccessful_transfer;
  pdu_session_res_failed_to_modify_item_mod_cfm_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_failed_to_modify_item_mod_res_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceFailedToModifyItemModRes ::= SEQUENCE
struct pdu_session_res_failed_to_modify_item_mod_res_s {
  bool                                                            ext             = false;
  bool                                                            ie_exts_present = false;
  uint16_t                                                        pdu_session_id  = 0;
  unbounded_octstring<true>                                       pdu_session_res_modify_unsuccessful_transfer;
  pdu_session_res_failed_to_modify_item_mod_res_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_failed_to_resume_item_res_req_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceFailedToResumeItemRESReq ::= SEQUENCE
struct pdu_session_res_failed_to_resume_item_res_req_s {
  bool                                                            ext             = false;
  bool                                                            ie_exts_present = false;
  uint16_t                                                        pdu_session_id  = 0;
  cause_c                                                         cause;
  pdu_session_res_failed_to_resume_item_res_req_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_failed_to_resume_item_res_res_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceFailedToResumeItemRESRes ::= SEQUENCE
struct pdu_session_res_failed_to_resume_item_res_res_s {
  bool                                                            ext             = false;
  bool                                                            ie_exts_present = false;
  uint16_t                                                        pdu_session_id  = 0;
  cause_c                                                         cause;
  pdu_session_res_failed_to_resume_item_res_res_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_failed_to_setup_item_ps_req_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceFailedToSetupItemPSReq ::= SEQUENCE
struct pdu_session_res_failed_to_setup_item_ps_req_s {
  bool                                                          ext             = false;
  bool                                                          ie_exts_present = false;
  uint16_t                                                      pdu_session_id  = 0;
  unbounded_octstring<true>                                     path_switch_request_setup_failed_transfer;
  pdu_session_res_failed_to_setup_item_ps_req_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_failed_to_setup_item_su_res_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceFailedToSetupItemSURes ::= SEQUENCE
struct pdu_session_res_failed_to_setup_item_su_res_s {
  bool                                                          ext             = false;
  bool                                                          ie_exts_present = false;
  uint16_t                                                      pdu_session_id  = 0;
  unbounded_octstring<true>                                     pdu_session_res_setup_unsuccessful_transfer;
  pdu_session_res_failed_to_setup_item_su_res_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceItemCxtRelCpl ::= SEQUENCE
struct pdu_session_res_item_cxt_rel_cpl_s {
  bool                                                                 ext            = false;
  uint16_t                                                             pdu_session_id = 0;
  protocol_ext_container_l<pdu_session_res_item_cxt_rel_cpl_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_item_cxt_rel_req_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceItemCxtRelReq ::= SEQUENCE
struct pdu_session_res_item_cxt_rel_req_s {
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  uint16_t                                           pdu_session_id  = 0;
  pdu_session_res_item_cxt_rel_req_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_modify_item_mod_cfm_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceModifyItemModCfm ::= SEQUENCE
struct pdu_session_res_modify_item_mod_cfm_s {
  bool                                                  ext             = false;
  bool                                                  ie_exts_present = false;
  uint16_t                                              pdu_session_id  = 0;
  unbounded_octstring<true>                             pdu_session_res_modify_confirm_transfer;
  pdu_session_res_modify_item_mod_cfm_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_modify_item_mod_ind_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceModifyItemModInd ::= SEQUENCE
struct pdu_session_res_modify_item_mod_ind_s {
  bool                                                  ext             = false;
  bool                                                  ie_exts_present = false;
  uint16_t                                              pdu_session_id  = 0;
  unbounded_octstring<true>                             pdu_session_res_modify_ind_transfer;
  pdu_session_res_modify_item_mod_ind_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct pdu_session_res_modify_item_mod_req_ext_ies_container {
  bool                             s_nssai_present                                    = false;
  bool                             pdu_session_expected_ue_activity_behaviour_present = false;
  s_nssai_s                        s_nssai;
  expected_ue_activity_behaviour_s pdu_session_expected_ue_activity_behaviour;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyItemModReq ::= SEQUENCE
struct pdu_session_res_modify_item_mod_req_s {
  bool                                                  ext             = false;
  bool                                                  ie_exts_present = false;
  uint16_t                                              pdu_session_id  = 0;
  unbounded_octstring<true>                             nas_pdu;
  unbounded_octstring<true>                             pdu_session_res_modify_request_transfer;
  pdu_session_res_modify_item_mod_req_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_modify_item_mod_res_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceModifyItemModRes ::= SEQUENCE
struct pdu_session_res_modify_item_mod_res_s {
  bool                                                  ext             = false;
  bool                                                  ie_exts_present = false;
  uint16_t                                              pdu_session_id  = 0;
  unbounded_octstring<true>                             pdu_session_res_modify_resp_transfer;
  pdu_session_res_modify_item_mod_res_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_notify_item_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceNotifyItem ::= SEQUENCE
struct pdu_session_res_notify_item_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  uint16_t                                      pdu_session_id  = 0;
  unbounded_octstring<true>                     pdu_session_res_notify_transfer;
  pdu_session_res_notify_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_released_item_not_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceReleasedItemNot ::= SEQUENCE
struct pdu_session_res_released_item_not_s {
  bool                                                ext             = false;
  bool                                                ie_exts_present = false;
  uint16_t                                            pdu_session_id  = 0;
  unbounded_octstring<true>                           pdu_session_res_notify_released_transfer;
  pdu_session_res_released_item_not_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_released_item_ps_ack_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceReleasedItemPSAck ::= SEQUENCE
struct pdu_session_res_released_item_ps_ack_s {
  bool                                                   ext             = false;
  bool                                                   ie_exts_present = false;
  uint16_t                                               pdu_session_id  = 0;
  unbounded_octstring<true>                              path_switch_request_unsuccessful_transfer;
  pdu_session_res_released_item_ps_ack_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_released_item_ps_fail_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceReleasedItemPSFail ::= SEQUENCE
struct pdu_session_res_released_item_ps_fail_s {
  bool                                                    ext             = false;
  bool                                                    ie_exts_present = false;
  uint16_t                                                pdu_session_id  = 0;
  unbounded_octstring<true>                               path_switch_request_unsuccessful_transfer;
  pdu_session_res_released_item_ps_fail_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_released_item_rel_res_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceReleasedItemRelRes ::= SEQUENCE
struct pdu_session_res_released_item_rel_res_s {
  bool                                                    ext             = false;
  bool                                                    ie_exts_present = false;
  uint16_t                                                pdu_session_id  = 0;
  unbounded_octstring<true>                               pdu_session_res_release_resp_transfer;
  pdu_session_res_released_item_rel_res_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_resume_item_res_req_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceResumeItemRESReq ::= SEQUENCE
struct pdu_session_res_resume_item_res_req_s {
  bool                                                  ext             = false;
  bool                                                  ie_exts_present = false;
  uint16_t                                              pdu_session_id  = 0;
  unbounded_octstring<true>                             ue_context_resume_request_transfer;
  pdu_session_res_resume_item_res_req_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_resume_item_res_res_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceResumeItemRESRes ::= SEQUENCE
struct pdu_session_res_resume_item_res_res_s {
  bool                                                  ext             = false;
  bool                                                  ie_exts_present = false;
  uint16_t                                              pdu_session_id  = 0;
  unbounded_octstring<true>                             ue_context_resume_resp_transfer;
  pdu_session_res_resume_item_res_res_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_secondary_rat_usage_item_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceSecondaryRATUsageItem ::= SEQUENCE
struct pdu_session_res_secondary_rat_usage_item_s {
  bool                                                       ext             = false;
  bool                                                       ie_exts_present = false;
  uint16_t                                                   pdu_session_id  = 0;
  unbounded_octstring<true>                                  secondary_rat_data_usage_report_transfer;
  pdu_session_res_secondary_rat_usage_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupItemSUReq ::= SEQUENCE
struct pdu_session_res_setup_item_su_req_s {
  bool                                                                  ext            = false;
  uint16_t                                                              pdu_session_id = 0;
  unbounded_octstring<true>                                             pdu_session_nas_pdu;
  s_nssai_s                                                             s_nssai;
  unbounded_octstring<true>                                             pdu_session_res_setup_request_transfer;
  protocol_ext_container_l<pdu_session_res_setup_item_su_req_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_setup_item_su_res_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceSetupItemSURes ::= SEQUENCE
struct pdu_session_res_setup_item_su_res_s {
  bool                                                ext             = false;
  bool                                                ie_exts_present = false;
  uint16_t                                            pdu_session_id  = 0;
  unbounded_octstring<true>                           pdu_session_res_setup_resp_transfer;
  pdu_session_res_setup_item_su_res_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_suspend_item_sus_req_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceSuspendItemSUSReq ::= SEQUENCE
struct pdu_session_res_suspend_item_sus_req_s {
  bool                                                   ext             = false;
  bool                                                   ie_exts_present = false;
  uint16_t                                               pdu_session_id  = 0;
  unbounded_octstring<true>                              ue_context_suspend_request_transfer;
  pdu_session_res_suspend_item_sus_req_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSwitchedItem ::= SEQUENCE
struct pdu_session_res_switched_item_s {
  bool                                                              ext            = false;
  uint16_t                                                          pdu_session_id = 0;
  unbounded_octstring<true>                                         path_switch_request_ack_transfer;
  protocol_ext_container_l<pdu_session_res_switched_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_to_be_switched_dl_item_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceToBeSwitchedDLItem ::= SEQUENCE
struct pdu_session_res_to_be_switched_dl_item_s {
  bool                                                     ext             = false;
  bool                                                     ie_exts_present = false;
  uint16_t                                                 pdu_session_id  = 0;
  unbounded_octstring<true>                                path_switch_request_transfer;
  pdu_session_res_to_be_switched_dl_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_res_to_release_item_rel_cmd_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceToReleaseItemRelCmd ::= SEQUENCE
struct pdu_session_res_to_release_item_rel_cmd_s {
  bool                                                      ext             = false;
  bool                                                      ie_exts_present = false;
  uint16_t                                                  pdu_session_id  = 0;
  unbounded_octstring<true>                                 pdu_session_res_release_cmd_transfer;
  pdu_session_res_to_release_item_rel_cmd_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSFailedCellIDList-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using pws_failed_cell_id_list_ext_ies_o = protocol_ies_empty_o;

// PagingProbabilityInformation ::= ENUMERATED
struct paging_probability_info_opts {
  enum options {
    p00,
    p05,
    p10,
    p15,
    p20,
    p25,
    p30,
    p35,
    p40,
    p45,
    p50,
    p55,
    p60,
    p65,
    p70,
    p75,
    p80,
    p85,
    p90,
    p95,
    p100,
    // ...
    nulltype
  } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using paging_probability_info_e = enumerated<paging_probability_info_opts, true>;

// QMCDeactivation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using qmc_deactivation_ext_ies_o = protocol_ext_empty_o;

// QoEReferenceList ::= SEQUENCE (SIZE (1..16)) OF OCTET STRING (SIZE (6))
using qo_e_ref_list_l = bounded_array<fixed_octstring<6, true>, 16>;

// ResetAll ::= ENUMERATED
struct reset_all_opts {
  enum options { reset_all, /*...*/ nulltype } value;

  const char* to_string() const;
};
using reset_all_e = enumerated<reset_all_opts, true>;

// ResetType-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using reset_type_ext_ies_o = protocol_ies_empty_o;

struct supported_ta_item_ext_ies_container {
  bool          cfg_tac_ind_present = false;
  bool          rat_info_present    = false;
  cfg_tac_ind_e cfg_tac_ind;
  rat_info_e    rat_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedTAItem ::= SEQUENCE
struct supported_ta_item_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  fixed_octstring<3, true>            tac;
  broadcast_plmn_list_l               broadcast_plmn_list;
  supported_ta_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using tai_list_for_paging_item_ext_ies_container = protocol_ext_container_empty_l;

// TAIListForPagingItem ::= SEQUENCE
struct tai_list_for_paging_item_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  tai_s                                      tai;
  tai_list_for_paging_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAIListForWarning ::= SEQUENCE (SIZE (1..65535)) OF TAI
using tai_list_for_warning_l = dyn_array<tai_s>;

using ue_ngap_id_pair_ext_ies_container = protocol_ext_container_empty_l;

// UE-NGAP-ID-pair ::= SEQUENCE
struct ue_ngap_id_pair_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  uint64_t                          amf_ue_ngap_id  = 0;
  uint64_t                          ran_ue_ngap_id  = 0;
  ue_ngap_id_pair_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-NGAP-IDs-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using ue_ngap_ids_ext_ies_o = protocol_ies_empty_o;

// UE-associatedLogicalNG-connectionList ::= SEQUENCE (SIZE (1..65536)) OF UE-associatedLogicalNG-connectionItem
using ue_associated_lc_ng_conn_list_l = dyn_array<ue_associated_lc_ng_conn_item_s>;

// UEPagingIdentity-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using ue_paging_id_ext_ies_o = protocol_ies_empty_o;

using ue_presence_in_area_of_interest_item_ext_ies_container = protocol_ext_container_empty_l;

// UEPresenceInAreaOfInterestItem ::= SEQUENCE
struct ue_presence_in_area_of_interest_item_s {
  bool                                                   ext                    = false;
  bool                                                   ie_exts_present        = false;
  uint8_t                                                location_report_ref_id = 1;
  ue_presence_e                                          ue_presence;
  ue_presence_in_area_of_interest_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-CP-SecurityInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ul_cp_security_info_ext_ies_o = protocol_ext_empty_o;

// WUS-Assistance-Information-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using wus_assist_info_ext_ies_o = protocol_ext_empty_o;

// WarningAreaList-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using warning_area_list_ext_ies_o = protocol_ies_empty_o;

// CancelAllWarningMessages ::= ENUMERATED
struct cancel_all_warning_msgs_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using cancel_all_warning_msgs_e = enumerated<cancel_all_warning_msgs_opts, true>;

// ConcurrentWarningMessageInd ::= ENUMERATED
struct concurrent_warning_msg_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using concurrent_warning_msg_ind_e = enumerated<concurrent_warning_msg_ind_opts, true>;

// HandoverFlag ::= ENUMERATED
struct ho_flag_opts {
  enum options { ho_prep, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ho_flag_e = enumerated<ho_flag_opts, true>;

// IAB-Supported ::= ENUMERATED
struct iab_supported_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using iab_supported_e = enumerated<iab_supported_opts, true>;

// IMSVoiceSupportIndicator ::= ENUMERATED
struct ims_voice_support_ind_opts {
  enum options { supported, not_supported, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ims_voice_support_ind_e = enumerated<ims_voice_support_ind_opts, true>;

// MDTPLMNModificationList ::= SEQUENCE (SIZE (0..16)) OF OCTET STRING (SIZE (3))
using mdt_plmn_mod_list_l = bounded_array<fixed_octstring<3, true>, 16>;

// MulticastGroupPagingAreaList ::= SEQUENCE (SIZE (1..64)) OF MulticastGroupPagingAreaItem
using multicast_group_paging_area_list_l = dyn_array<multicast_group_paging_area_item_s>;

// NB-IoT-DefaultPagingDRX ::= ENUMERATED
struct nb_iot_default_paging_drx_opts {
  enum options { rf128, rf256, rf512, rf1024, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using nb_iot_default_paging_drx_e = enumerated<nb_iot_default_paging_drx_opts, true>;

using nb_iot_paging_e_drx_info_ext_ies_container = protocol_ext_container_empty_l;

// NB-IoT-Paging-eDRXInfo ::= SEQUENCE
struct nb_iot_paging_e_drx_info_s {
  bool                                       ext                            = false;
  bool                                       nb_iot_paging_time_win_present = false;
  bool                                       ie_exts_present                = false;
  nb_iot_paging_e_drx_cycle_e                nb_iot_paging_e_drx_cycle;
  nb_iot_paging_time_win_e                   nb_iot_paging_time_win;
  nb_iot_paging_e_drx_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NB-IoT-PagingDRX ::= ENUMERATED
struct nb_iot_paging_drx_opts {
  enum options { rf32, rf64, rf128, rf256, rf512, rf1024, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using nb_iot_paging_drx_e = enumerated<nb_iot_paging_drx_opts, true>;

// NGRAN-TNLAssociationToRemoveList ::= SEQUENCE (SIZE (1..32)) OF NGRAN-TNLAssociationToRemoveItem
using ngran_tnl_assoc_to_rem_list_l = dyn_array<ngran_tnl_assoc_to_rem_item_s>;

// OverloadStartNSSAIList ::= SEQUENCE (SIZE (1..1024)) OF OverloadStartNSSAIItem
using overload_start_nssai_list_l = dyn_array<overload_start_nssai_item_s>;

// PDUSessionResourceFailedToModifyListModCfm ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceFailedToModifyItemModCfm
using pdu_session_res_failed_to_modify_list_mod_cfm_l = dyn_array<pdu_session_res_failed_to_modify_item_mod_cfm_s>;

// PDUSessionResourceFailedToModifyListModRes ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceFailedToModifyItemModRes
using pdu_session_res_failed_to_modify_list_mod_res_l = dyn_array<pdu_session_res_failed_to_modify_item_mod_res_s>;

// PDUSessionResourceFailedToResumeListRESReq ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceFailedToResumeItemRESReq
using pdu_session_res_failed_to_resume_list_res_req_l = dyn_array<pdu_session_res_failed_to_resume_item_res_req_s>;

// PDUSessionResourceFailedToResumeListRESRes ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceFailedToResumeItemRESRes
using pdu_session_res_failed_to_resume_list_res_res_l = dyn_array<pdu_session_res_failed_to_resume_item_res_res_s>;

// PDUSessionResourceFailedToSetupListPSReq ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceFailedToSetupItemPSReq
using pdu_session_res_failed_to_setup_list_ps_req_l = dyn_array<pdu_session_res_failed_to_setup_item_ps_req_s>;

// PDUSessionResourceFailedToSetupListSURes ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceFailedToSetupItemSURes
using pdu_session_res_failed_to_setup_list_su_res_l = dyn_array<pdu_session_res_failed_to_setup_item_su_res_s>;

// PDUSessionResourceListCxtRelCpl ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceItemCxtRelCpl
using pdu_session_res_list_cxt_rel_cpl_l = dyn_array<pdu_session_res_item_cxt_rel_cpl_s>;

// PDUSessionResourceListCxtRelReq ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceItemCxtRelReq
using pdu_session_res_list_cxt_rel_req_l = dyn_array<pdu_session_res_item_cxt_rel_req_s>;

// PDUSessionResourceModifyListModCfm ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceModifyItemModCfm
using pdu_session_res_modify_list_mod_cfm_l = dyn_array<pdu_session_res_modify_item_mod_cfm_s>;

// PDUSessionResourceModifyListModInd ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceModifyItemModInd
using pdu_session_res_modify_list_mod_ind_l = dyn_array<pdu_session_res_modify_item_mod_ind_s>;

// PDUSessionResourceModifyListModReq ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceModifyItemModReq
using pdu_session_res_modify_list_mod_req_l = dyn_array<pdu_session_res_modify_item_mod_req_s>;

// PDUSessionResourceModifyListModRes ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceModifyItemModRes
using pdu_session_res_modify_list_mod_res_l = dyn_array<pdu_session_res_modify_item_mod_res_s>;

// PDUSessionResourceNotifyList ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceNotifyItem
using pdu_session_res_notify_list_l = dyn_array<pdu_session_res_notify_item_s>;

// PDUSessionResourceReleasedListNot ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceReleasedItemNot
using pdu_session_res_released_list_not_l = dyn_array<pdu_session_res_released_item_not_s>;

// PDUSessionResourceReleasedListPSAck ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceReleasedItemPSAck
using pdu_session_res_released_list_ps_ack_l = dyn_array<pdu_session_res_released_item_ps_ack_s>;

// PDUSessionResourceReleasedListPSFail ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceReleasedItemPSFail
using pdu_session_res_released_list_ps_fail_l = dyn_array<pdu_session_res_released_item_ps_fail_s>;

// PDUSessionResourceReleasedListRelRes ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceReleasedItemRelRes
using pdu_session_res_released_list_rel_res_l = dyn_array<pdu_session_res_released_item_rel_res_s>;

// PDUSessionResourceResumeListRESReq ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceResumeItemRESReq
using pdu_session_res_resume_list_res_req_l = dyn_array<pdu_session_res_resume_item_res_req_s>;

// PDUSessionResourceResumeListRESRes ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceResumeItemRESRes
using pdu_session_res_resume_list_res_res_l = dyn_array<pdu_session_res_resume_item_res_res_s>;

// PDUSessionResourceSecondaryRATUsageList ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceSecondaryRATUsageItem
using pdu_session_res_secondary_rat_usage_list_l = dyn_array<pdu_session_res_secondary_rat_usage_item_s>;

// PDUSessionResourceSetupListSUReq ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceSetupItemSUReq
using pdu_session_res_setup_list_su_req_l = dyn_array<pdu_session_res_setup_item_su_req_s>;

// PDUSessionResourceSetupListSURes ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceSetupItemSURes
using pdu_session_res_setup_list_su_res_l = dyn_array<pdu_session_res_setup_item_su_res_s>;

// PDUSessionResourceSuspendListSUSReq ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceSuspendItemSUSReq
using pdu_session_res_suspend_list_sus_req_l = dyn_array<pdu_session_res_suspend_item_sus_req_s>;

// PDUSessionResourceSwitchedList ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceSwitchedItem
using pdu_session_res_switched_list_l = dyn_array<pdu_session_res_switched_item_s>;

// PDUSessionResourceToBeSwitchedDLList ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceToBeSwitchedDLItem
using pdu_session_res_to_be_switched_dl_list_l = dyn_array<pdu_session_res_to_be_switched_dl_item_s>;

// PDUSessionResourceToReleaseListRelCmd ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceToReleaseItemRelCmd
using pdu_session_res_to_release_list_rel_cmd_l = dyn_array<pdu_session_res_to_release_item_rel_cmd_s>;

// PWSFailedCellIDList ::= CHOICE
struct pws_failed_cell_id_list_c {
  struct types_opts {
    enum options { eutra_cgi_pws_failed_list, nr_cgi_pws_failed_list, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  pws_failed_cell_id_list_c() = default;
  pws_failed_cell_id_list_c(const pws_failed_cell_id_list_c& other);
  pws_failed_cell_id_list_c& operator=(const pws_failed_cell_id_list_c& other);
  ~pws_failed_cell_id_list_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_cgi_list_l& eutra_cgi_pws_failed_list()
  {
    assert_choice_type(types::eutra_cgi_pws_failed_list, type_, "PWSFailedCellIDList");
    return c.get<eutra_cgi_list_l>();
  }
  nr_cgi_list_l& nr_cgi_pws_failed_list()
  {
    assert_choice_type(types::nr_cgi_pws_failed_list, type_, "PWSFailedCellIDList");
    return c.get<nr_cgi_list_l>();
  }
  protocol_ie_single_container_s<pws_failed_cell_id_list_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "PWSFailedCellIDList");
    return c.get<protocol_ie_single_container_s<pws_failed_cell_id_list_ext_ies_o>>();
  }
  const eutra_cgi_list_l& eutra_cgi_pws_failed_list() const
  {
    assert_choice_type(types::eutra_cgi_pws_failed_list, type_, "PWSFailedCellIDList");
    return c.get<eutra_cgi_list_l>();
  }
  const nr_cgi_list_l& nr_cgi_pws_failed_list() const
  {
    assert_choice_type(types::nr_cgi_pws_failed_list, type_, "PWSFailedCellIDList");
    return c.get<nr_cgi_list_l>();
  }
  const protocol_ie_single_container_s<pws_failed_cell_id_list_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "PWSFailedCellIDList");
    return c.get<protocol_ie_single_container_s<pws_failed_cell_id_list_ext_ies_o>>();
  }
  eutra_cgi_list_l&                                                  set_eutra_cgi_pws_failed_list();
  nr_cgi_list_l&                                                     set_nr_cgi_pws_failed_list();
  protocol_ie_single_container_s<pws_failed_cell_id_list_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<eutra_cgi_list_l, nr_cgi_list_l, protocol_ie_single_container_s<pws_failed_cell_id_list_ext_ies_o>> c;

  void destroy_();
};

// PagingCause ::= ENUMERATED
struct paging_cause_opts {
  enum options { voice, /*...*/ nulltype } value;

  const char* to_string() const;
};
using paging_cause_e = enumerated<paging_cause_opts, true>;

// PagingOrigin ::= ENUMERATED
struct paging_origin_opts {
  enum options { non_neg3gpp, /*...*/ nulltype } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
using paging_origin_e = enumerated<paging_origin_opts, true>;

// PagingPriority ::= ENUMERATED
struct paging_prio_opts {
  enum options {
    priolevel1,
    priolevel2,
    priolevel3,
    priolevel4,
    priolevel5,
    priolevel6,
    priolevel7,
    priolevel8,
    // ...
    nulltype
  } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using paging_prio_e = enumerated<paging_prio_opts, true>;

using qmc_deactivation_ext_ies_container = protocol_ext_container_empty_l;

// QMCDeactivation ::= SEQUENCE
struct qmc_deactivation_s {
  bool                               ext             = false;
  bool                               ie_exts_present = false;
  qo_e_ref_list_l                    qo_e_ref_list;
  qmc_deactivation_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCState ::= ENUMERATED
struct rrc_state_opts {
  enum options { inactive, connected, /*...*/ nulltype } value;

  const char* to_string() const;
};
using rrc_state_e = enumerated<rrc_state_opts, true>;

// ResetType ::= CHOICE
struct reset_type_c {
  struct types_opts {
    enum options { ng_interface, part_of_ng_interface, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  reset_type_c() = default;
  reset_type_c(const reset_type_c& other);
  reset_type_c& operator=(const reset_type_c& other);
  ~reset_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  reset_all_e& ng_interface()
  {
    assert_choice_type(types::ng_interface, type_, "ResetType");
    return c.get<reset_all_e>();
  }
  ue_associated_lc_ng_conn_list_l& part_of_ng_interface()
  {
    assert_choice_type(types::part_of_ng_interface, type_, "ResetType");
    return c.get<ue_associated_lc_ng_conn_list_l>();
  }
  protocol_ie_single_container_s<reset_type_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "ResetType");
    return c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>();
  }
  const reset_all_e& ng_interface() const
  {
    assert_choice_type(types::ng_interface, type_, "ResetType");
    return c.get<reset_all_e>();
  }
  const ue_associated_lc_ng_conn_list_l& part_of_ng_interface() const
  {
    assert_choice_type(types::part_of_ng_interface, type_, "ResetType");
    return c.get<ue_associated_lc_ng_conn_list_l>();
  }
  const protocol_ie_single_container_s<reset_type_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "ResetType");
    return c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>();
  }
  reset_all_e&                                          set_ng_interface();
  ue_associated_lc_ng_conn_list_l&                      set_part_of_ng_interface();
  protocol_ie_single_container_s<reset_type_ext_ies_o>& set_choice_exts();

private:
  types                                                                                                  type_;
  choice_buffer_t<protocol_ie_single_container_s<reset_type_ext_ies_o>, ue_associated_lc_ng_conn_list_l> c;

  void destroy_();
};

// SupportedTAList ::= SEQUENCE (SIZE (1..256)) OF SupportedTAItem
using supported_ta_list_l = dyn_array<supported_ta_item_s>;

// Suspend-Request-Indication ::= ENUMERATED
struct suspend_request_ind_opts {
  enum options { suspend_requested, /*...*/ nulltype } value;

  const char* to_string() const;
};
using suspend_request_ind_e = enumerated<suspend_request_ind_opts, true>;

// Suspend-Response-Indication ::= ENUMERATED
struct suspend_resp_ind_opts {
  enum options { suspend_indicated, /*...*/ nulltype } value;

  const char* to_string() const;
};
using suspend_resp_ind_e = enumerated<suspend_resp_ind_opts, true>;

// TAIListForPaging ::= SEQUENCE (SIZE (1..16)) OF TAIListForPagingItem
using tai_list_for_paging_l = dyn_array<tai_list_for_paging_item_s>;

// TAIListForRestart ::= SEQUENCE (SIZE (1..2048)) OF TAI
using tai_list_for_restart_l = dyn_array<tai_s>;

// UE-NGAP-IDs ::= CHOICE
struct ue_ngap_ids_c {
  struct types_opts {
    enum options { ue_ngap_id_pair, amf_ue_ngap_id, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ue_ngap_ids_c() = default;
  ue_ngap_ids_c(const ue_ngap_ids_c& other);
  ue_ngap_ids_c& operator=(const ue_ngap_ids_c& other);
  ~ue_ngap_ids_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ue_ngap_id_pair_s& ue_ngap_id_pair()
  {
    assert_choice_type(types::ue_ngap_id_pair, type_, "UE-NGAP-IDs");
    return c.get<ue_ngap_id_pair_s>();
  }
  uint64_t& amf_ue_ngap_id()
  {
    assert_choice_type(types::amf_ue_ngap_id, type_, "UE-NGAP-IDs");
    return c.get<uint64_t>();
  }
  protocol_ie_single_container_s<ue_ngap_ids_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "UE-NGAP-IDs");
    return c.get<protocol_ie_single_container_s<ue_ngap_ids_ext_ies_o>>();
  }
  const ue_ngap_id_pair_s& ue_ngap_id_pair() const
  {
    assert_choice_type(types::ue_ngap_id_pair, type_, "UE-NGAP-IDs");
    return c.get<ue_ngap_id_pair_s>();
  }
  const uint64_t& amf_ue_ngap_id() const
  {
    assert_choice_type(types::amf_ue_ngap_id, type_, "UE-NGAP-IDs");
    return c.get<uint64_t>();
  }
  const protocol_ie_single_container_s<ue_ngap_ids_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "UE-NGAP-IDs");
    return c.get<protocol_ie_single_container_s<ue_ngap_ids_ext_ies_o>>();
  }
  ue_ngap_id_pair_s&                                     set_ue_ngap_id_pair();
  uint64_t&                                              set_amf_ue_ngap_id();
  protocol_ie_single_container_s<ue_ngap_ids_ext_ies_o>& set_choice_exts();

private:
  types                                                                                     type_;
  choice_buffer_t<protocol_ie_single_container_s<ue_ngap_ids_ext_ies_o>, ue_ngap_id_pair_s> c;

  void destroy_();
};

// UEPagingIdentity ::= CHOICE
struct ue_paging_id_c {
  struct types_opts {
    enum options { five_g_s_tmsi, choice_exts, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ue_paging_id_c() = default;
  ue_paging_id_c(const ue_paging_id_c& other);
  ue_paging_id_c& operator=(const ue_paging_id_c& other);
  ~ue_paging_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  five_g_s_tmsi_s& five_g_s_tmsi()
  {
    assert_choice_type(types::five_g_s_tmsi, type_, "UEPagingIdentity");
    return c.get<five_g_s_tmsi_s>();
  }
  protocol_ie_single_container_s<ue_paging_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "UEPagingIdentity");
    return c.get<protocol_ie_single_container_s<ue_paging_id_ext_ies_o>>();
  }
  const five_g_s_tmsi_s& five_g_s_tmsi() const
  {
    assert_choice_type(types::five_g_s_tmsi, type_, "UEPagingIdentity");
    return c.get<five_g_s_tmsi_s>();
  }
  const protocol_ie_single_container_s<ue_paging_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "UEPagingIdentity");
    return c.get<protocol_ie_single_container_s<ue_paging_id_ext_ies_o>>();
  }
  five_g_s_tmsi_s&                                        set_five_g_s_tmsi();
  protocol_ie_single_container_s<ue_paging_id_ext_ies_o>& set_choice_exts();

private:
  types                                                                                    type_;
  choice_buffer_t<five_g_s_tmsi_s, protocol_ie_single_container_s<ue_paging_id_ext_ies_o>> c;

  void destroy_();
};

// UEPresenceInAreaOfInterestList ::= SEQUENCE (SIZE (1..64)) OF UEPresenceInAreaOfInterestItem
using ue_presence_in_area_of_interest_list_l = dyn_array<ue_presence_in_area_of_interest_item_s>;

// UERetentionInformation ::= ENUMERATED
struct ue_retention_info_opts {
  enum options { ues_retained, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ue_retention_info_e = enumerated<ue_retention_info_opts, true>;

using ul_cp_security_info_ext_ies_container = protocol_ext_container_empty_l;

// UL-CP-SecurityInformation ::= SEQUENCE
struct ul_cp_security_info_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  fixed_bitstring<16, false, true>      ul_nas_mac;
  fixed_bitstring<5, false, true>       ul_nas_count;
  ul_cp_security_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using wus_assist_info_ext_ies_container = protocol_ext_container_empty_l;

// WUS-Assistance-Information ::= SEQUENCE
struct wus_assist_info_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  paging_probability_info_e         paging_probability_info;
  wus_assist_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WarningAreaList ::= CHOICE
struct warning_area_list_c {
  struct types_opts {
    enum options {
      eutra_cgi_list_for_warning,
      nr_cgi_list_for_warning,
      tai_list_for_warning,
      emergency_area_id_list,
      choice_exts,
      nulltype
    } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  warning_area_list_c() = default;
  warning_area_list_c(const warning_area_list_c& other);
  warning_area_list_c& operator=(const warning_area_list_c& other);
  ~warning_area_list_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  eutra_cgi_list_for_warning_l& eutra_cgi_list_for_warning()
  {
    assert_choice_type(types::eutra_cgi_list_for_warning, type_, "WarningAreaList");
    return c.get<eutra_cgi_list_for_warning_l>();
  }
  nr_cgi_list_for_warning_l& nr_cgi_list_for_warning()
  {
    assert_choice_type(types::nr_cgi_list_for_warning, type_, "WarningAreaList");
    return c.get<nr_cgi_list_for_warning_l>();
  }
  tai_list_for_warning_l& tai_list_for_warning()
  {
    assert_choice_type(types::tai_list_for_warning, type_, "WarningAreaList");
    return c.get<tai_list_for_warning_l>();
  }
  emergency_area_id_list_l& emergency_area_id_list()
  {
    assert_choice_type(types::emergency_area_id_list, type_, "WarningAreaList");
    return c.get<emergency_area_id_list_l>();
  }
  protocol_ie_single_container_s<warning_area_list_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "WarningAreaList");
    return c.get<protocol_ie_single_container_s<warning_area_list_ext_ies_o>>();
  }
  const eutra_cgi_list_for_warning_l& eutra_cgi_list_for_warning() const
  {
    assert_choice_type(types::eutra_cgi_list_for_warning, type_, "WarningAreaList");
    return c.get<eutra_cgi_list_for_warning_l>();
  }
  const nr_cgi_list_for_warning_l& nr_cgi_list_for_warning() const
  {
    assert_choice_type(types::nr_cgi_list_for_warning, type_, "WarningAreaList");
    return c.get<nr_cgi_list_for_warning_l>();
  }
  const tai_list_for_warning_l& tai_list_for_warning() const
  {
    assert_choice_type(types::tai_list_for_warning, type_, "WarningAreaList");
    return c.get<tai_list_for_warning_l>();
  }
  const emergency_area_id_list_l& emergency_area_id_list() const
  {
    assert_choice_type(types::emergency_area_id_list, type_, "WarningAreaList");
    return c.get<emergency_area_id_list_l>();
  }
  const protocol_ie_single_container_s<warning_area_list_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "WarningAreaList");
    return c.get<protocol_ie_single_container_s<warning_area_list_ext_ies_o>>();
  }
  eutra_cgi_list_for_warning_l&                                set_eutra_cgi_list_for_warning();
  nr_cgi_list_for_warning_l&                                   set_nr_cgi_list_for_warning();
  tai_list_for_warning_l&                                      set_tai_list_for_warning();
  emergency_area_id_list_l&                                    set_emergency_area_id_list();
  protocol_ie_single_container_s<warning_area_list_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<emergency_area_id_list_l,
                  eutra_cgi_list_for_warning_l,
                  nr_cgi_list_for_warning_l,
                  protocol_ie_single_container_s<warning_area_list_ext_ies_o>,
                  tai_list_for_warning_l>
      c;

  void destroy_();
};

// LastVisitedPSCellInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using last_visited_pscell_info_ext_ies_o = protocol_ext_empty_o;

using last_visited_pscell_info_ext_ies_container = protocol_ext_container_empty_l;

// LastVisitedPSCellInformation ::= SEQUENCE
struct last_visited_pscell_info_s {
  bool                                       ext               = false;
  bool                                       pscell_id_present = false;
  bool                                       ie_exts_present   = false;
  ngran_cgi_c                                pscell_id;
  uint16_t                                   time_stay = 0;
  last_visited_pscell_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LastVisitedPSCellList ::= SEQUENCE (SIZE (1..8)) OF LastVisitedPSCellInformation
using last_visited_pscell_list_l = dyn_array<last_visited_pscell_info_s>;

// LastVisitedNGRANCellInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct last_visited_ngran_cell_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { last_visited_pscell_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::last_visited_pscell_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    last_visited_pscell_list_l&       last_visited_pscell_list() { return c; }
    const last_visited_pscell_list_l& last_visited_pscell_list() const { return c; }

  private:
    last_visited_pscell_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// LastVisitedCellInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using last_visited_cell_info_ext_ies_o = protocol_ies_empty_o;

// LastVisitedNGRANCellInformation ::= SEQUENCE
struct last_visited_ngran_cell_info_s {
  bool        ext                                                 = false;
  bool        time_ue_stayed_in_cell_enhanced_granularity_present = false;
  bool        ho_cause_value_present                              = false;
  ngran_cgi_c global_cell_id;
  cell_type_s cell_type;
  uint16_t    time_ue_stayed_in_cell                      = 0;
  uint16_t    time_ue_stayed_in_cell_enhanced_granularity = 0;
  cause_c     ho_cause_value;
  protocol_ext_container_l<last_visited_ngran_cell_info_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LastVisitedCellInformation ::= CHOICE
struct last_visited_cell_info_c {
  struct types_opts {
    enum options { ngran_cell, eutran_cell, utran_cell, geran_cell, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  last_visited_cell_info_c() = default;
  last_visited_cell_info_c(const last_visited_cell_info_c& other);
  last_visited_cell_info_c& operator=(const last_visited_cell_info_c& other);
  ~last_visited_cell_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  last_visited_ngran_cell_info_s& ngran_cell()
  {
    assert_choice_type(types::ngran_cell, type_, "LastVisitedCellInformation");
    return c.get<last_visited_ngran_cell_info_s>();
  }
  unbounded_octstring<true>& eutran_cell()
  {
    assert_choice_type(types::eutran_cell, type_, "LastVisitedCellInformation");
    return c.get<unbounded_octstring<true>>();
  }
  unbounded_octstring<true>& utran_cell()
  {
    assert_choice_type(types::utran_cell, type_, "LastVisitedCellInformation");
    return c.get<unbounded_octstring<true>>();
  }
  unbounded_octstring<true>& geran_cell()
  {
    assert_choice_type(types::geran_cell, type_, "LastVisitedCellInformation");
    return c.get<unbounded_octstring<true>>();
  }
  protocol_ie_single_container_s<last_visited_cell_info_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "LastVisitedCellInformation");
    return c.get<protocol_ie_single_container_s<last_visited_cell_info_ext_ies_o>>();
  }
  const last_visited_ngran_cell_info_s& ngran_cell() const
  {
    assert_choice_type(types::ngran_cell, type_, "LastVisitedCellInformation");
    return c.get<last_visited_ngran_cell_info_s>();
  }
  const unbounded_octstring<true>& eutran_cell() const
  {
    assert_choice_type(types::eutran_cell, type_, "LastVisitedCellInformation");
    return c.get<unbounded_octstring<true>>();
  }
  const unbounded_octstring<true>& utran_cell() const
  {
    assert_choice_type(types::utran_cell, type_, "LastVisitedCellInformation");
    return c.get<unbounded_octstring<true>>();
  }
  const unbounded_octstring<true>& geran_cell() const
  {
    assert_choice_type(types::geran_cell, type_, "LastVisitedCellInformation");
    return c.get<unbounded_octstring<true>>();
  }
  const protocol_ie_single_container_s<last_visited_cell_info_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "LastVisitedCellInformation");
    return c.get<protocol_ie_single_container_s<last_visited_cell_info_ext_ies_o>>();
  }
  last_visited_ngran_cell_info_s&                                   set_ngran_cell();
  unbounded_octstring<true>&                                        set_eutran_cell();
  unbounded_octstring<true>&                                        set_utran_cell();
  unbounded_octstring<true>&                                        set_geran_cell();
  protocol_ie_single_container_s<last_visited_cell_info_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<last_visited_ngran_cell_info_s,
                  protocol_ie_single_container_s<last_visited_cell_info_ext_ies_o>,
                  unbounded_octstring<true>>
      c;

  void destroy_();
};

// LastVisitedCellItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using last_visited_cell_item_ext_ies_o = protocol_ext_empty_o;

using last_visited_cell_item_ext_ies_container = protocol_ext_container_empty_l;

// LastVisitedCellItem ::= SEQUENCE
struct last_visited_cell_item_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  last_visited_cell_info_c                 last_visited_cell_info;
  last_visited_cell_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NonDynamic5QIDescriptor-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
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
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
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
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

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

// NonDynamic5QIDescriptor ::= SEQUENCE
struct non_dyn_5qi_descriptor_s {
  bool                                     ext                           = false;
  bool                                     prio_level_qos_present        = false;
  bool                                     averaging_win_present         = false;
  bool                                     max_data_burst_volume_present = false;
  bool                                     ie_exts_present               = false;
  uint16_t                                 five_qi                       = 0;
  uint8_t                                  prio_level_qos                = 1;
  uint16_t                                 averaging_win                 = 0;
  uint16_t                                 max_data_burst_volume         = 0;
  non_dyn_5qi_descriptor_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosCharacteristics-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using qos_characteristics_ext_ies_o = protocol_ies_empty_o;

// QosMonitoringRequest ::= ENUMERATED
struct qos_monitoring_request_opts {
  enum options { ul, dl, both, /*...*/ stop, nulltype } value;

  const char* to_string() const;
};
using qos_monitoring_request_e = enumerated<qos_monitoring_request_opts, true, 1>;

// AdditionalQosFlowInformation ::= ENUMERATED
struct add_qos_flow_info_opts {
  enum options { more_likely, /*...*/ nulltype } value;

  const char* to_string() const;
};
using add_qos_flow_info_e = enumerated<add_qos_flow_info_opts, true>;

// MRB-ProgressInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using mrb_progress_info_ext_ies_o = protocol_ies_empty_o;

// QosCharacteristics ::= CHOICE
struct qos_characteristics_c {
  struct types_opts {
    enum options { non_dyn5qi, dyn5qi, choice_exts, nulltype } value;

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
  non_dyn_5qi_descriptor_s& non_dyn5qi()
  {
    assert_choice_type(types::non_dyn5qi, type_, "QosCharacteristics");
    return c.get<non_dyn_5qi_descriptor_s>();
  }
  dyn_5qi_descriptor_s& dyn5qi()
  {
    assert_choice_type(types::dyn5qi, type_, "QosCharacteristics");
    return c.get<dyn_5qi_descriptor_s>();
  }
  protocol_ie_single_container_s<qos_characteristics_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "QosCharacteristics");
    return c.get<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>();
  }
  const non_dyn_5qi_descriptor_s& non_dyn5qi() const
  {
    assert_choice_type(types::non_dyn5qi, type_, "QosCharacteristics");
    return c.get<non_dyn_5qi_descriptor_s>();
  }
  const dyn_5qi_descriptor_s& dyn5qi() const
  {
    assert_choice_type(types::dyn5qi, type_, "QosCharacteristics");
    return c.get<dyn_5qi_descriptor_s>();
  }
  const protocol_ie_single_container_s<qos_characteristics_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "QosCharacteristics");
    return c.get<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>();
  }
  non_dyn_5qi_descriptor_s&                                      set_non_dyn5qi();
  dyn_5qi_descriptor_s&                                          set_dyn5qi();
  protocol_ie_single_container_s<qos_characteristics_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<dyn_5qi_descriptor_s,
                  non_dyn_5qi_descriptor_s,
                  protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>
      c;

  void destroy_();
};

// QosFlowLevelQosParameters-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct qos_flow_level_qos_params_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { qos_monitoring_request, qos_monitoring_report_freq, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_monitoring_request_e&       qos_monitoring_request();
    uint16_t&                       qos_monitoring_report_freq();
    const qos_monitoring_request_e& qos_monitoring_request() const;
    const uint16_t&                 qos_monitoring_report_freq() const;

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ReflectiveQosAttribute ::= ENUMERATED
struct reflective_qos_attribute_opts {
  enum options { subject_to, /*...*/ nulltype } value;

  const char* to_string() const;
};
using reflective_qos_attribute_e = enumerated<reflective_qos_attribute_opts, true>;

// MBS-MappingandDataForwardingRequestItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_mapand_data_forwarding_request_item_ext_ies_o = protocol_ext_empty_o;

// MBS-QoSFlowList ::= SEQUENCE (SIZE (1..64)) OF INTEGER (0..63,...)
using mbs_qos_flow_list_l = dyn_array<uint8_t>;

// MBS-QoSFlowsToBeSetupItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_qos_flows_to_be_setup_item_ext_ies_o = protocol_ext_empty_o;

// MRB-ProgressInformation ::= CHOICE
struct mrb_progress_info_c {
  struct types_opts {
    enum options { pdcp_sn_len12, pdcp_sn_len18, choice_exts, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  mrb_progress_info_c() = default;
  mrb_progress_info_c(const mrb_progress_info_c& other);
  mrb_progress_info_c& operator=(const mrb_progress_info_c& other);
  ~mrb_progress_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& pdcp_sn_len12()
  {
    assert_choice_type(types::pdcp_sn_len12, type_, "MRB-ProgressInformation");
    return c.get<uint16_t>();
  }
  uint32_t& pdcp_sn_len18()
  {
    assert_choice_type(types::pdcp_sn_len18, type_, "MRB-ProgressInformation");
    return c.get<uint32_t>();
  }
  protocol_ie_single_container_s<mrb_progress_info_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "MRB-ProgressInformation");
    return c.get<protocol_ie_single_container_s<mrb_progress_info_ext_ies_o>>();
  }
  const uint16_t& pdcp_sn_len12() const
  {
    assert_choice_type(types::pdcp_sn_len12, type_, "MRB-ProgressInformation");
    return c.get<uint16_t>();
  }
  const uint32_t& pdcp_sn_len18() const
  {
    assert_choice_type(types::pdcp_sn_len18, type_, "MRB-ProgressInformation");
    return c.get<uint32_t>();
  }
  const protocol_ie_single_container_s<mrb_progress_info_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "MRB-ProgressInformation");
    return c.get<protocol_ie_single_container_s<mrb_progress_info_ext_ies_o>>();
  }
  uint16_t&                                                    set_pdcp_sn_len12();
  uint32_t&                                                    set_pdcp_sn_len18();
  protocol_ie_single_container_s<mrb_progress_info_ext_ies_o>& set_choice_exts();

private:
  types                                                                        type_;
  choice_buffer_t<protocol_ie_single_container_s<mrb_progress_info_ext_ies_o>> c;

  void destroy_();
};

struct qos_flow_level_qos_params_ext_ies_container {
  bool                     qos_monitoring_request_present     = false;
  bool                     qos_monitoring_report_freq_present = false;
  qos_monitoring_request_e qos_monitoring_request;
  uint16_t                 qos_monitoring_report_freq;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowLevelQosParameters ::= SEQUENCE
struct qos_flow_level_qos_params_s {
  bool                                        ext                              = false;
  bool                                        gbr_qos_info_present             = false;
  bool                                        reflective_qos_attribute_present = false;
  bool                                        add_qos_flow_info_present        = false;
  bool                                        ie_exts_present                  = false;
  qos_characteristics_c                       qos_characteristics;
  alloc_and_retention_prio_s                  alloc_and_retention_prio;
  gbr_qos_info_s                              gbr_qos_info;
  reflective_qos_attribute_e                  reflective_qos_attribute;
  add_qos_flow_info_e                         add_qos_flow_info;
  qos_flow_level_qos_params_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mbs_mapand_data_forwarding_request_item_ext_ies_container = protocol_ext_container_empty_l;

// MBS-MappingandDataForwardingRequestItem ::= SEQUENCE
struct mbs_mapand_data_forwarding_request_item_s {
  bool                                                      ext                       = false;
  bool                                                      mrb_progress_info_present = false;
  bool                                                      ie_exts_present           = false;
  uint16_t                                                  mrb_id                    = 1;
  mbs_qos_flow_list_l                                       mbs_qos_flow_list;
  mrb_progress_info_c                                       mrb_progress_info;
  mbs_mapand_data_forwarding_request_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using mbs_qos_flows_to_be_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// MBS-QoSFlowsToBeSetupItem ::= SEQUENCE
struct mbs_qos_flows_to_be_setup_item_s {
  bool                                             ext             = false;
  bool                                             ie_exts_present = false;
  uint8_t                                          mb_sqos_flow_id = 0;
  qos_flow_level_qos_params_s                      mb_sqos_flow_level_qos_params;
  mbs_qos_flows_to_be_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-ActiveSessionInformation-SourcetoTargetItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_active_session_info_sourceto_target_item_ext_ies_o = protocol_ext_empty_o;

// MBS-MappingandDataForwardingRequestList ::= SEQUENCE (SIZE (1..32)) OF MBS-MappingandDataForwardingRequestItem
using mbs_mapand_data_forwarding_request_list_l = dyn_array<mbs_mapand_data_forwarding_request_item_s>;

// MBS-QoSFlowsToBeSetupList ::= SEQUENCE (SIZE (1..64)) OF MBS-QoSFlowsToBeSetupItem
using mbs_qos_flows_to_be_setup_list_l = dyn_array<mbs_qos_flows_to_be_setup_item_s>;

using mbs_active_session_info_sourceto_target_item_ext_ies_container = protocol_ext_container_empty_l;

// MBS-ActiveSessionInformation-SourcetoTargetItem ::= SEQUENCE
struct mbs_active_session_info_sourceto_target_item_s {
  bool                                                           ext                         = false;
  bool                                                           mbs_area_session_id_present = false;
  bool                                                           mbs_service_area_present    = false;
  bool                                                           ie_exts_present             = false;
  mbs_session_id_s                                               mbs_session_id;
  uint32_t                                                       mbs_area_session_id = 0;
  mbs_service_area_c                                             mbs_service_area;
  mbs_qos_flows_to_be_setup_list_l                               mbs_qos_flows_to_be_setup_list;
  mbs_mapand_data_forwarding_request_list_l                      mbs_mapand_data_forwarding_request_list;
  mbs_active_session_info_sourceto_target_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-ActiveSessionInformation-SourcetoTargetList ::= SEQUENCE (SIZE (1..256)) OF
// MBS-ActiveSessionInformation-SourcetoTargetItem
using mbs_active_session_info_sourceto_target_list_l = dyn_array<mbs_active_session_info_sourceto_target_item_s>;

// MBS-DataForwardingResponseMRBItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_data_forwarding_resp_mrb_item_ext_ies_o = protocol_ext_empty_o;

using mbs_data_forwarding_resp_mrb_item_ext_ies_container = protocol_ext_container_empty_l;

// MBS-DataForwardingResponseMRBItem ::= SEQUENCE
struct mbs_data_forwarding_resp_mrb_item_s {
  bool                                                ext                       = false;
  bool                                                mrb_progress_info_present = false;
  bool                                                ie_exts_present           = false;
  uint16_t                                            mrb_id                    = 1;
  up_transport_layer_info_c                           dl_forwarding_up_tnl_info;
  mrb_progress_info_c                                 mrb_progress_info;
  mbs_data_forwarding_resp_mrb_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-ActiveSessionInformation-TargettoSourceItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_active_session_info_targetto_source_item_ext_ies_o = protocol_ext_empty_o;

// MBS-DataForwardingResponseMRBList ::= SEQUENCE (SIZE (1..32)) OF MBS-DataForwardingResponseMRBItem
using mbs_data_forwarding_resp_mrb_list_l = dyn_array<mbs_data_forwarding_resp_mrb_item_s>;

using mbs_active_session_info_targetto_source_item_ext_ies_container = protocol_ext_container_empty_l;

// MBS-ActiveSessionInformation-TargettoSourceItem ::= SEQUENCE
struct mbs_active_session_info_targetto_source_item_s {
  bool                                                           ext             = false;
  bool                                                           ie_exts_present = false;
  mbs_session_id_s                                               mbs_session_id;
  mbs_data_forwarding_resp_mrb_list_l                            mbs_data_forwarding_resp_mrb_list;
  mbs_active_session_info_targetto_source_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-ActiveSessionInformation-TargettoSourceList ::= SEQUENCE (SIZE (1..256)) OF
// MBS-ActiveSessionInformation-TargettoSourceItem
using mbs_active_session_info_targetto_source_list_l = dyn_array<mbs_active_session_info_targetto_source_item_s>;

// MBS-DistributionReleaseRequesTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_distribution_release_reques_transfer_ext_ies_o = protocol_ext_empty_o;

using mbs_distribution_release_reques_transfer_ext_ies_container = protocol_ext_container_empty_l;

// MBS-DistributionReleaseRequestTransfer ::= SEQUENCE
struct mbs_distribution_release_request_transfer_s {
  bool                                                       ext                                 = false;
  bool                                                       mbs_area_session_id_present         = false;
  bool                                                       shared_ngu_unicast_tnl_info_present = false;
  bool                                                       ie_exts_present                     = false;
  mbs_session_id_s                                           mbs_session_id;
  uint32_t                                                   mbs_area_session_id = 0;
  up_transport_layer_info_c                                  shared_ngu_unicast_tnl_info;
  cause_c                                                    cause;
  mbs_distribution_release_reques_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-DistributionSetupRequestTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_distribution_setup_request_transfer_ext_ies_o = protocol_ext_empty_o;

using mbs_distribution_setup_request_transfer_ext_ies_container = protocol_ext_container_empty_l;

// MBS-DistributionSetupRequestTransfer ::= SEQUENCE
struct mbs_distribution_setup_request_transfer_s {
  bool                                                      ext                                 = false;
  bool                                                      mbs_area_session_id_present         = false;
  bool                                                      shared_ngu_unicast_tnl_info_present = false;
  bool                                                      ie_exts_present                     = false;
  mbs_session_id_s                                          mbs_session_id;
  uint32_t                                                  mbs_area_session_id = 0;
  up_transport_layer_info_c                                 shared_ngu_unicast_tnl_info;
  mbs_distribution_setup_request_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SharedNGU-MulticastTNLInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using shared_ngu_multicast_tnl_info_ext_ies_o = protocol_ext_empty_o;

// MBS-SessionTNLInfo5GCItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_session_tnl_info5_gc_item_ext_ies_o = protocol_ext_empty_o;

using shared_ngu_multicast_tnl_info_ext_ies_container = protocol_ext_container_empty_l;

// SharedNGU-MulticastTNLInformation ::= SEQUENCE
struct shared_ngu_multicast_tnl_info_s {
  bool                                            ext             = false;
  bool                                            ie_exts_present = false;
  bounded_bitstring<1, 160, true, true>           ip_multicast_address;
  bounded_bitstring<1, 160, true, true>           ip_source_address;
  fixed_octstring<4, true>                        gtp_teid;
  shared_ngu_multicast_tnl_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-DistributionSetupResponseTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_distribution_setup_resp_transfer_ext_ies_o = protocol_ext_empty_o;

using mbs_session_tnl_info5_gc_item_ext_ies_container = protocol_ext_container_empty_l;

// MBS-SessionTNLInfo5GCItem ::= SEQUENCE
struct mbs_session_tnl_info5_gc_item_s {
  bool                                            ext                 = false;
  bool                                            ie_exts_present     = false;
  uint32_t                                        mbs_area_session_id = 0;
  shared_ngu_multicast_tnl_info_s                 shared_ngu_multicast_tnl_info;
  mbs_session_tnl_info5_gc_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSSessionStatus ::= ENUMERATED
struct mbs_session_status_opts {
  enum options { activ, deactiv, /*...*/ nulltype } value;

  const char* to_string() const;
};
using mbs_session_status_e = enumerated<mbs_session_status_opts, true>;

using mbs_distribution_setup_resp_transfer_ext_ies_container = protocol_ext_container_empty_l;

// MBS-DistributionSetupResponseTransfer ::= SEQUENCE
struct mbs_distribution_setup_resp_transfer_s {
  bool                                                   ext                                   = false;
  bool                                                   mbs_area_session_id_present           = false;
  bool                                                   shared_ngu_multicast_tnl_info_present = false;
  bool                                                   mbs_service_area_present              = false;
  bool                                                   ie_exts_present                       = false;
  mbs_session_id_s                                       mbs_session_id;
  uint32_t                                               mbs_area_session_id = 0;
  mbs_session_tnl_info5_gc_item_s                        shared_ngu_multicast_tnl_info;
  mbs_qos_flows_to_be_setup_list_l                       mbs_qos_flows_to_be_setup_list;
  mbs_session_status_e                                   mbs_session_status;
  mbs_service_area_c                                     mbs_service_area;
  mbs_distribution_setup_resp_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-DistributionSetupUnsuccessfulTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_distribution_setup_unsuccessful_transfer_ext_ies_o = protocol_ext_empty_o;

using mbs_distribution_setup_unsuccessful_transfer_ext_ies_container = protocol_ext_container_empty_l;

// MBS-DistributionSetupUnsuccessfulTransfer ::= SEQUENCE
struct mbs_distribution_setup_unsuccessful_transfer_s {
  bool                                                           ext                         = false;
  bool                                                           mbs_area_session_id_present = false;
  bool                                                           crit_diagnostics_present    = false;
  bool                                                           ie_exts_present             = false;
  mbs_session_id_s                                               mbs_session_id;
  uint32_t                                                       mbs_area_session_id = 0;
  cause_c                                                        cause;
  crit_diagnostics_s                                             crit_diagnostics;
  mbs_distribution_setup_unsuccessful_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-SessionFSAIDList ::= SEQUENCE (SIZE (1..64)) OF OCTET STRING (SIZE (3))
using mbs_session_fsa_id_list_l = dyn_array<fixed_octstring<3, true>>;

// MBS-SessionTNLInfo5GC-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using mbs_session_tnl_info5_gc_ext_ies_o = protocol_ies_empty_o;

// MBS-SessionTNLInfo5GCList ::= SEQUENCE (SIZE (1..256)) OF MBS-SessionTNLInfo5GCItem
using mbs_session_tnl_info5_gc_list_l = dyn_array<mbs_session_tnl_info5_gc_item_s>;

// MBS-SessionTNLInfo5GC ::= CHOICE
struct mbs_session_tnl_info5_gc_c {
  struct types_opts {
    enum options { locationindependent, locationdependent, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  mbs_session_tnl_info5_gc_c() = default;
  mbs_session_tnl_info5_gc_c(const mbs_session_tnl_info5_gc_c& other);
  mbs_session_tnl_info5_gc_c& operator=(const mbs_session_tnl_info5_gc_c& other);
  ~mbs_session_tnl_info5_gc_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  shared_ngu_multicast_tnl_info_s& locationindependent()
  {
    assert_choice_type(types::locationindependent, type_, "MBS-SessionTNLInfo5GC");
    return c.get<shared_ngu_multicast_tnl_info_s>();
  }
  mbs_session_tnl_info5_gc_list_l& locationdependent()
  {
    assert_choice_type(types::locationdependent, type_, "MBS-SessionTNLInfo5GC");
    return c.get<mbs_session_tnl_info5_gc_list_l>();
  }
  protocol_ie_single_container_s<mbs_session_tnl_info5_gc_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "MBS-SessionTNLInfo5GC");
    return c.get<protocol_ie_single_container_s<mbs_session_tnl_info5_gc_ext_ies_o>>();
  }
  const shared_ngu_multicast_tnl_info_s& locationindependent() const
  {
    assert_choice_type(types::locationindependent, type_, "MBS-SessionTNLInfo5GC");
    return c.get<shared_ngu_multicast_tnl_info_s>();
  }
  const mbs_session_tnl_info5_gc_list_l& locationdependent() const
  {
    assert_choice_type(types::locationdependent, type_, "MBS-SessionTNLInfo5GC");
    return c.get<mbs_session_tnl_info5_gc_list_l>();
  }
  const protocol_ie_single_container_s<mbs_session_tnl_info5_gc_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "MBS-SessionTNLInfo5GC");
    return c.get<protocol_ie_single_container_s<mbs_session_tnl_info5_gc_ext_ies_o>>();
  }
  shared_ngu_multicast_tnl_info_s&                                    set_locationindependent();
  mbs_session_tnl_info5_gc_list_l&                                    set_locationdependent();
  protocol_ie_single_container_s<mbs_session_tnl_info5_gc_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<mbs_session_tnl_info5_gc_list_l,
                  protocol_ie_single_container_s<mbs_session_tnl_info5_gc_ext_ies_o>,
                  shared_ngu_multicast_tnl_info_s>
      c;

  void destroy_();
};

// MBS-SessionTNLInfoNGRANItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_session_tnl_info_ngran_item_ext_ies_o = protocol_ext_empty_o;

using mbs_session_tnl_info_ngran_item_ext_ies_container = protocol_ext_container_empty_l;

// MBS-SessionTNLInfoNGRANItem ::= SEQUENCE
struct mbs_session_tnl_info_ngran_item_s {
  bool                                              ext                                 = false;
  bool                                              shared_ngu_unicast_tnl_info_present = false;
  bool                                              ie_exts_present                     = false;
  uint32_t                                          mbs_area_session_id                 = 0;
  up_transport_layer_info_c                         shared_ngu_unicast_tnl_info;
  mbs_session_tnl_info_ngran_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-SessionTNLInfoNGRAN-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using mbs_session_tnl_info_ngran_ext_ies_o = protocol_ies_empty_o;

// MBS-SessionTNLInfoNGRANList ::= SEQUENCE (SIZE (1..256)) OF MBS-SessionTNLInfoNGRANItem
using mbs_session_tnl_info_ngran_list_l = dyn_array<mbs_session_tnl_info_ngran_item_s>;

// MBS-SessionTNLInfoNGRAN ::= CHOICE
struct mbs_session_tnl_info_ngran_c {
  struct types_opts {
    enum options { locationindependent, locationdependent, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  mbs_session_tnl_info_ngran_c() = default;
  mbs_session_tnl_info_ngran_c(const mbs_session_tnl_info_ngran_c& other);
  mbs_session_tnl_info_ngran_c& operator=(const mbs_session_tnl_info_ngran_c& other);
  ~mbs_session_tnl_info_ngran_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  up_transport_layer_info_c& locationindependent()
  {
    assert_choice_type(types::locationindependent, type_, "MBS-SessionTNLInfoNGRAN");
    return c.get<up_transport_layer_info_c>();
  }
  mbs_session_tnl_info_ngran_list_l& locationdependent()
  {
    assert_choice_type(types::locationdependent, type_, "MBS-SessionTNLInfoNGRAN");
    return c.get<mbs_session_tnl_info_ngran_list_l>();
  }
  protocol_ie_single_container_s<mbs_session_tnl_info_ngran_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "MBS-SessionTNLInfoNGRAN");
    return c.get<protocol_ie_single_container_s<mbs_session_tnl_info_ngran_ext_ies_o>>();
  }
  const up_transport_layer_info_c& locationindependent() const
  {
    assert_choice_type(types::locationindependent, type_, "MBS-SessionTNLInfoNGRAN");
    return c.get<up_transport_layer_info_c>();
  }
  const mbs_session_tnl_info_ngran_list_l& locationdependent() const
  {
    assert_choice_type(types::locationdependent, type_, "MBS-SessionTNLInfoNGRAN");
    return c.get<mbs_session_tnl_info_ngran_list_l>();
  }
  const protocol_ie_single_container_s<mbs_session_tnl_info_ngran_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "MBS-SessionTNLInfoNGRAN");
    return c.get<protocol_ie_single_container_s<mbs_session_tnl_info_ngran_ext_ies_o>>();
  }
  up_transport_layer_info_c&                                            set_locationindependent();
  mbs_session_tnl_info_ngran_list_l&                                    set_locationdependent();
  protocol_ie_single_container_s<mbs_session_tnl_info_ngran_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<mbs_session_tnl_info_ngran_list_l,
                  protocol_ie_single_container_s<mbs_session_tnl_info_ngran_ext_ies_o>,
                  up_transport_layer_info_c>
      c;

  void destroy_();
};

// MBSSessionFailedtoSetupItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_session_failedto_setup_item_ext_ies_o = protocol_ext_empty_o;

using mbs_session_failedto_setup_item_ext_ies_container = protocol_ext_container_empty_l;

// MBSSessionFailedtoSetupItem ::= SEQUENCE
struct mbs_session_failedto_setup_item_s {
  bool                                              ext                         = false;
  bool                                              mbs_area_session_id_present = false;
  bool                                              ie_exts_present             = false;
  mbs_session_id_s                                  mbs_session_id;
  uint32_t                                          mbs_area_session_id = 0;
  cause_c                                           cause;
  mbs_session_failedto_setup_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSSessionFailedtoSetupList ::= SEQUENCE (SIZE (1..32)) OF MBSSessionFailedtoSetupItem
using mbs_session_failedto_setup_list_l = dyn_array<mbs_session_failedto_setup_item_s>;

// MBSSessionReleaseResponseTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_session_release_resp_transfer_ext_ies_o = protocol_ext_empty_o;

using mbs_session_release_resp_transfer_ext_ies_container = protocol_ext_container_empty_l;

// MBSSessionReleaseResponseTransfer ::= SEQUENCE
struct mbs_session_release_resp_transfer_s {
  bool                                                ext                                = false;
  bool                                                mbs_session_tnl_info_ngran_present = false;
  bool                                                ie_exts_present                    = false;
  mbs_session_tnl_info_ngran_c                        mbs_session_tnl_info_ngran;
  mbs_session_release_resp_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSSessionSetupOrModFailureTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_session_setup_or_mod_fail_transfer_ext_ies_o = protocol_ext_empty_o;

using mbs_session_setup_or_mod_fail_transfer_ext_ies_container = protocol_ext_container_empty_l;

// MBSSessionSetupOrModFailureTransfer ::= SEQUENCE
struct mbs_session_setup_or_mod_fail_transfer_s {
  bool                                                     ext                      = false;
  bool                                                     crit_diagnostics_present = false;
  bool                                                     ie_exts_present          = false;
  cause_c                                                  cause;
  crit_diagnostics_s                                       crit_diagnostics;
  mbs_session_setup_or_mod_fail_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSSessionSetupOrModRequestTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct mbs_session_setup_or_mod_request_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mbs_session_tnl_info5_gc,
        mbs_qos_flows_to_be_setup_mod_list,
        mbs_session_fsa_id_list,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_tnl_info5_gc_c&             mbs_session_tnl_info5_gc();
    mbs_qos_flows_to_be_setup_list_l&       mbs_qos_flows_to_be_setup_mod_list();
    mbs_session_fsa_id_list_l&              mbs_session_fsa_id_list();
    const mbs_session_tnl_info5_gc_c&       mbs_session_tnl_info5_gc() const;
    const mbs_qos_flows_to_be_setup_list_l& mbs_qos_flows_to_be_setup_mod_list() const;
    const mbs_session_fsa_id_list_l&        mbs_session_fsa_id_list() const;

  private:
    types                                                                                                    type_;
    choice_buffer_t<mbs_qos_flows_to_be_setup_list_l, mbs_session_fsa_id_list_l, mbs_session_tnl_info5_gc_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct mbs_session_setup_or_mod_request_transfer_ies_container {
  bool                             mbs_session_tnl_info5_gc_present = false;
  bool                             mbs_session_fsa_id_list_present  = false;
  mbs_session_tnl_info5_gc_c       mbs_session_tnl_info5_gc;
  mbs_qos_flows_to_be_setup_list_l mbs_qos_flows_to_be_setup_mod_list;
  mbs_session_fsa_id_list_l        mbs_session_fsa_id_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSSessionSetupOrModRequestTransfer ::= SEQUENCE
using mbs_session_setup_or_mod_request_transfer_s =
    elementary_procedure_option<mbs_session_setup_or_mod_request_transfer_ies_container>;

// MBSSessionSetupOrModResponseTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_session_setup_or_mod_resp_transfer_ext_ies_o = protocol_ext_empty_o;

using mbs_session_setup_or_mod_resp_transfer_ext_ies_container = protocol_ext_container_empty_l;

// MBSSessionSetupOrModResponseTransfer ::= SEQUENCE
struct mbs_session_setup_or_mod_resp_transfer_s {
  bool                                                     ext                                = false;
  bool                                                     mbs_session_tnl_info_ngran_present = false;
  bool                                                     ie_exts_present                    = false;
  mbs_session_tnl_info_ngran_c                             mbs_session_tnl_info_ngran;
  mbs_session_setup_or_mod_resp_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSSessionSetupRequestItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_session_setup_request_item_ext_ies_o = protocol_ext_empty_o;

using mbs_session_setup_request_item_ext_ies_container = protocol_ext_container_empty_l;

// MBSSessionSetupRequestItem ::= SEQUENCE
struct mbs_session_setup_request_item_s {
  bool                                             ext                         = false;
  bool                                             mbs_area_session_id_present = false;
  bool                                             ie_exts_present             = false;
  mbs_session_id_s                                 mbs_session_id;
  uint32_t                                         mbs_area_session_id = 0;
  associated_mbs_qos_flow_setup_request_list_l     associated_mbs_qos_flow_setup_request_list;
  mbs_session_setup_request_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSSessionSetupRequestList ::= SEQUENCE (SIZE (1..32)) OF MBSSessionSetupRequestItem
using mbs_session_setup_request_list_l = dyn_array<mbs_session_setup_request_item_s>;

// MBSSessionSetupResponseItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_session_setup_resp_item_ext_ies_o = protocol_ext_empty_o;

using mbs_session_setup_resp_item_ext_ies_container = protocol_ext_container_empty_l;

// MBSSessionSetupResponseItem ::= SEQUENCE
struct mbs_session_setup_resp_item_s {
  bool                                          ext                         = false;
  bool                                          mbs_area_session_id_present = false;
  bool                                          ie_exts_present             = false;
  mbs_session_id_s                              mbs_session_id;
  uint32_t                                      mbs_area_session_id = 0;
  mbs_session_setup_resp_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSSessionSetupResponseList ::= SEQUENCE (SIZE (1..32)) OF MBSSessionSetupResponseItem
using mbs_session_setup_resp_list_l = dyn_array<mbs_session_setup_resp_item_s>;

// MBSSessionSetuporModifyRequestItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_session_setupor_modify_request_item_ext_ies_o = protocol_ext_empty_o;

using mbs_session_setupor_modify_request_item_ext_ies_container = protocol_ext_container_empty_l;

// MBSSessionSetuporModifyRequestItem ::= SEQUENCE
struct mbs_session_setupor_modify_request_item_s {
  bool                                                      ext                         = false;
  bool                                                      mbs_area_session_id_present = false;
  bool                                                      ie_exts_present             = false;
  mbs_session_id_s                                          mbs_session_id;
  uint32_t                                                  mbs_area_session_id = 0;
  associated_mbs_qos_flow_setupor_modify_request_list_l     associated_mbs_qos_flow_setupor_modify_request_list;
  qos_flow_list_with_cause_l                                mbs_qos_flow_to_release_list;
  mbs_session_setupor_modify_request_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSSessionSetuporModifyRequestList ::= SEQUENCE (SIZE (1..32)) OF MBSSessionSetuporModifyRequestItem
using mbs_session_setupor_modify_request_list_l = dyn_array<mbs_session_setupor_modify_request_item_s>;

// MBSSessionToReleaseItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using mbs_session_to_release_item_ext_ies_o = protocol_ext_empty_o;

using mbs_session_to_release_item_ext_ies_container = protocol_ext_container_empty_l;

// MBSSessionToReleaseItem ::= SEQUENCE
struct mbs_session_to_release_item_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  mbs_session_id_s                              mbs_session_id;
  cause_c                                       cause;
  mbs_session_to_release_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSSessionToReleaseList ::= SEQUENCE (SIZE (1..32)) OF MBSSessionToReleaseItem
using mbs_session_to_release_list_l = dyn_array<mbs_session_to_release_item_s>;

// MulticastSessionActivationRequestTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using multicast_session_activation_request_transfer_ext_ies_o = protocol_ext_empty_o;

using multicast_session_activation_request_transfer_ext_ies_container = protocol_ext_container_empty_l;

// MulticastSessionActivationRequestTransfer ::= SEQUENCE
struct multicast_session_activation_request_transfer_s {
  bool                                                            ext             = false;
  bool                                                            ie_exts_present = false;
  mbs_session_id_s                                                mbs_session_id;
  multicast_session_activation_request_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastSessionDeactivationRequestTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using multicast_session_deactivation_request_transfer_ext_ies_o = protocol_ext_empty_o;

using multicast_session_deactivation_request_transfer_ext_ies_container = protocol_ext_container_empty_l;

// MulticastSessionDeactivationRequestTransfer ::= SEQUENCE
struct multicast_session_deactivation_request_transfer_s {
  bool                                                              ext             = false;
  bool                                                              ie_exts_present = false;
  mbs_session_id_s                                                  mbs_session_id;
  multicast_session_deactivation_request_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastSessionUpdateRequestTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct multicast_session_upd_request_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        mbs_session_id,
        mbs_service_area,
        mbs_qos_flows_to_be_setup_mod_list,
        mbs_qos_flow_to_release_list,
        mbs_session_tnl_info5_gc,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbs_session_id_s&                       mbs_session_id();
    mbs_service_area_c&                     mbs_service_area();
    mbs_qos_flows_to_be_setup_list_l&       mbs_qos_flows_to_be_setup_mod_list();
    qos_flow_list_with_cause_l&             mbs_qos_flow_to_release_list();
    mbs_session_tnl_info5_gc_c&             mbs_session_tnl_info5_gc();
    const mbs_session_id_s&                 mbs_session_id() const;
    const mbs_service_area_c&               mbs_service_area() const;
    const mbs_qos_flows_to_be_setup_list_l& mbs_qos_flows_to_be_setup_mod_list() const;
    const qos_flow_list_with_cause_l&       mbs_qos_flow_to_release_list() const;
    const mbs_session_tnl_info5_gc_c&       mbs_session_tnl_info5_gc() const;

  private:
    types type_;
    choice_buffer_t<mbs_qos_flows_to_be_setup_list_l,
                    mbs_service_area_c,
                    mbs_session_id_s,
                    mbs_session_tnl_info5_gc_c,
                    qos_flow_list_with_cause_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct multicast_session_upd_request_transfer_ies_container {
  bool                             mbs_service_area_present                   = false;
  bool                             mbs_qos_flows_to_be_setup_mod_list_present = false;
  bool                             mbs_qos_flow_to_release_list_present       = false;
  bool                             mbs_session_tnl_info5_gc_present           = false;
  mbs_session_id_s                 mbs_session_id;
  mbs_service_area_c               mbs_service_area;
  mbs_qos_flows_to_be_setup_list_l mbs_qos_flows_to_be_setup_mod_list;
  qos_flow_list_with_cause_l       mbs_qos_flow_to_release_list;
  mbs_session_tnl_info5_gc_c       mbs_session_tnl_info5_gc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastSessionUpdateRequestTransfer ::= SEQUENCE
using multicast_session_upd_request_transfer_s =
    elementary_procedure_option<multicast_session_upd_request_transfer_ies_container>;

// NGAPIESupportInformationRequestItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ngap_ie_support_info_request_item_ext_ies_o = protocol_ext_empty_o;

using ngap_ie_support_info_request_item_ext_ies_container = protocol_ext_container_empty_l;

// NGAPIESupportInformationRequestItem ::= SEQUENCE
struct ngap_ie_support_info_request_item_s {
  bool                                                ext                 = false;
  bool                                                ie_exts_present     = false;
  uint32_t                                            ngap_protocol_ie_id = 0;
  ngap_ie_support_info_request_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGAPIESupportInformationRequestList ::= SEQUENCE (SIZE (1..32)) OF NGAPIESupportInformationRequestItem
using ngap_ie_support_info_request_list_l = dyn_array<ngap_ie_support_info_request_item_s>;

// NGAPIESupportInformationResponseItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ngap_ie_support_info_resp_item_ext_ies_o = protocol_ext_empty_o;

using ngap_ie_support_info_resp_item_ext_ies_container = protocol_ext_container_empty_l;

// NGAPIESupportInformationResponseItem ::= SEQUENCE
struct ngap_ie_support_info_resp_item_s {
  struct ngap_protocol_ie_support_info_opts {
    enum options { supported, not_supported, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ngap_protocol_ie_support_info_e_ = enumerated<ngap_protocol_ie_support_info_opts, true>;
  struct ngap_protocol_ie_presence_info_opts {
    enum options { present, not_present, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using ngap_protocol_ie_presence_info_e_ = enumerated<ngap_protocol_ie_presence_info_opts, true>;

  // member variables
  bool                                             ext                 = false;
  bool                                             ie_exts_present     = false;
  uint32_t                                         ngap_protocol_ie_id = 0;
  ngap_protocol_ie_support_info_e_                 ngap_protocol_ie_support_info;
  ngap_protocol_ie_presence_info_e_                ngap_protocol_ie_presence_info;
  ngap_ie_support_info_resp_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGAPIESupportInformationResponseList ::= SEQUENCE (SIZE (1..32)) OF NGAPIESupportInformationResponseItem
using ngap_ie_support_info_resp_list_l = dyn_array<ngap_ie_support_info_resp_item_s>;

// PDUSessionAggregateMaximumBitRate-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_aggr_max_bit_rate_ext_ies_o = protocol_ext_empty_o;

using pdu_session_aggr_max_bit_rate_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionAggregateMaximumBitRate ::= SEQUENCE
struct pdu_session_aggr_max_bit_rate_s {
  bool                                            ext                              = false;
  bool                                            ie_exts_present                  = false;
  uint64_t                                        pdu_session_aggr_max_bit_rate_dl = 0;
  uint64_t                                        pdu_session_aggr_max_bit_rate_ul = 0;
  pdu_session_aggr_max_bit_rate_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULForwarding ::= ENUMERATED
struct ul_forwarding_opts {
  enum options { ul_forwarding_proposed, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ul_forwarding_e = enumerated<ul_forwarding_opts, true>;

// QosFlowInformationItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct qos_flow_info_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { ul_forwarding, source_tnla_ddr_info, source_node_tnla_ddr_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ul_forwarding_e&                             ul_forwarding();
    bounded_bitstring<1, 160, true, true>&       source_tnla_ddr_info();
    bounded_bitstring<1, 160, true, true>&       source_node_tnla_ddr_info();
    const ul_forwarding_e&                       ul_forwarding() const;
    const bounded_bitstring<1, 160, true, true>& source_tnla_ddr_info() const;
    const bounded_bitstring<1, 160, true, true>& source_node_tnla_ddr_info() const;

  private:
    types                                                  type_;
    choice_buffer_t<bounded_bitstring<1, 160, true, true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct qos_flow_info_item_ext_ies_container {
  bool                                  ul_forwarding_present             = false;
  bool                                  source_tnla_ddr_info_present      = false;
  bool                                  source_node_tnla_ddr_info_present = false;
  ul_forwarding_e                       ul_forwarding;
  bounded_bitstring<1, 160, true, true> source_tnla_ddr_info;
  bounded_bitstring<1, 160, true, true> source_node_tnla_ddr_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowInformationItem ::= SEQUENCE
struct qos_flow_info_item_s {
  bool                                 ext                   = false;
  bool                                 dl_forwarding_present = false;
  bool                                 ie_exts_present       = false;
  uint8_t                              qos_flow_id           = 0;
  dl_forwarding_e                      dl_forwarding;
  qos_flow_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceInformationItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_info_item_ext_ies_o = protocol_ext_empty_o;

// QosFlowInformationList ::= SEQUENCE (SIZE (1..64)) OF QosFlowInformationItem
using qos_flow_info_list_l = dyn_array<qos_flow_info_item_s>;

using pdu_session_res_info_item_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceInformationItem ::= SEQUENCE
struct pdu_session_res_info_item_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  uint16_t                                    pdu_session_id  = 0;
  qos_flow_info_list_l                        qos_flow_info_list;
  drbs_to_qos_flows_map_list_l                drbs_to_qos_flows_map_list;
  pdu_session_res_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceInformationList ::= SEQUENCE (SIZE (1..256)) OF PDUSessionResourceInformationItem
using pdu_session_res_info_list_l = dyn_array<pdu_session_res_info_item_s>;

// UPTransportLayerInformationPairItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using up_transport_layer_info_pair_item_ext_ies_o = protocol_ext_empty_o;

// QosFlowModifyConfirmItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using qos_flow_modify_confirm_item_ext_ies_o = protocol_ext_empty_o;

using up_transport_layer_info_pair_item_ext_ies_container = protocol_ext_container_empty_l;

// UPTransportLayerInformationPairItem ::= SEQUENCE
struct up_transport_layer_info_pair_item_s {
  bool                                                ext             = false;
  bool                                                ie_exts_present = false;
  up_transport_layer_info_c                           ul_ngu_up_tnl_info;
  up_transport_layer_info_c                           dl_ngu_up_tnl_info;
  up_transport_layer_info_pair_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using qos_flow_modify_confirm_item_ext_ies_container = protocol_ext_container_empty_l;

// QosFlowModifyConfirmItem ::= SEQUENCE
struct qos_flow_modify_confirm_item_s {
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  uint8_t                                        qos_flow_id     = 0;
  qos_flow_modify_confirm_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UPTransportLayerInformationPairList ::= SEQUENCE (SIZE (1..3)) OF UPTransportLayerInformationPairItem
using up_transport_layer_info_pair_list_l = dyn_array<up_transport_layer_info_pair_item_s>;

// PDUSessionResourceModifyConfirmTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_modify_confirm_transfer_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { redundant_ul_ngu_up_tnl_info, add_redundant_ngu_up_tnl_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    up_transport_layer_info_c&                 redundant_ul_ngu_up_tnl_info();
    up_transport_layer_info_pair_list_l&       add_redundant_ngu_up_tnl_info();
    const up_transport_layer_info_c&           redundant_ul_ngu_up_tnl_info() const;
    const up_transport_layer_info_pair_list_l& add_redundant_ngu_up_tnl_info() const;

  private:
    types                                                                           type_;
    choice_buffer_t<up_transport_layer_info_c, up_transport_layer_info_pair_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// QosFlowModifyConfirmList ::= SEQUENCE (SIZE (1..64)) OF QosFlowModifyConfirmItem
using qos_flow_modify_confirm_list_l = dyn_array<qos_flow_modify_confirm_item_s>;

struct pdu_session_res_modify_confirm_transfer_ext_ies_container {
  bool                                redundant_ul_ngu_up_tnl_info_present  = false;
  bool                                add_redundant_ngu_up_tnl_info_present = false;
  up_transport_layer_info_c           redundant_ul_ngu_up_tnl_info;
  up_transport_layer_info_pair_list_l add_redundant_ngu_up_tnl_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyConfirmTransfer ::= SEQUENCE
struct pdu_session_res_modify_confirm_transfer_s {
  bool                                                      ext             = false;
  bool                                                      ie_exts_present = false;
  qos_flow_modify_confirm_list_l                            qos_flow_modify_confirm_list;
  up_transport_layer_info_c                                 ul_ngu_up_tnl_info;
  up_transport_layer_info_pair_list_l                       add_ng_u_up_tnl_info;
  qos_flow_list_with_cause_l                                qos_flow_failed_to_modify_list;
  pdu_session_res_modify_confirm_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VolumeTimedReport-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using volume_timed_report_item_ext_ies_o = protocol_ext_empty_o;

using volume_timed_report_item_ext_ies_container = protocol_ext_container_empty_l;

// VolumeTimedReport-Item ::= SEQUENCE
struct volume_timed_report_item_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  fixed_octstring<4, true>                   start_time_stamp;
  fixed_octstring<4, true>                   end_time_stamp;
  uint64_t                                   usage_count_ul = 0;
  uint64_t                                   usage_count_dl = 0;
  volume_timed_report_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoSFlowsUsageReport-Item-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using qos_flows_usage_report_item_ext_ies_o = protocol_ext_empty_o;

// VolumeTimedReportList ::= SEQUENCE (SIZE (1..2)) OF VolumeTimedReport-Item
using volume_timed_report_list_l = dyn_array<volume_timed_report_item_s>;

// PDUSessionUsageReport-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_usage_report_ext_ies_o = protocol_ext_empty_o;

using qos_flows_usage_report_item_ext_ies_container = protocol_ext_container_empty_l;

// QoSFlowsUsageReport-Item ::= SEQUENCE
struct qos_flows_usage_report_item_s {
  struct rat_type_opts {
    enum options { nr, eutra, /*...*/ nr_unlicensed, e_utra_unlicensed, nulltype } value;

    const char* to_string() const;
  };
  using rat_type_e_ = enumerated<rat_type_opts, true, 2>;

  // member variables
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  uint8_t                                       qos_flow_id     = 0;
  rat_type_e_                                   rat_type;
  volume_timed_report_list_l                    qos_flows_timed_report_list;
  qos_flows_usage_report_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pdu_session_usage_report_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionUsageReport ::= SEQUENCE
struct pdu_session_usage_report_s {
  struct rat_type_opts {
    enum options { nr, eutra, /*...*/ nr_unlicensed, e_utra_unlicensed, nulltype } value;

    const char* to_string() const;
  };
  using rat_type_e_ = enumerated<rat_type_opts, true, 2>;

  // member variables
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  rat_type_e_                                rat_type;
  volume_timed_report_list_l                 pdu_session_timed_report_list;
  pdu_session_usage_report_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoSFlowsUsageReportList ::= SEQUENCE (SIZE (1..64)) OF QoSFlowsUsageReport-Item
using qos_flows_usage_report_list_l = dyn_array<qos_flows_usage_report_item_s>;

// SecondaryRATUsageInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using secondary_rat_usage_info_ext_ies_o = protocol_ext_empty_o;

using secondary_rat_usage_info_ext_ies_container = protocol_ext_container_empty_l;

// SecondaryRATUsageInformation ::= SEQUENCE
struct secondary_rat_usage_info_s {
  bool                                       ext                              = false;
  bool                                       pdu_session_usage_report_present = false;
  bool                                       ie_ext_present                   = false;
  pdu_session_usage_report_s                 pdu_session_usage_report;
  qos_flows_usage_report_list_l              qos_flows_usage_report_list;
  secondary_rat_usage_info_ext_ies_container ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyIndicationTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_modify_ind_transfer_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        secondary_rat_usage_info,
        security_result,
        redundant_dl_qos_flow_per_tnl_info,
        add_redundant_dl_qos_flow_per_tnl_info,
        global_ran_node_id,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    secondary_rat_usage_info_s&         secondary_rat_usage_info();
    security_result_s&                  security_result();
    qos_flow_per_tnl_info_s&            redundant_dl_qos_flow_per_tnl_info();
    qos_flow_per_tnl_info_list_l&       add_redundant_dl_qos_flow_per_tnl_info();
    global_ran_node_id_c&               global_ran_node_id();
    const secondary_rat_usage_info_s&   secondary_rat_usage_info() const;
    const security_result_s&            security_result() const;
    const qos_flow_per_tnl_info_s&      redundant_dl_qos_flow_per_tnl_info() const;
    const qos_flow_per_tnl_info_list_l& add_redundant_dl_qos_flow_per_tnl_info() const;
    const global_ran_node_id_c&         global_ran_node_id() const;

  private:
    types type_;
    choice_buffer_t<global_ran_node_id_c,
                    qos_flow_per_tnl_info_list_l,
                    qos_flow_per_tnl_info_s,
                    secondary_rat_usage_info_s,
                    security_result_s>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct pdu_session_res_modify_ind_transfer_ext_ies_container {
  bool                         secondary_rat_usage_info_present               = false;
  bool                         security_result_present                        = false;
  bool                         redundant_dl_qos_flow_per_tnl_info_present     = false;
  bool                         add_redundant_dl_qos_flow_per_tnl_info_present = false;
  bool                         global_ran_node_id_present                     = false;
  secondary_rat_usage_info_s   secondary_rat_usage_info;
  security_result_s            security_result;
  qos_flow_per_tnl_info_s      redundant_dl_qos_flow_per_tnl_info;
  qos_flow_per_tnl_info_list_l add_redundant_dl_qos_flow_per_tnl_info;
  global_ran_node_id_c         global_ran_node_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyIndicationTransfer ::= SEQUENCE
struct pdu_session_res_modify_ind_transfer_s {
  bool                                                  ext             = false;
  bool                                                  ie_exts_present = false;
  qos_flow_per_tnl_info_s                               dl_qos_flow_per_tnl_info;
  qos_flow_per_tnl_info_list_l                          add_dl_qos_flow_per_tnl_info;
  pdu_session_res_modify_ind_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyIndicationUnsuccessfulTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_modify_ind_unsuccessful_transfer_ext_ies_o = protocol_ext_empty_o;

using pdu_session_res_modify_ind_unsuccessful_transfer_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceModifyIndicationUnsuccessfulTransfer ::= SEQUENCE
struct pdu_session_res_modify_ind_unsuccessful_transfer_s {
  bool                                                               ext             = false;
  bool                                                               ie_exts_present = false;
  cause_c                                                            cause;
  pdu_session_res_modify_ind_unsuccessful_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TSCAssistanceInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct tsc_assist_info_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { survival_time, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::survival_time; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&       survival_time() { return c; }
    const uint32_t& survival_time() const { return c; }

  private:
    uint32_t c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TSCAssistanceInformation ::= SEQUENCE
struct tsc_assist_info_s {
  bool                                                ext         = false;
  uint32_t                                            periodicity = 0;
  unbounded_octstring<true>                           burst_arrival_time;
  protocol_ext_container_l<tsc_assist_info_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TSCTrafficCharacteristics-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using tsc_traffic_characteristics_ext_ies_o = protocol_ext_empty_o;

// RedundantQosFlowIndicator ::= ENUMERATED
struct redundant_qos_flow_ind_opts {
  enum options { true_value, false_value, nulltype } value;

  const char* to_string() const;
};
using redundant_qos_flow_ind_e = enumerated<redundant_qos_flow_ind_opts>;

using tsc_traffic_characteristics_ext_ies_container = protocol_ext_container_empty_l;

// TSCTrafficCharacteristics ::= SEQUENCE
struct tsc_traffic_characteristics_s {
  bool                                          ext                        = false;
  bool                                          tsc_assist_info_dl_present = false;
  bool                                          tsc_assist_info_ul_present = false;
  bool                                          ie_exts_present            = false;
  tsc_assist_info_s                             tsc_assist_info_dl;
  tsc_assist_info_s                             tsc_assist_info_ul;
  tsc_traffic_characteristics_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MaximumIntegrityProtectedDataRate ::= ENUMERATED
struct max_integrity_protected_data_rate_opts {
  enum options { bitrate64kbs, max_ue_rate, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using max_integrity_protected_data_rate_e = enumerated<max_integrity_protected_data_rate_opts, true>;

// QosFlowAddOrModifyRequestItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct qos_flow_add_or_modify_request_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { tsc_traffic_characteristics, redundant_qos_flow_ind, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    tsc_traffic_characteristics_s&       tsc_traffic_characteristics();
    redundant_qos_flow_ind_e&            redundant_qos_flow_ind();
    const tsc_traffic_characteristics_s& tsc_traffic_characteristics() const;
    const redundant_qos_flow_ind_e&      redundant_qos_flow_ind() const;

  private:
    types                                          type_;
    choice_buffer_t<tsc_traffic_characteristics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UL-NGU-UP-TNLModifyItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct ul_ngu_up_tnl_modify_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { redundant_ul_ngu_up_tnl_info, redundant_dl_ngu_up_tnl_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    up_transport_layer_info_c&       redundant_ul_ngu_up_tnl_info();
    up_transport_layer_info_c&       redundant_dl_ngu_up_tnl_info();
    const up_transport_layer_info_c& redundant_ul_ngu_up_tnl_info() const;
    const up_transport_layer_info_c& redundant_dl_ngu_up_tnl_info() const;

  private:
    types                                      type_;
    choice_buffer_t<up_transport_layer_info_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ConfidentialityProtectionIndication ::= ENUMERATED
struct confidentiality_protection_ind_opts {
  enum options { required, preferred, not_needed, /*...*/ nulltype } value;

  const char* to_string() const;
};
using confidentiality_protection_ind_e = enumerated<confidentiality_protection_ind_opts, true>;

// IntegrityProtectionIndication ::= ENUMERATED
struct integrity_protection_ind_opts {
  enum options { required, preferred, not_needed, /*...*/ nulltype } value;

  const char* to_string() const;
};
using integrity_protection_ind_e = enumerated<integrity_protection_ind_opts, true>;

struct qos_flow_add_or_modify_request_item_ext_ies_container {
  bool                          tsc_traffic_characteristics_present = false;
  bool                          redundant_qos_flow_ind_present      = false;
  tsc_traffic_characteristics_s tsc_traffic_characteristics;
  redundant_qos_flow_ind_e      redundant_qos_flow_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowAddOrModifyRequestItem ::= SEQUENCE
struct qos_flow_add_or_modify_request_item_s {
  bool                                                  ext                               = false;
  bool                                                  qos_flow_level_qos_params_present = false;
  bool                                                  erab_id_present                   = false;
  bool                                                  ie_exts_present                   = false;
  uint8_t                                               qos_flow_id                       = 0;
  qos_flow_level_qos_params_s                           qos_flow_level_qos_params;
  uint8_t                                               erab_id = 0;
  qos_flow_add_or_modify_request_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SecurityIndication-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct security_ind_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { max_integrity_protected_data_rate_dl, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::max_integrity_protected_data_rate_dl; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    max_integrity_protected_data_rate_e&       max_integrity_protected_data_rate_dl() { return c; }
    const max_integrity_protected_data_rate_e& max_integrity_protected_data_rate_dl() const { return c; }

  private:
    max_integrity_protected_data_rate_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct ul_ngu_up_tnl_modify_item_ext_ies_container {
  bool                      redundant_ul_ngu_up_tnl_info_present = false;
  bool                      redundant_dl_ngu_up_tnl_info_present = false;
  up_transport_layer_info_c redundant_ul_ngu_up_tnl_info;
  up_transport_layer_info_c redundant_dl_ngu_up_tnl_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-NGU-UP-TNLModifyItem ::= SEQUENCE
struct ul_ngu_up_tnl_modify_item_s {
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  up_transport_layer_info_c                   ul_ngu_up_tnl_info;
  up_transport_layer_info_c                   dl_ngu_up_tnl_info;
  ul_ngu_up_tnl_modify_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowAddOrModifyRequestList ::= SEQUENCE (SIZE (1..64)) OF QosFlowAddOrModifyRequestItem
using qos_flow_add_or_modify_request_list_l = dyn_array<qos_flow_add_or_modify_request_item_s>;

// SecurityIndication ::= SEQUENCE
struct security_ind_s {
  bool                                             ext                                          = false;
  bool                                             max_integrity_protected_data_rate_ul_present = false;
  integrity_protection_ind_e                       integrity_protection_ind;
  confidentiality_protection_ind_e                 confidentiality_protection_ind;
  max_integrity_protected_data_rate_e              max_integrity_protected_data_rate_ul;
  protocol_ext_container_l<security_ind_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-NGU-UP-TNLModifyList ::= SEQUENCE (SIZE (1..4)) OF UL-NGU-UP-TNLModifyItem
using ul_ngu_up_tnl_modify_list_l = dyn_array<ul_ngu_up_tnl_modify_item_s>;

// PDUSessionResourceModifyRequestTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_modify_request_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        pdu_session_aggr_max_bit_rate,
        ul_ngu_up_tnl_modify_list,
        network_instance,
        qos_flow_add_or_modify_request_list,
        qos_flow_to_release_list,
        add_ul_ngu_up_tnl_info,
        common_network_instance,
        add_redundant_ul_ngu_up_tnl_info,
        redundant_common_network_instance,
        redundant_ul_ngu_up_tnl_info,
        security_ind,
        mbs_session_setupor_modify_request_list,
        mbs_session_to_release_list,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pdu_session_aggr_max_bit_rate_s&                 pdu_session_aggr_max_bit_rate();
    ul_ngu_up_tnl_modify_list_l&                     ul_ngu_up_tnl_modify_list();
    uint16_t&                                        network_instance();
    qos_flow_add_or_modify_request_list_l&           qos_flow_add_or_modify_request_list();
    qos_flow_list_with_cause_l&                      qos_flow_to_release_list();
    up_transport_layer_info_list_l&                  add_ul_ngu_up_tnl_info();
    unbounded_octstring<true>&                       common_network_instance();
    up_transport_layer_info_list_l&                  add_redundant_ul_ngu_up_tnl_info();
    unbounded_octstring<true>&                       redundant_common_network_instance();
    up_transport_layer_info_c&                       redundant_ul_ngu_up_tnl_info();
    security_ind_s&                                  security_ind();
    mbs_session_setupor_modify_request_list_l&       mbs_session_setupor_modify_request_list();
    mbs_session_to_release_list_l&                   mbs_session_to_release_list();
    const pdu_session_aggr_max_bit_rate_s&           pdu_session_aggr_max_bit_rate() const;
    const ul_ngu_up_tnl_modify_list_l&               ul_ngu_up_tnl_modify_list() const;
    const uint16_t&                                  network_instance() const;
    const qos_flow_add_or_modify_request_list_l&     qos_flow_add_or_modify_request_list() const;
    const qos_flow_list_with_cause_l&                qos_flow_to_release_list() const;
    const up_transport_layer_info_list_l&            add_ul_ngu_up_tnl_info() const;
    const unbounded_octstring<true>&                 common_network_instance() const;
    const up_transport_layer_info_list_l&            add_redundant_ul_ngu_up_tnl_info() const;
    const unbounded_octstring<true>&                 redundant_common_network_instance() const;
    const up_transport_layer_info_c&                 redundant_ul_ngu_up_tnl_info() const;
    const security_ind_s&                            security_ind() const;
    const mbs_session_setupor_modify_request_list_l& mbs_session_setupor_modify_request_list() const;
    const mbs_session_to_release_list_l&             mbs_session_to_release_list() const;

  private:
    types type_;
    choice_buffer_t<mbs_session_setupor_modify_request_list_l,
                    mbs_session_to_release_list_l,
                    pdu_session_aggr_max_bit_rate_s,
                    qos_flow_add_or_modify_request_list_l,
                    qos_flow_list_with_cause_l,
                    security_ind_s,
                    ul_ngu_up_tnl_modify_list_l,
                    unbounded_octstring<true>,
                    up_transport_layer_info_c,
                    up_transport_layer_info_list_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct pdu_session_res_modify_request_transfer_ies_container {
  bool                                      pdu_session_aggr_max_bit_rate_present           = false;
  bool                                      ul_ngu_up_tnl_modify_list_present               = false;
  bool                                      network_instance_present                        = false;
  bool                                      qos_flow_add_or_modify_request_list_present     = false;
  bool                                      qos_flow_to_release_list_present                = false;
  bool                                      add_ul_ngu_up_tnl_info_present                  = false;
  bool                                      common_network_instance_present                 = false;
  bool                                      add_redundant_ul_ngu_up_tnl_info_present        = false;
  bool                                      redundant_common_network_instance_present       = false;
  bool                                      redundant_ul_ngu_up_tnl_info_present            = false;
  bool                                      security_ind_present                            = false;
  bool                                      mbs_session_setupor_modify_request_list_present = false;
  bool                                      mbs_session_to_release_list_present             = false;
  pdu_session_aggr_max_bit_rate_s           pdu_session_aggr_max_bit_rate;
  ul_ngu_up_tnl_modify_list_l               ul_ngu_up_tnl_modify_list;
  uint16_t                                  network_instance;
  qos_flow_add_or_modify_request_list_l     qos_flow_add_or_modify_request_list;
  qos_flow_list_with_cause_l                qos_flow_to_release_list;
  up_transport_layer_info_list_l            add_ul_ngu_up_tnl_info;
  unbounded_octstring<true>                 common_network_instance;
  up_transport_layer_info_list_l            add_redundant_ul_ngu_up_tnl_info;
  unbounded_octstring<true>                 redundant_common_network_instance;
  up_transport_layer_info_c                 redundant_ul_ngu_up_tnl_info;
  security_ind_s                            security_ind;
  mbs_session_setupor_modify_request_list_l mbs_session_setupor_modify_request_list;
  mbs_session_to_release_list_l             mbs_session_to_release_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyRequestTransfer ::= SEQUENCE
using pdu_session_res_modify_request_transfer_s =
    elementary_procedure_option<pdu_session_res_modify_request_transfer_ies_container>;

// QosFlowAddOrModifyResponseItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct qos_flow_add_or_modify_resp_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { current_qos_para_set_idx, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::current_qos_para_set_idx; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&       current_qos_para_set_idx() { return c; }
    const uint8_t& current_qos_para_set_idx() const { return c; }

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

// QosFlowAddOrModifyResponseItem ::= SEQUENCE
struct qos_flow_add_or_modify_resp_item_s {
  bool                                                                 ext         = false;
  uint8_t                                                              qos_flow_id = 0;
  protocol_ext_container_l<qos_flow_add_or_modify_resp_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyResponseTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_modify_resp_transfer_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        add_ngu_up_tnl_info,
        redundant_dl_ngu_up_tnl_info,
        redundant_ul_ngu_up_tnl_info,
        add_redundant_dl_qos_flow_per_tnl_info,
        add_redundant_ngu_up_tnl_info,
        secondary_rat_usage_info,
        mbs_support_ind,
        mbs_session_setupor_modify_resp_list,
        mbs_session_failedto_setupor_modify_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    up_transport_layer_info_pair_list_l&       add_ngu_up_tnl_info();
    up_transport_layer_info_c&                 redundant_dl_ngu_up_tnl_info();
    up_transport_layer_info_c&                 redundant_ul_ngu_up_tnl_info();
    qos_flow_per_tnl_info_list_l&              add_redundant_dl_qos_flow_per_tnl_info();
    up_transport_layer_info_pair_list_l&       add_redundant_ngu_up_tnl_info();
    secondary_rat_usage_info_s&                secondary_rat_usage_info();
    mbs_support_ind_e&                         mbs_support_ind();
    mbs_session_setup_resp_list_l&             mbs_session_setupor_modify_resp_list();
    mbs_session_failedto_setup_list_l&         mbs_session_failedto_setupor_modify_list();
    const up_transport_layer_info_pair_list_l& add_ngu_up_tnl_info() const;
    const up_transport_layer_info_c&           redundant_dl_ngu_up_tnl_info() const;
    const up_transport_layer_info_c&           redundant_ul_ngu_up_tnl_info() const;
    const qos_flow_per_tnl_info_list_l&        add_redundant_dl_qos_flow_per_tnl_info() const;
    const up_transport_layer_info_pair_list_l& add_redundant_ngu_up_tnl_info() const;
    const secondary_rat_usage_info_s&          secondary_rat_usage_info() const;
    const mbs_support_ind_e&                   mbs_support_ind() const;
    const mbs_session_setup_resp_list_l&       mbs_session_setupor_modify_resp_list() const;
    const mbs_session_failedto_setup_list_l&   mbs_session_failedto_setupor_modify_list() const;

  private:
    types type_;
    choice_buffer_t<mbs_session_failedto_setup_list_l,
                    mbs_session_setup_resp_list_l,
                    qos_flow_per_tnl_info_list_l,
                    secondary_rat_usage_info_s,
                    up_transport_layer_info_c,
                    up_transport_layer_info_pair_list_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// QosFlowAddOrModifyResponseList ::= SEQUENCE (SIZE (1..64)) OF QosFlowAddOrModifyResponseItem
using qos_flow_add_or_modify_resp_list_l = dyn_array<qos_flow_add_or_modify_resp_item_s>;

struct pdu_session_res_modify_resp_transfer_ext_ies_container {
  bool                                add_ngu_up_tnl_info_present                      = false;
  bool                                redundant_dl_ngu_up_tnl_info_present             = false;
  bool                                redundant_ul_ngu_up_tnl_info_present             = false;
  bool                                add_redundant_dl_qos_flow_per_tnl_info_present   = false;
  bool                                add_redundant_ngu_up_tnl_info_present            = false;
  bool                                secondary_rat_usage_info_present                 = false;
  bool                                mbs_support_ind_present                          = false;
  bool                                mbs_session_setupor_modify_resp_list_present     = false;
  bool                                mbs_session_failedto_setupor_modify_list_present = false;
  up_transport_layer_info_pair_list_l add_ngu_up_tnl_info;
  up_transport_layer_info_c           redundant_dl_ngu_up_tnl_info;
  up_transport_layer_info_c           redundant_ul_ngu_up_tnl_info;
  qos_flow_per_tnl_info_list_l        add_redundant_dl_qos_flow_per_tnl_info;
  up_transport_layer_info_pair_list_l add_redundant_ngu_up_tnl_info;
  secondary_rat_usage_info_s          secondary_rat_usage_info;
  mbs_support_ind_e                   mbs_support_ind;
  mbs_session_setup_resp_list_l       mbs_session_setupor_modify_resp_list;
  mbs_session_failedto_setup_list_l   mbs_session_failedto_setupor_modify_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyResponseTransfer ::= SEQUENCE
struct pdu_session_res_modify_resp_transfer_s {
  bool                                                   ext                        = false;
  bool                                                   dl_ngu_up_tnl_info_present = false;
  bool                                                   ul_ngu_up_tnl_info_present = false;
  bool                                                   ie_exts_present            = false;
  up_transport_layer_info_c                              dl_ngu_up_tnl_info;
  up_transport_layer_info_c                              ul_ngu_up_tnl_info;
  qos_flow_add_or_modify_resp_list_l                     qos_flow_add_or_modify_resp_list;
  qos_flow_per_tnl_info_list_l                           add_dl_qos_flow_per_tnl_info;
  qos_flow_list_with_cause_l                             qos_flow_failed_to_add_or_modify_list;
  pdu_session_res_modify_resp_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceModifyUnsuccessfulTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_modify_unsuccessful_transfer_ext_ies_o = protocol_ext_empty_o;

using pdu_session_res_modify_unsuccessful_transfer_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceModifyUnsuccessfulTransfer ::= SEQUENCE
struct pdu_session_res_modify_unsuccessful_transfer_s {
  bool                                                           ext                      = false;
  bool                                                           crit_diagnostics_present = false;
  bool                                                           ie_exts_present          = false;
  cause_c                                                        cause;
  crit_diagnostics_s                                             crit_diagnostics;
  pdu_session_res_modify_unsuccessful_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceNotifyReleasedTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_notify_released_transfer_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { secondary_rat_usage_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::secondary_rat_usage_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    secondary_rat_usage_info_s&       secondary_rat_usage_info() { return c; }
    const secondary_rat_usage_info_s& secondary_rat_usage_info() const { return c; }

  private:
    secondary_rat_usage_info_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceNotifyReleasedTransfer ::= SEQUENCE
struct pdu_session_res_notify_released_transfer_s {
  bool                                                                         ext = false;
  cause_c                                                                      cause;
  protocol_ext_container_l<pdu_session_res_notify_released_transfer_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowFeedbackItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using qos_flow_feedback_item_ext_ies_o = protocol_ext_empty_o;

// NotificationCause ::= ENUMERATED
struct notif_cause_opts {
  enum options { fulfilled, not_fulfilled, /*...*/ nulltype } value;

  const char* to_string() const;
};
using notif_cause_e = enumerated<notif_cause_opts, true>;

using qos_flow_feedback_item_ext_ies_container = protocol_ext_container_empty_l;

// QosFlowFeedbackItem ::= SEQUENCE
struct qos_flow_feedback_item_s {
  bool                                     ext                              = false;
  bool                                     upd_feedback_present             = false;
  bool                                     cnpacket_delay_budget_dl_present = false;
  bool                                     cnpacket_delay_budget_ul_present = false;
  bool                                     ie_exts_present                  = false;
  uint8_t                                  qos_flow_id                      = 0;
  fixed_bitstring<8, true, true>           upd_feedback;
  uint32_t                                 cnpacket_delay_budget_dl = 1;
  uint32_t                                 cnpacket_delay_budget_ul = 1;
  qos_flow_feedback_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowNotifyItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct qos_flow_notify_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { current_qos_para_set_idx, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::current_qos_para_set_idx; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&       current_qos_para_set_idx() { return c; }
    const uint8_t& current_qos_para_set_idx() const { return c; }

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

// QosFlowFeedbackList ::= SEQUENCE (SIZE (1..64)) OF QosFlowFeedbackItem
using qos_flow_feedback_list_l = dyn_array<qos_flow_feedback_item_s>;

// QosFlowNotifyItem ::= SEQUENCE
struct qos_flow_notify_item_s {
  bool                                                     ext         = false;
  uint8_t                                                  qos_flow_id = 0;
  notif_cause_e                                            notif_cause;
  protocol_ext_container_l<qos_flow_notify_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceNotifyTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_notify_transfer_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { secondary_rat_usage_info, qos_flow_feedback_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    secondary_rat_usage_info_s&       secondary_rat_usage_info();
    qos_flow_feedback_list_l&         qos_flow_feedback_list();
    const secondary_rat_usage_info_s& secondary_rat_usage_info() const;
    const qos_flow_feedback_list_l&   qos_flow_feedback_list() const;

  private:
    types                                                                 type_;
    choice_buffer_t<qos_flow_feedback_list_l, secondary_rat_usage_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// QosFlowNotifyList ::= SEQUENCE (SIZE (1..64)) OF QosFlowNotifyItem
using qos_flow_notify_list_l = dyn_array<qos_flow_notify_item_s>;

struct pdu_session_res_notify_transfer_ext_ies_container {
  bool                       secondary_rat_usage_info_present = false;
  bool                       qos_flow_feedback_list_present   = false;
  secondary_rat_usage_info_s secondary_rat_usage_info;
  qos_flow_feedback_list_l   qos_flow_feedback_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceNotifyTransfer ::= SEQUENCE
struct pdu_session_res_notify_transfer_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  qos_flow_notify_list_l                            qos_flow_notify_list;
  qos_flow_list_with_cause_l                        qos_flow_released_list;
  pdu_session_res_notify_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceReleaseCommandTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_release_cmd_transfer_ext_ies_o = protocol_ext_empty_o;

using pdu_session_res_release_cmd_transfer_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceReleaseCommandTransfer ::= SEQUENCE
struct pdu_session_res_release_cmd_transfer_s {
  bool                                                   ext             = false;
  bool                                                   ie_exts_present = false;
  cause_c                                                cause;
  pdu_session_res_release_cmd_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceReleaseResponseTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_release_resp_transfer_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { secondary_rat_usage_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::secondary_rat_usage_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    secondary_rat_usage_info_s&       secondary_rat_usage_info() { return c; }
    const secondary_rat_usage_info_s& secondary_rat_usage_info() const { return c; }

  private:
    secondary_rat_usage_info_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDUSessionResourceReleaseResponseTransfer ::= SEQUENCE
struct pdu_session_res_release_resp_transfer_s {
  bool                                                                      ext = false;
  protocol_ext_container_l<pdu_session_res_release_resp_transfer_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowSetupRequestItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct qos_flow_setup_request_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { tsc_traffic_characteristics, redundant_qos_flow_ind, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    tsc_traffic_characteristics_s&       tsc_traffic_characteristics();
    redundant_qos_flow_ind_e&            redundant_qos_flow_ind();
    const tsc_traffic_characteristics_s& tsc_traffic_characteristics() const;
    const redundant_qos_flow_ind_e&      redundant_qos_flow_ind() const;

  private:
    types                                          type_;
    choice_buffer_t<tsc_traffic_characteristics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct qos_flow_setup_request_item_ext_ies_container {
  bool                          tsc_traffic_characteristics_present = false;
  bool                          redundant_qos_flow_ind_present      = false;
  tsc_traffic_characteristics_s tsc_traffic_characteristics;
  redundant_qos_flow_ind_e      redundant_qos_flow_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowSetupRequestItem ::= SEQUENCE
struct qos_flow_setup_request_item_s {
  bool                                          ext             = false;
  bool                                          erab_id_present = false;
  bool                                          ie_exts_present = false;
  uint8_t                                       qos_flow_id     = 0;
  qos_flow_level_qos_params_s                   qos_flow_level_qos_params;
  uint8_t                                       erab_id = 0;
  qos_flow_setup_request_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DataForwardingNotPossible ::= ENUMERATED
struct data_forwarding_not_possible_opts {
  enum options { data_forwarding_not_possible, /*...*/ nulltype } value;

  const char* to_string() const;
};
using data_forwarding_not_possible_e = enumerated<data_forwarding_not_possible_opts, true>;

// PDUSessionType ::= ENUMERATED
struct pdu_session_type_opts {
  enum options { ipv4, ipv6, ipv4v6, ethernet, unstructured, /*...*/ nulltype } value;

  const char* to_string() const;
};
using pdu_session_type_e = enumerated<pdu_session_type_opts, true>;

// QosFlowSetupRequestList ::= SEQUENCE (SIZE (1..64)) OF QosFlowSetupRequestItem
using qos_flow_setup_request_list_l = dyn_array<qos_flow_setup_request_item_s>;

// PDUSessionResourceSetupRequestTransferIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
struct pdu_session_res_setup_request_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        pdu_session_aggr_max_bit_rate,
        ul_ngu_up_tnl_info,
        add_ul_ngu_up_tnl_info,
        data_forwarding_not_possible,
        pdu_session_type,
        security_ind,
        network_instance,
        qos_flow_setup_request_list,
        common_network_instance,
        direct_forwarding_path_availability,
        redundant_ul_ngu_up_tnl_info,
        add_redundant_ul_ngu_up_tnl_info,
        redundant_common_network_instance,
        redundant_pdu_session_info,
        mbs_session_setup_request_list,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pdu_session_aggr_max_bit_rate_s&             pdu_session_aggr_max_bit_rate();
    up_transport_layer_info_c&                   ul_ngu_up_tnl_info();
    up_transport_layer_info_list_l&              add_ul_ngu_up_tnl_info();
    data_forwarding_not_possible_e&              data_forwarding_not_possible();
    pdu_session_type_e&                          pdu_session_type();
    security_ind_s&                              security_ind();
    uint16_t&                                    network_instance();
    qos_flow_setup_request_list_l&               qos_flow_setup_request_list();
    unbounded_octstring<true>&                   common_network_instance();
    direct_forwarding_path_availability_e&       direct_forwarding_path_availability();
    up_transport_layer_info_c&                   redundant_ul_ngu_up_tnl_info();
    up_transport_layer_info_list_l&              add_redundant_ul_ngu_up_tnl_info();
    unbounded_octstring<true>&                   redundant_common_network_instance();
    redundant_pdu_session_info_s&                redundant_pdu_session_info();
    mbs_session_setup_request_list_l&            mbs_session_setup_request_list();
    const pdu_session_aggr_max_bit_rate_s&       pdu_session_aggr_max_bit_rate() const;
    const up_transport_layer_info_c&             ul_ngu_up_tnl_info() const;
    const up_transport_layer_info_list_l&        add_ul_ngu_up_tnl_info() const;
    const data_forwarding_not_possible_e&        data_forwarding_not_possible() const;
    const pdu_session_type_e&                    pdu_session_type() const;
    const security_ind_s&                        security_ind() const;
    const uint16_t&                              network_instance() const;
    const qos_flow_setup_request_list_l&         qos_flow_setup_request_list() const;
    const unbounded_octstring<true>&             common_network_instance() const;
    const direct_forwarding_path_availability_e& direct_forwarding_path_availability() const;
    const up_transport_layer_info_c&             redundant_ul_ngu_up_tnl_info() const;
    const up_transport_layer_info_list_l&        add_redundant_ul_ngu_up_tnl_info() const;
    const unbounded_octstring<true>&             redundant_common_network_instance() const;
    const redundant_pdu_session_info_s&          redundant_pdu_session_info() const;
    const mbs_session_setup_request_list_l&      mbs_session_setup_request_list() const;

  private:
    types type_;
    choice_buffer_t<mbs_session_setup_request_list_l,
                    pdu_session_aggr_max_bit_rate_s,
                    qos_flow_setup_request_list_l,
                    redundant_pdu_session_info_s,
                    security_ind_s,
                    unbounded_octstring<true>,
                    up_transport_layer_info_c,
                    up_transport_layer_info_list_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct pdu_session_res_setup_request_transfer_ies_container {
  bool                                  pdu_session_aggr_max_bit_rate_present       = false;
  bool                                  add_ul_ngu_up_tnl_info_present              = false;
  bool                                  data_forwarding_not_possible_present        = false;
  bool                                  security_ind_present                        = false;
  bool                                  network_instance_present                    = false;
  bool                                  common_network_instance_present             = false;
  bool                                  direct_forwarding_path_availability_present = false;
  bool                                  redundant_ul_ngu_up_tnl_info_present        = false;
  bool                                  add_redundant_ul_ngu_up_tnl_info_present    = false;
  bool                                  redundant_common_network_instance_present   = false;
  bool                                  redundant_pdu_session_info_present          = false;
  bool                                  mbs_session_setup_request_list_present      = false;
  pdu_session_aggr_max_bit_rate_s       pdu_session_aggr_max_bit_rate;
  up_transport_layer_info_c             ul_ngu_up_tnl_info;
  up_transport_layer_info_list_l        add_ul_ngu_up_tnl_info;
  data_forwarding_not_possible_e        data_forwarding_not_possible;
  pdu_session_type_e                    pdu_session_type;
  security_ind_s                        security_ind;
  uint16_t                              network_instance;
  qos_flow_setup_request_list_l         qos_flow_setup_request_list;
  unbounded_octstring<true>             common_network_instance;
  direct_forwarding_path_availability_e direct_forwarding_path_availability;
  up_transport_layer_info_c             redundant_ul_ngu_up_tnl_info;
  up_transport_layer_info_list_l        add_redundant_ul_ngu_up_tnl_info;
  unbounded_octstring<true>             redundant_common_network_instance;
  redundant_pdu_session_info_s          redundant_pdu_session_info;
  mbs_session_setup_request_list_l      mbs_session_setup_request_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupRequestTransfer ::= SEQUENCE
using pdu_session_res_setup_request_transfer_s =
    elementary_procedure_option<pdu_session_res_setup_request_transfer_ies_container>;

// PDUSessionResourceSetupResponseTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct pdu_session_res_setup_resp_transfer_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        redundant_dl_qos_flow_per_tnl_info,
        add_redundant_dl_qos_flow_per_tnl_info,
        used_r_sn_info,
        global_ran_node_id,
        mbs_support_ind,
        mbs_session_setup_resp_list,
        mbs_session_failedto_setup_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_flow_per_tnl_info_s&                 redundant_dl_qos_flow_per_tnl_info();
    qos_flow_per_tnl_info_list_l&            add_redundant_dl_qos_flow_per_tnl_info();
    redundant_pdu_session_info_s&            used_r_sn_info();
    global_ran_node_id_c&                    global_ran_node_id();
    mbs_support_ind_e&                       mbs_support_ind();
    mbs_session_setup_resp_list_l&           mbs_session_setup_resp_list();
    mbs_session_failedto_setup_list_l&       mbs_session_failedto_setup_list();
    const qos_flow_per_tnl_info_s&           redundant_dl_qos_flow_per_tnl_info() const;
    const qos_flow_per_tnl_info_list_l&      add_redundant_dl_qos_flow_per_tnl_info() const;
    const redundant_pdu_session_info_s&      used_r_sn_info() const;
    const global_ran_node_id_c&              global_ran_node_id() const;
    const mbs_support_ind_e&                 mbs_support_ind() const;
    const mbs_session_setup_resp_list_l&     mbs_session_setup_resp_list() const;
    const mbs_session_failedto_setup_list_l& mbs_session_failedto_setup_list() const;

  private:
    types type_;
    choice_buffer_t<global_ran_node_id_c,
                    mbs_session_failedto_setup_list_l,
                    mbs_session_setup_resp_list_l,
                    qos_flow_per_tnl_info_list_l,
                    qos_flow_per_tnl_info_s,
                    redundant_pdu_session_info_s>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct pdu_session_res_setup_resp_transfer_ext_ies_container {
  bool                              redundant_dl_qos_flow_per_tnl_info_present     = false;
  bool                              add_redundant_dl_qos_flow_per_tnl_info_present = false;
  bool                              used_r_sn_info_present                         = false;
  bool                              global_ran_node_id_present                     = false;
  bool                              mbs_support_ind_present                        = false;
  bool                              mbs_session_setup_resp_list_present            = false;
  bool                              mbs_session_failedto_setup_list_present        = false;
  qos_flow_per_tnl_info_s           redundant_dl_qos_flow_per_tnl_info;
  qos_flow_per_tnl_info_list_l      add_redundant_dl_qos_flow_per_tnl_info;
  redundant_pdu_session_info_s      used_r_sn_info;
  global_ran_node_id_c              global_ran_node_id;
  mbs_support_ind_e                 mbs_support_ind;
  mbs_session_setup_resp_list_l     mbs_session_setup_resp_list;
  mbs_session_failedto_setup_list_l mbs_session_failedto_setup_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupResponseTransfer ::= SEQUENCE
struct pdu_session_res_setup_resp_transfer_s {
  bool                                                  ext                     = false;
  bool                                                  security_result_present = false;
  bool                                                  ie_exts_present         = false;
  qos_flow_per_tnl_info_s                               dl_qos_flow_per_tnl_info;
  qos_flow_per_tnl_info_list_l                          add_dl_qos_flow_per_tnl_info;
  security_result_s                                     security_result;
  qos_flow_list_with_cause_l                            qos_flow_failed_to_setup_list;
  pdu_session_res_setup_resp_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDUSessionResourceSetupUnsuccessfulTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using pdu_session_res_setup_unsuccessful_transfer_ext_ies_o = protocol_ext_empty_o;

using pdu_session_res_setup_unsuccessful_transfer_ext_ies_container = protocol_ext_container_empty_l;

// PDUSessionResourceSetupUnsuccessfulTransfer ::= SEQUENCE
struct pdu_session_res_setup_unsuccessful_transfer_s {
  bool                                                          ext                      = false;
  bool                                                          crit_diagnostics_present = false;
  bool                                                          ie_exts_present          = false;
  cause_c                                                       cause;
  crit_diagnostics_s                                            crit_diagnostics;
  pdu_session_res_setup_unsuccessful_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowParametersItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct qos_flow_params_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { cn_packet_delay_budget_dl, cn_packet_delay_budget_ul, burst_arrival_time_dl, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&                        cn_packet_delay_budget_dl();
    uint32_t&                        cn_packet_delay_budget_ul();
    unbounded_octstring<true>&       burst_arrival_time_dl();
    const uint32_t&                  cn_packet_delay_budget_dl() const;
    const uint32_t&                  cn_packet_delay_budget_ul() const;
    const unbounded_octstring<true>& burst_arrival_time_dl() const;

  private:
    types                                      type_;
    choice_buffer_t<unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct qos_flow_params_item_ext_ies_container {
  bool                      cn_packet_delay_budget_dl_present = false;
  bool                      cn_packet_delay_budget_ul_present = false;
  bool                      burst_arrival_time_dl_present     = false;
  uint32_t                  cn_packet_delay_budget_dl;
  uint32_t                  cn_packet_delay_budget_ul;
  unbounded_octstring<true> burst_arrival_time_dl;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowParametersItem ::= SEQUENCE
struct qos_flow_params_item_s {
  bool                                   ext             = false;
  bool                                   ie_exts_present = false;
  uint8_t                                qos_flow_id     = 0;
  alt_qos_para_set_list_l                alt_qos_para_set_list;
  qos_flow_params_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowParametersList ::= SEQUENCE (SIZE (1..64)) OF QosFlowParametersItem
using qos_flow_params_list_l = dyn_array<qos_flow_params_item_s>;

// PathSwitchRequestAcknowledgeTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct path_switch_request_ack_transfer_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        add_ngu_up_tnl_info,
        redundant_ul_ngu_up_tnl_info,
        add_redundant_ngu_up_tnl_info,
        qos_flow_params_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    up_transport_layer_info_pair_list_l&       add_ngu_up_tnl_info();
    up_transport_layer_info_c&                 redundant_ul_ngu_up_tnl_info();
    up_transport_layer_info_pair_list_l&       add_redundant_ngu_up_tnl_info();
    qos_flow_params_list_l&                    qos_flow_params_list();
    const up_transport_layer_info_pair_list_l& add_ngu_up_tnl_info() const;
    const up_transport_layer_info_c&           redundant_ul_ngu_up_tnl_info() const;
    const up_transport_layer_info_pair_list_l& add_redundant_ngu_up_tnl_info() const;
    const qos_flow_params_list_l&              qos_flow_params_list() const;

  private:
    types                                                                                                   type_;
    choice_buffer_t<qos_flow_params_list_l, up_transport_layer_info_c, up_transport_layer_info_pair_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct path_switch_request_ack_transfer_ext_ies_container {
  bool                                add_ngu_up_tnl_info_present           = false;
  bool                                redundant_ul_ngu_up_tnl_info_present  = false;
  bool                                add_redundant_ngu_up_tnl_info_present = false;
  bool                                qos_flow_params_list_present          = false;
  up_transport_layer_info_pair_list_l add_ngu_up_tnl_info;
  up_transport_layer_info_c           redundant_ul_ngu_up_tnl_info;
  up_transport_layer_info_pair_list_l add_redundant_ngu_up_tnl_info;
  qos_flow_params_list_l              qos_flow_params_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequestAcknowledgeTransfer ::= SEQUENCE
struct path_switch_request_ack_transfer_s {
  bool                                               ext                        = false;
  bool                                               ul_ngu_up_tnl_info_present = false;
  bool                                               security_ind_present       = false;
  bool                                               ie_exts_present            = false;
  up_transport_layer_info_c                          ul_ngu_up_tnl_info;
  security_ind_s                                     security_ind;
  path_switch_request_ack_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequestSetupFailedTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using path_switch_request_setup_failed_transfer_ext_ies_o = protocol_ext_empty_o;

using path_switch_request_setup_failed_transfer_ext_ies_container = protocol_ext_container_empty_l;

// PathSwitchRequestSetupFailedTransfer ::= SEQUENCE
struct path_switch_request_setup_failed_transfer_s {
  bool                                                        ext             = false;
  bool                                                        ie_exts_present = false;
  cause_c                                                     cause;
  path_switch_request_setup_failed_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QosFlowAcceptedItem-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct qos_flow_accepted_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { current_qos_para_set_idx, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::current_qos_para_set_idx; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&       current_qos_para_set_idx() { return c; }
    const uint8_t& current_qos_para_set_idx() const { return c; }

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

// DL-NGU-TNLInformationReused ::= ENUMERATED
struct dl_ngu_tnl_info_reused_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using dl_ngu_tnl_info_reused_e = enumerated<dl_ngu_tnl_info_reused_opts, true>;

// QosFlowAcceptedItem ::= SEQUENCE
struct qos_flow_accepted_item_s {
  bool                                                       ext         = false;
  uint8_t                                                    qos_flow_id = 0;
  protocol_ext_container_l<qos_flow_accepted_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UserPlaneSecurityInformation-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using user_plane_security_info_ext_ies_o = protocol_ext_empty_o;

// PathSwitchRequestTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct path_switch_request_transfer_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        add_dl_qos_flow_per_tnl_info,
        redundant_dl_ngu_up_tnl_info,
        redundant_dl_ngu_tnl_info_reused,
        add_redundant_dl_qos_flow_per_tnl_info,
        used_r_sn_info,
        global_ran_node_id,
        mbs_support_ind,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    qos_flow_per_tnl_info_list_l&       add_dl_qos_flow_per_tnl_info();
    up_transport_layer_info_c&          redundant_dl_ngu_up_tnl_info();
    dl_ngu_tnl_info_reused_e&           redundant_dl_ngu_tnl_info_reused();
    qos_flow_per_tnl_info_list_l&       add_redundant_dl_qos_flow_per_tnl_info();
    redundant_pdu_session_info_s&       used_r_sn_info();
    global_ran_node_id_c&               global_ran_node_id();
    mbs_support_ind_e&                  mbs_support_ind();
    const qos_flow_per_tnl_info_list_l& add_dl_qos_flow_per_tnl_info() const;
    const up_transport_layer_info_c&    redundant_dl_ngu_up_tnl_info() const;
    const dl_ngu_tnl_info_reused_e&     redundant_dl_ngu_tnl_info_reused() const;
    const qos_flow_per_tnl_info_list_l& add_redundant_dl_qos_flow_per_tnl_info() const;
    const redundant_pdu_session_info_s& used_r_sn_info() const;
    const global_ran_node_id_c&         global_ran_node_id() const;
    const mbs_support_ind_e&            mbs_support_ind() const;

  private:
    types type_;
    choice_buffer_t<global_ran_node_id_c,
                    qos_flow_per_tnl_info_list_l,
                    redundant_pdu_session_info_s,
                    up_transport_layer_info_c>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// QosFlowAcceptedList ::= SEQUENCE (SIZE (1..64)) OF QosFlowAcceptedItem
using qos_flow_accepted_list_l = dyn_array<qos_flow_accepted_item_s>;

using user_plane_security_info_ext_ies_container = protocol_ext_container_empty_l;

// UserPlaneSecurityInformation ::= SEQUENCE
struct user_plane_security_info_s {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  security_result_s                          security_result;
  security_ind_s                             security_ind;
  user_plane_security_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct path_switch_request_transfer_ext_ies_container {
  bool                         add_dl_qos_flow_per_tnl_info_present           = false;
  bool                         redundant_dl_ngu_up_tnl_info_present           = false;
  bool                         redundant_dl_ngu_tnl_info_reused_present       = false;
  bool                         add_redundant_dl_qos_flow_per_tnl_info_present = false;
  bool                         used_r_sn_info_present                         = false;
  bool                         global_ran_node_id_present                     = false;
  bool                         mbs_support_ind_present                        = false;
  qos_flow_per_tnl_info_list_l add_dl_qos_flow_per_tnl_info;
  up_transport_layer_info_c    redundant_dl_ngu_up_tnl_info;
  dl_ngu_tnl_info_reused_e     redundant_dl_ngu_tnl_info_reused;
  qos_flow_per_tnl_info_list_l add_redundant_dl_qos_flow_per_tnl_info;
  redundant_pdu_session_info_s used_r_sn_info;
  global_ran_node_id_c         global_ran_node_id;
  mbs_support_ind_e            mbs_support_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequestTransfer ::= SEQUENCE
struct path_switch_request_transfer_s {
  bool                                           ext                              = false;
  bool                                           dl_ngu_tnl_info_reused_present   = false;
  bool                                           user_plane_security_info_present = false;
  bool                                           ie_exts_present                  = false;
  up_transport_layer_info_c                      dl_ngu_up_tnl_info;
  dl_ngu_tnl_info_reused_e                       dl_ngu_tnl_info_reused;
  user_plane_security_info_s                     user_plane_security_info;
  qos_flow_accepted_list_l                       qos_flow_accepted_list;
  path_switch_request_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PathSwitchRequestUnsuccessfulTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using path_switch_request_unsuccessful_transfer_ext_ies_o = protocol_ext_empty_o;

using path_switch_request_unsuccessful_transfer_ext_ies_container = protocol_ext_container_empty_l;

// PathSwitchRequestUnsuccessfulTransfer ::= SEQUENCE
struct path_switch_request_unsuccessful_transfer_s {
  bool                                                        ext             = false;
  bool                                                        ie_exts_present = false;
  cause_c                                                     cause;
  path_switch_request_unsuccessful_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoSFlowList ::= SEQUENCE (SIZE (1..64)) OF INTEGER (0..63,...)
using qos_flow_list_l = dyn_array<uint8_t>;

// SecondaryRATDataUsageReportTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using secondary_rat_data_usage_report_transfer_ext_ies_o = protocol_ext_empty_o;

using secondary_rat_data_usage_report_transfer_ext_ies_container = protocol_ext_container_empty_l;

// SecondaryRATDataUsageReportTransfer ::= SEQUENCE
struct secondary_rat_data_usage_report_transfer_s {
  bool                                                       ext                              = false;
  bool                                                       secondary_rat_usage_info_present = false;
  bool                                                       ie_exts_present                  = false;
  secondary_rat_usage_info_s                                 secondary_rat_usage_info;
  secondary_rat_data_usage_report_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SourceNodeID-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using source_node_id_ext_ies_o = protocol_ies_empty_o;

// UEHistoryInformationFromTheUE-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-IES
using ue_history_info_from_the_ue_ext_ies_o = protocol_ies_empty_o;

// SourceNodeID ::= CHOICE
struct source_node_id_c {
  struct types_opts {
    enum options { sourceen_gnb_id, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  source_node_id_c() = default;
  source_node_id_c(const source_node_id_c& other);
  source_node_id_c& operator=(const source_node_id_c& other);
  ~source_node_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  global_gnb_id_s& sourceen_gnb_id()
  {
    assert_choice_type(types::sourceen_gnb_id, type_, "SourceNodeID");
    return c.get<global_gnb_id_s>();
  }
  protocol_ie_single_container_s<source_node_id_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "SourceNodeID");
    return c.get<protocol_ie_single_container_s<source_node_id_ext_ies_o>>();
  }
  const global_gnb_id_s& sourceen_gnb_id() const
  {
    assert_choice_type(types::sourceen_gnb_id, type_, "SourceNodeID");
    return c.get<global_gnb_id_s>();
  }
  const protocol_ie_single_container_s<source_node_id_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "SourceNodeID");
    return c.get<protocol_ie_single_container_s<source_node_id_ext_ies_o>>();
  }
  global_gnb_id_s&                                          set_sourceen_gnb_id();
  protocol_ie_single_container_s<source_node_id_ext_ies_o>& set_choice_exts();

private:
  types                                                                                      type_;
  choice_buffer_t<global_gnb_id_s, protocol_ie_single_container_s<source_node_id_ext_ies_o>> c;

  void destroy_();
};

// UEHistoryInformationFromTheUE ::= CHOICE
struct ue_history_info_from_the_ue_c {
  struct types_opts {
    enum options { nr, choice_exts, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ue_history_info_from_the_ue_c() = default;
  ue_history_info_from_the_ue_c(const ue_history_info_from_the_ue_c& other);
  ue_history_info_from_the_ue_c& operator=(const ue_history_info_from_the_ue_c& other);
  ~ue_history_info_from_the_ue_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  unbounded_octstring<true>& nr()
  {
    assert_choice_type(types::nr, type_, "UEHistoryInformationFromTheUE");
    return c.get<unbounded_octstring<true>>();
  }
  protocol_ie_single_container_s<ue_history_info_from_the_ue_ext_ies_o>& choice_exts()
  {
    assert_choice_type(types::choice_exts, type_, "UEHistoryInformationFromTheUE");
    return c.get<protocol_ie_single_container_s<ue_history_info_from_the_ue_ext_ies_o>>();
  }
  const unbounded_octstring<true>& nr() const
  {
    assert_choice_type(types::nr, type_, "UEHistoryInformationFromTheUE");
    return c.get<unbounded_octstring<true>>();
  }
  const protocol_ie_single_container_s<ue_history_info_from_the_ue_ext_ies_o>& choice_exts() const
  {
    assert_choice_type(types::choice_exts, type_, "UEHistoryInformationFromTheUE");
    return c.get<protocol_ie_single_container_s<ue_history_info_from_the_ue_ext_ies_o>>();
  }
  unbounded_octstring<true>&                                             set_nr();
  protocol_ie_single_container_s<ue_history_info_from_the_ue_ext_ies_o>& set_choice_exts();

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<ue_history_info_from_the_ue_ext_ies_o>, unbounded_octstring<true>> c;

  void destroy_();
};

// SourceNGRANNode-ToTargetNGRANNode-TransparentContainer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct source_ngran_node_to_target_ngran_node_transparent_container_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        s_gnb_ue_x2ap_id,
        ue_history_info_from_the_ue,
        source_node_id,
        ue_context_ref_at_source,
        mbs_active_session_info_sourceto_target_list,
        q_mcc_onfig_info,
        ngap_ie_support_info_request_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint64_t&                                             s_gnb_ue_x2ap_id();
    ue_history_info_from_the_ue_c&                        ue_history_info_from_the_ue();
    source_node_id_c&                                     source_node_id();
    uint64_t&                                             ue_context_ref_at_source();
    mbs_active_session_info_sourceto_target_list_l&       mbs_active_session_info_sourceto_target_list();
    q_mcc_onfig_info_s&                                   q_mcc_onfig_info();
    ngap_ie_support_info_request_list_l&                  ngap_ie_support_info_request_list();
    const uint64_t&                                       s_gnb_ue_x2ap_id() const;
    const ue_history_info_from_the_ue_c&                  ue_history_info_from_the_ue() const;
    const source_node_id_c&                               source_node_id() const;
    const uint64_t&                                       ue_context_ref_at_source() const;
    const mbs_active_session_info_sourceto_target_list_l& mbs_active_session_info_sourceto_target_list() const;
    const q_mcc_onfig_info_s&                             q_mcc_onfig_info() const;
    const ngap_ie_support_info_request_list_l&            ngap_ie_support_info_request_list() const;

  private:
    types type_;
    choice_buffer_t<mbs_active_session_info_sourceto_target_list_l,
                    ngap_ie_support_info_request_list_l,
                    q_mcc_onfig_info_s,
                    source_node_id_c,
                    ue_history_info_from_the_ue_c>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEHistoryInformation ::= SEQUENCE (SIZE (1..16)) OF LastVisitedCellItem
using ue_history_info_l = dyn_array<last_visited_cell_item_s>;

struct source_ngran_node_to_target_ngran_node_transparent_container_ext_ies_container {
  bool                                           s_gnb_ue_x2ap_id_present                             = false;
  bool                                           ue_history_info_from_the_ue_present                  = false;
  bool                                           source_node_id_present                               = false;
  bool                                           ue_context_ref_at_source_present                     = false;
  bool                                           mbs_active_session_info_sourceto_target_list_present = false;
  bool                                           q_mcc_onfig_info_present                             = false;
  bool                                           ngap_ie_support_info_request_list_present            = false;
  uint64_t                                       s_gnb_ue_x2ap_id;
  ue_history_info_from_the_ue_c                  ue_history_info_from_the_ue;
  source_node_id_c                               source_node_id;
  uint64_t                                       ue_context_ref_at_source;
  mbs_active_session_info_sourceto_target_list_l mbs_active_session_info_sourceto_target_list;
  q_mcc_onfig_info_s                             q_mcc_onfig_info;
  ngap_ie_support_info_request_list_l            ngap_ie_support_info_request_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SourceNGRANNode-ToTargetNGRANNode-TransparentContainer ::= SEQUENCE
struct source_ngran_node_to_target_ngran_node_transparent_container_s {
  bool                                                                           ext                 = false;
  bool                                                                           idx_to_rfsp_present = false;
  bool                                                                           ie_exts_present     = false;
  unbounded_octstring<true>                                                      rrc_container;
  pdu_session_res_info_list_l                                                    pdu_session_res_info_list;
  erab_info_list_l                                                               erab_info_list;
  ngran_cgi_c                                                                    target_cell_id;
  uint16_t                                                                       idx_to_rfsp = 1;
  ue_history_info_l                                                              ue_history_info;
  source_ngran_node_to_target_ngran_node_transparent_container_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TargetNGRANNode-ToSourceNGRANNode-FailureTransparentContainer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct target_ngran_node_to_source_ngran_node_fail_transparent_container_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { ngap_ie_support_info_resp_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ngap_ie_support_info_resp_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ngap_ie_support_info_resp_list_l&       ngap_ie_support_info_resp_list() { return c; }
    const ngap_ie_support_info_resp_list_l& ngap_ie_support_info_resp_list() const { return c; }

  private:
    ngap_ie_support_info_resp_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TargetNGRANNode-ToSourceNGRANNode-FailureTransparentContainer ::= SEQUENCE
struct target_ngran_node_to_source_ngran_node_fail_transparent_container_s {
  bool            ext                   = false;
  bool            cell_cag_info_present = false;
  cell_cag_info_s cell_cag_info;
  protocol_ext_container_l<target_ngran_node_to_source_ngran_node_fail_transparent_container_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TargetNGRANNode-ToSourceNGRANNode-TransparentContainer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
struct target_ngran_node_to_source_ngran_node_transparent_container_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options {
        daps_resp_info_list,
        direct_forwarding_path_availability,
        mbs_active_session_info_targetto_source_list,
        ngap_ie_support_info_resp_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    ext_c(const ext_c& other);
    ext_c& operator=(const ext_c& other);
    ~ext_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    daps_resp_info_list_l&                                daps_resp_info_list();
    direct_forwarding_path_availability_e&                direct_forwarding_path_availability();
    mbs_active_session_info_targetto_source_list_l&       mbs_active_session_info_targetto_source_list();
    ngap_ie_support_info_resp_list_l&                     ngap_ie_support_info_resp_list();
    const daps_resp_info_list_l&                          daps_resp_info_list() const;
    const direct_forwarding_path_availability_e&          direct_forwarding_path_availability() const;
    const mbs_active_session_info_targetto_source_list_l& mbs_active_session_info_targetto_source_list() const;
    const ngap_ie_support_info_resp_list_l&               ngap_ie_support_info_resp_list() const;

  private:
    types type_;
    choice_buffer_t<daps_resp_info_list_l,
                    mbs_active_session_info_targetto_source_list_l,
                    ngap_ie_support_info_resp_list_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct target_ngran_node_to_source_ngran_node_transparent_container_ext_ies_container {
  bool                                           daps_resp_info_list_present                          = false;
  bool                                           direct_forwarding_path_availability_present          = false;
  bool                                           mbs_active_session_info_targetto_source_list_present = false;
  bool                                           ngap_ie_support_info_resp_list_present               = false;
  daps_resp_info_list_l                          daps_resp_info_list;
  direct_forwarding_path_availability_e          direct_forwarding_path_availability;
  mbs_active_session_info_targetto_source_list_l mbs_active_session_info_targetto_source_list;
  ngap_ie_support_info_resp_list_l               ngap_ie_support_info_resp_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TargetNGRANNode-ToSourceNGRANNode-TransparentContainer ::= SEQUENCE
struct target_ngran_node_to_source_ngran_node_transparent_container_s {
  bool                                                                           ext             = false;
  bool                                                                           ie_exts_present = false;
  unbounded_octstring<true>                                                      rrc_container;
  target_ngran_node_to_source_ngran_node_transparent_container_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextResumeRequestTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ue_context_resume_request_transfer_ext_ies_o = protocol_ext_empty_o;

using ue_context_resume_request_transfer_ext_ies_container = protocol_ext_container_empty_l;

// UEContextResumeRequestTransfer ::= SEQUENCE
struct ue_context_resume_request_transfer_s {
  bool                                                 ext             = false;
  bool                                                 ie_exts_present = false;
  qos_flow_list_with_cause_l                           qos_flow_failed_to_resume_list;
  ue_context_resume_request_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextResumeResponseTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ue_context_resume_resp_transfer_ext_ies_o = protocol_ext_empty_o;

using ue_context_resume_resp_transfer_ext_ies_container = protocol_ext_container_empty_l;

// UEContextResumeResponseTransfer ::= SEQUENCE
struct ue_context_resume_resp_transfer_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  qos_flow_list_with_cause_l                        qos_flow_failed_to_resume_list;
  ue_context_resume_resp_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SuspendIndicator ::= ENUMERATED
struct suspend_ind_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using suspend_ind_e = enumerated<suspend_ind_opts, true>;

// UEContextSuspendRequestTransfer-ExtIEs ::= OBJECT SET OF NGAP-PROTOCOL-EXTENSION
using ue_context_suspend_request_transfer_ext_ies_o = protocol_ext_empty_o;

using ue_context_suspend_request_transfer_ext_ies_container = protocol_ext_container_empty_l;

// UEContextSuspendRequestTransfer ::= SEQUENCE
struct ue_context_suspend_request_transfer_s {
  bool                                                  ext                 = false;
  bool                                                  suspend_ind_present = false;
  bool                                                  ie_exts_present     = false;
  suspend_ind_e                                         suspend_ind;
  ue_context_suspend_request_transfer_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace ngap
} // namespace asn1

extern template struct asn1::protocol_ie_single_container_s<asn1::ngap::cp_transport_layer_info_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::ngap::global_ran_node_id_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::ngap::son_info_report_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::ngap::intersys_son_info_report_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::ngap::intersys_son_info_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::ngap::son_info_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::ngap::user_location_info_w_agf_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::ngap::user_location_info_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::ngap::target_id_ext_ies_o>;

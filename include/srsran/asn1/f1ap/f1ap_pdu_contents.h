/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/*******************************************************************************
 *
 *                     3GPP TS ASN1 F1AP v17.4.1 (2023-04)
 *
 ******************************************************************************/

#pragma once

#include "f1ap_ies.h"

namespace asn1 {
namespace f1ap {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// AccessAndMobilityIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct access_and_mob_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        rach_report_info_list,
        rlf_report_info_list,
        successful_ho_report_info_list,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                               transaction_id();
    rach_report_info_list_l&                rach_report_info_list();
    rlf_report_info_list_l&                 rlf_report_info_list();
    successful_ho_report_info_list_l&       successful_ho_report_info_list();
    const uint16_t&                         transaction_id() const;
    const rach_report_info_list_l&          rach_report_info_list() const;
    const rlf_report_info_list_l&           rlf_report_info_list() const;
    const successful_ho_report_info_list_l& successful_ho_report_info_list() const;

  private:
    types                                                                                              type_;
    choice_buffer_t<rach_report_info_list_l, rlf_report_info_list_l, successful_ho_report_info_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct access_and_mob_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                                  rach_report_info_list_present          = false;
  bool                                                                  rlf_report_info_list_present           = false;
  bool                                                                  successful_ho_report_info_list_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>                     transaction_id;
  ie_field_s<dyn_seq_of<rach_report_info_item_s, 1, 64, true>>          rach_report_info_list;
  ie_field_s<dyn_seq_of<rlf_report_info_item_s, 1, 64, true>>           rlf_report_info_list;
  ie_field_s<dyn_seq_of<successful_ho_report_info_item_s, 1, 64, true>> successful_ho_report_info_list;

  // sequence methods
  access_and_mob_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AccessAndMobilityIndication ::= SEQUENCE
using access_and_mob_ind_s = elementary_procedure_option<access_and_mob_ind_ies_container>;

// AccessSuccessIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct access_success_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, nr_cgi, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&       gnb_cu_ue_f1ap_id();
    uint64_t&       gnb_du_ue_f1ap_id();
    nr_cgi_s&       nr_cgi();
    const uint64_t& gnb_cu_ue_f1ap_id() const;
    const uint64_t& gnb_du_ue_f1ap_id() const;
    const nr_cgi_s& nr_cgi() const;

  private:
    types                     type_;
    choice_buffer_t<nr_cgi_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct access_success_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<nr_cgi_s>                                      nr_cgi;

  // sequence methods
  access_success_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AccessSuccess ::= SEQUENCE
using access_success_s = elementary_procedure_option<access_success_ies_container>;

// AperiodicSRS-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
using aperiodic_srs_ext_ies_o = protocol_ext_empty_o;

using aperiodic_srs_ext_ies_container = protocol_ext_container_empty_l;

// AperiodicSRS ::= SEQUENCE
struct aperiodic_srs_s {
  struct aperiodic_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<aperiodic_opts, true> aperiodic_e_;

  // member variables
  bool                            ext                     = false;
  bool                            srs_res_trigger_present = false;
  bool                            ie_exts_present         = false;
  aperiodic_e_                    aperiodic;
  srs_res_trigger_s               srs_res_trigger;
  aperiodic_srs_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Associated-SCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct associated_scell_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { associated_scell_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::associated_scell_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    associated_scell_item_s&       associated_scell_item() { return c; }
    const associated_scell_item_s& associated_scell_item() const { return c; }

  private:
    associated_scell_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Associated-SCell-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using associated_scell_list_l = bounded_array<protocol_ie_single_container_s<associated_scell_item_ies_o>, 32>;

// BAP-Header-Rewriting-Added-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bap_hdr_rewriting_added_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bap_hdr_rewriting_added_list_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::bap_hdr_rewriting_added_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bap_hdr_rewriting_added_list_item_s&       bap_hdr_rewriting_added_list_item() { return c; }
    const bap_hdr_rewriting_added_list_item_s& bap_hdr_rewriting_added_list_item() const { return c; }

  private:
    bap_hdr_rewriting_added_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BAP-Header-Rewriting-Added-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bap_hdr_rewriting_added_list_l =
    dyn_array<protocol_ie_single_container_s<bap_hdr_rewriting_added_list_item_ies_o>>;

// BAP-Header-Rewriting-Removed-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bap_hdr_rewriting_remd_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bap_hdr_rewriting_remd_list_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::bap_hdr_rewriting_remd_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bap_hdr_rewriting_remd_list_item_s&       bap_hdr_rewriting_remd_list_item() { return c; }
    const bap_hdr_rewriting_remd_list_item_s& bap_hdr_rewriting_remd_list_item() const { return c; }

  private:
    bap_hdr_rewriting_remd_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BAP-Header-Rewriting-Removed-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bap_hdr_rewriting_remd_list_l = dyn_array<protocol_ie_single_container_s<bap_hdr_rewriting_remd_list_item_ies_o>>;

// BH-Routing-Information-Added-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_routing_info_added_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_routing_info_added_list_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::bh_routing_info_added_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_routing_info_added_list_item_s&       bh_routing_info_added_list_item() { return c; }
    const bh_routing_info_added_list_item_s& bh_routing_info_added_list_item() const { return c; }

  private:
    bh_routing_info_added_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BH-Routing-Information-Removed-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_routing_info_remd_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_routing_info_remd_list_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::bh_routing_info_remd_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_routing_info_remd_list_item_s&       bh_routing_info_remd_list_item() { return c; }
    const bh_routing_info_remd_list_item_s& bh_routing_info_remd_list_item() const { return c; }

  private:
    bh_routing_info_remd_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BH-Routing-Information-Added-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_routing_info_added_list_l = dyn_array<protocol_ie_single_container_s<bh_routing_info_added_list_item_ies_o>>;

// BH-Routing-Information-Removed-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_routing_info_remd_list_l = dyn_array<protocol_ie_single_container_s<bh_routing_info_remd_list_item_ies_o>>;

// BAPMappingConfiguration-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bap_map_cfg_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        bh_routing_info_added_list,
        bh_routing_info_remd_list,
        traffic_map_info,
        buffer_size_thresh,
        bap_hdr_rewriting_added_list,
        re_routing_enable_ind,
        bap_hdr_rewriting_remd_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                             transaction_id();
    bh_routing_info_added_list_l&         bh_routing_info_added_list();
    bh_routing_info_remd_list_l&          bh_routing_info_remd_list();
    traffic_map_info_c&                   traffic_map_info();
    uint32_t&                             buffer_size_thresh();
    bap_hdr_rewriting_added_list_l&       bap_hdr_rewriting_added_list();
    re_routing_enable_ind_e&              re_routing_enable_ind();
    bap_hdr_rewriting_remd_list_l&        bap_hdr_rewriting_remd_list();
    const uint16_t&                       transaction_id() const;
    const bh_routing_info_added_list_l&   bh_routing_info_added_list() const;
    const bh_routing_info_remd_list_l&    bh_routing_info_remd_list() const;
    const traffic_map_info_c&             traffic_map_info() const;
    const uint32_t&                       buffer_size_thresh() const;
    const bap_hdr_rewriting_added_list_l& bap_hdr_rewriting_added_list() const;
    const re_routing_enable_ind_e&        re_routing_enable_ind() const;
    const bap_hdr_rewriting_remd_list_l&  bap_hdr_rewriting_remd_list() const;

  private:
    types type_;
    choice_buffer_t<bap_hdr_rewriting_added_list_l,
                    bap_hdr_rewriting_remd_list_l,
                    bh_routing_info_added_list_l,
                    bh_routing_info_remd_list_l,
                    traffic_map_info_c>
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

struct bap_map_cfg_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              bh_routing_info_added_list_present   = false;
  bool                                              bh_routing_info_remd_list_present    = false;
  bool                                              traffic_map_info_present             = false;
  bool                                              buffer_size_thresh_present           = false;
  bool                                              bap_hdr_rewriting_added_list_present = false;
  bool                                              re_routing_enable_ind_present        = false;
  bool                                              bap_hdr_rewriting_remd_list_present  = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<bh_routing_info_added_list_item_ies_o>, 1, 1024, true>>
      bh_routing_info_added_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<bh_routing_info_remd_list_item_ies_o>, 1, 1024, true>>
                                                          bh_routing_info_remd_list;
  ie_field_s<traffic_map_info_c>                          traffic_map_info;
  ie_field_s<integer<uint32_t, 0, 16777215, false, true>> buffer_size_thresh;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<bap_hdr_rewriting_added_list_item_ies_o>, 1, 1024, true>>
                                      bap_hdr_rewriting_added_list;
  ie_field_s<re_routing_enable_ind_e> re_routing_enable_ind;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<bap_hdr_rewriting_remd_list_item_ies_o>, 1, 1024, true>>
      bap_hdr_rewriting_remd_list;

  // sequence methods
  bap_map_cfg_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BAPMappingConfiguration ::= SEQUENCE
using bap_map_cfg_s = elementary_procedure_option<bap_map_cfg_ies_container>;

// BAPMappingConfigurationAcknowledge-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bap_map_cfg_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                               type_;
    choice_buffer_t<crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct bap_map_cfg_ack_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  bap_map_cfg_ack_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BAPMappingConfigurationAcknowledge ::= SEQUENCE
using bap_map_cfg_ack_s = elementary_procedure_option<bap_map_cfg_ack_ies_container>;

// BAPMappingConfigurationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bap_map_cfg_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct bap_map_cfg_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              time_to_wait_present     = false;
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cause_c>                               cause;
  ie_field_s<time_to_wait_e>                        time_to_wait;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  bap_map_cfg_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BAPMappingConfigurationFailure ::= SEQUENCE
using bap_map_cfg_fail_s = elementary_procedure_option<bap_map_cfg_fail_ies_container>;

// BHChannels-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_failed_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_failed_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::bh_chs_failed_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_failed_to_be_modified_item_s&       bh_chs_failed_to_be_modified_item() { return c; }
    const bh_chs_failed_to_be_modified_item_s& bh_chs_failed_to_be_modified_item() const { return c; }

  private:
    bh_chs_failed_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-FailedToBeModified-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_failed_to_be_modified_list_l =
    dyn_array<protocol_ie_single_container_s<bh_chs_failed_to_be_modified_item_ies_o>>;

// BHChannels-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_failed_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_failed_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::bh_chs_failed_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_failed_to_be_setup_item_s&       bh_chs_failed_to_be_setup_item() { return c; }
    const bh_chs_failed_to_be_setup_item_s& bh_chs_failed_to_be_setup_item() const { return c; }

  private:
    bh_chs_failed_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-FailedToBeSetup-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_failed_to_be_setup_list_l =
    dyn_array<protocol_ie_single_container_s<bh_chs_failed_to_be_setup_item_ies_o>>;

// BHChannels-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_failed_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_failed_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::bh_chs_failed_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_failed_to_be_setup_mod_item_s&       bh_chs_failed_to_be_setup_mod_item() { return c; }
    const bh_chs_failed_to_be_setup_mod_item_s& bh_chs_failed_to_be_setup_mod_item() const { return c; }

  private:
    bh_chs_failed_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-FailedToBeSetupMod-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_failed_to_be_setup_mod_list_l =
    dyn_array<protocol_ie_single_container_s<bh_chs_failed_to_be_setup_mod_item_ies_o>>;

// BHChannels-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::bh_chs_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_modified_item_s&       bh_chs_modified_item() { return c; }
    const bh_chs_modified_item_s& bh_chs_modified_item() const { return c; }

  private:
    bh_chs_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-Modified-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_modified_list_l = dyn_array<protocol_ie_single_container_s<bh_chs_modified_item_ies_o>>;

// BHChannels-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_required_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_required_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::bh_chs_required_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_required_to_be_released_item_s&       bh_chs_required_to_be_released_item() { return c; }
    const bh_chs_required_to_be_released_item_s& bh_chs_required_to_be_released_item() const { return c; }

  private:
    bh_chs_required_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-Required-ToBeReleased-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES
// : IEsSetParam}
using bh_chs_required_to_be_released_list_l =
    dyn_array<protocol_ie_single_container_s<bh_chs_required_to_be_released_item_ies_o>>;

// BHChannels-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::bh_chs_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_setup_item_s&       bh_chs_setup_item() { return c; }
    const bh_chs_setup_item_s& bh_chs_setup_item() const { return c; }

  private:
    bh_chs_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-Setup-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using bh_chs_setup_list_l = dyn_array<protocol_ie_single_container_s<bh_chs_setup_item_ies_o>>;

// BHChannels-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::bh_chs_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_setup_mod_item_s&       bh_chs_setup_mod_item() { return c; }
    const bh_chs_setup_mod_item_s& bh_chs_setup_mod_item() const { return c; }

  private:
    bh_chs_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-SetupMod-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_setup_mod_list_l = dyn_array<protocol_ie_single_container_s<bh_chs_setup_mod_item_ies_o>>;

// BHChannels-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::bh_chs_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_to_be_modified_item_s&       bh_chs_to_be_modified_item() { return c; }
    const bh_chs_to_be_modified_item_s& bh_chs_to_be_modified_item() const { return c; }

  private:
    bh_chs_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-ToBeModified-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_to_be_modified_list_l = dyn_array<protocol_ie_single_container_s<bh_chs_to_be_modified_item_ies_o>>;

// BHChannels-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::bh_chs_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_to_be_released_item_s&       bh_chs_to_be_released_item() { return c; }
    const bh_chs_to_be_released_item_s& bh_chs_to_be_released_item() const { return c; }

  private:
    bh_chs_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-ToBeReleased-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_to_be_released_list_l = dyn_array<protocol_ie_single_container_s<bh_chs_to_be_released_item_ies_o>>;

// BHChannels-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::bh_chs_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_to_be_setup_item_s&       bh_chs_to_be_setup_item() { return c; }
    const bh_chs_to_be_setup_item_s& bh_chs_to_be_setup_item() const { return c; }

  private:
    bh_chs_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-ToBeSetup-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_to_be_setup_list_l = dyn_array<protocol_ie_single_container_s<bh_chs_to_be_setup_item_ies_o>>;

// BHChannels-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::bh_chs_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_to_be_setup_mod_item_s&       bh_chs_to_be_setup_mod_item() { return c; }
    const bh_chs_to_be_setup_mod_item_s& bh_chs_to_be_setup_mod_item() const { return c; }

  private:
    bh_chs_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_to_be_setup_mod_list_l = dyn_array<protocol_ie_single_container_s<bh_chs_to_be_setup_mod_item_ies_o>>;

// Broadcast-To-Be-Cancelled-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_to_be_cancelled_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_to_be_cancelled_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::broadcast_to_be_cancelled_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_to_be_cancelled_item_s&       broadcast_to_be_cancelled_item() { return c; }
    const broadcast_to_be_cancelled_item_s& broadcast_to_be_cancelled_item() const { return c; }

  private:
    broadcast_to_be_cancelled_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Broadcast-To-Be-Cancelled-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_to_be_cancelled_list_l =
    dyn_array<protocol_ie_single_container_s<broadcast_to_be_cancelled_list_item_ies_o>>;

// BroadcastContextModificationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_mod_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_mbs_f1ap_id, gnb_du_mbs_f1ap_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_mbs_f1ap_id();
    uint64_t&                 gnb_du_mbs_f1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_mbs_f1ap_id() const;
    const uint64_t&           gnb_du_mbs_f1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_context_mod_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  broadcast_context_mod_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastContextModificationFailure ::= SEQUENCE
using broadcast_context_mod_fail_s = elementary_procedure_option<broadcast_context_mod_fail_ies_container>;

// BroadcastMRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_to_be_modified_item_s&       broadcast_m_rbs_to_be_modified_item() { return c; }
    const broadcast_m_rbs_to_be_modified_item_s& broadcast_m_rbs_to_be_modified_item() const { return c; }

  private:
    broadcast_m_rbs_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_to_be_released_item_s&       broadcast_m_rbs_to_be_released_item() { return c; }
    const broadcast_m_rbs_to_be_released_item_s& broadcast_m_rbs_to_be_released_item() const { return c; }

  private:
    broadcast_m_rbs_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_to_be_setup_mod_item_s&       broadcast_m_rbs_to_be_setup_mod_item() { return c; }
    const broadcast_m_rbs_to_be_setup_mod_item_s& broadcast_m_rbs_to_be_setup_mod_item() const { return c; }

  private:
    broadcast_m_rbs_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-ToBeModified-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_to_be_modified_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_to_be_modified_item_ies_o>, 32>;

// BroadcastMRBs-ToBeReleased-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_to_be_released_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_to_be_released_item_ies_o>, 32>;

// BroadcastMRBs-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_to_be_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_to_be_setup_mod_item_ies_o>, 32>;

// BroadcastContextModificationRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_mod_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_mbs_f1ap_id,
        gnb_du_mbs_f1ap_id,
        mbs_service_area,
        mbs_cu_to_du_rrc_info,
        broadcast_m_rbs_to_be_setup_mod_list,
        broadcast_m_rbs_to_be_modified_list,
        broadcast_m_rbs_to_be_released_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                     gnb_cu_mbs_f1ap_id();
    uint64_t&                                     gnb_du_mbs_f1ap_id();
    mbs_service_area_c&                           mbs_service_area();
    mbs_cu_to_du_rrc_info_s&                      mbs_cu_to_du_rrc_info();
    broadcast_m_rbs_to_be_setup_mod_list_l&       broadcast_m_rbs_to_be_setup_mod_list();
    broadcast_m_rbs_to_be_modified_list_l&        broadcast_m_rbs_to_be_modified_list();
    broadcast_m_rbs_to_be_released_list_l&        broadcast_m_rbs_to_be_released_list();
    const uint64_t&                               gnb_cu_mbs_f1ap_id() const;
    const uint64_t&                               gnb_du_mbs_f1ap_id() const;
    const mbs_service_area_c&                     mbs_service_area() const;
    const mbs_cu_to_du_rrc_info_s&                mbs_cu_to_du_rrc_info() const;
    const broadcast_m_rbs_to_be_setup_mod_list_l& broadcast_m_rbs_to_be_setup_mod_list() const;
    const broadcast_m_rbs_to_be_modified_list_l&  broadcast_m_rbs_to_be_modified_list() const;
    const broadcast_m_rbs_to_be_released_list_l&  broadcast_m_rbs_to_be_released_list() const;

  private:
    types type_;
    choice_buffer_t<broadcast_m_rbs_to_be_modified_list_l,
                    broadcast_m_rbs_to_be_released_list_l,
                    broadcast_m_rbs_to_be_setup_mod_list_l,
                    mbs_cu_to_du_rrc_info_s,
                    mbs_service_area_c>
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

struct broadcast_context_mod_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      mbs_service_area_present                     = false;
  bool                                                      broadcast_m_rbs_to_be_setup_mod_list_present = false;
  bool                                                      broadcast_m_rbs_to_be_modified_list_present  = false;
  bool                                                      broadcast_m_rbs_to_be_released_list_present  = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<mbs_service_area_c>                            mbs_service_area;
  ie_field_s<mbs_cu_to_du_rrc_info_s>                       mbs_cu_to_du_rrc_info;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<broadcast_m_rbs_to_be_setup_mod_item_ies_o>, 1, 32, true>>
      broadcast_m_rbs_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<broadcast_m_rbs_to_be_modified_item_ies_o>, 1, 32, true>>
      broadcast_m_rbs_to_be_modified_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<broadcast_m_rbs_to_be_released_item_ies_o>, 1, 32, true>>
      broadcast_m_rbs_to_be_released_list;

  // sequence methods
  broadcast_context_mod_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastContextModificationRequest ::= SEQUENCE
using broadcast_context_mod_request_s = elementary_procedure_option<broadcast_context_mod_request_ies_container>;

// BroadcastMRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_failed_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_failed_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_failed_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_failed_to_be_modified_item_s&       broadcast_m_rbs_failed_to_be_modified_item() { return c; }
    const broadcast_m_rbs_failed_to_be_modified_item_s& broadcast_m_rbs_failed_to_be_modified_item() const { return c; }

  private:
    broadcast_m_rbs_failed_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_failed_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_failed_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_failed_to_be_setup_mod_item_s&       broadcast_m_rbs_failed_to_be_setup_mod_item() { return c; }
    const broadcast_m_rbs_failed_to_be_setup_mod_item_s& broadcast_m_rbs_failed_to_be_setup_mod_item() const
    {
      return c;
    }

  private:
    broadcast_m_rbs_failed_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_modified_item_s&       broadcast_m_rbs_modified_item() { return c; }
    const broadcast_m_rbs_modified_item_s& broadcast_m_rbs_modified_item() const { return c; }

  private:
    broadcast_m_rbs_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_setup_mod_item_s&       broadcast_m_rbs_setup_mod_item() { return c; }
    const broadcast_m_rbs_setup_mod_item_s& broadcast_m_rbs_setup_mod_item() const { return c; }

  private:
    broadcast_m_rbs_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-FailedToBeModified-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_failed_to_be_modified_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_failed_to_be_modified_item_ies_o>, 32>;

// BroadcastMRBs-FailedToBeSetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_failed_to_be_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o>, 32>;

// BroadcastMRBs-Modified-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_modified_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_modified_item_ies_o>, 32>;

// BroadcastMRBs-SetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_setup_mod_item_ies_o>, 32>;

// BroadcastContextModificationResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_mod_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_mbs_f1ap_id,
        gnb_du_mbs_f1ap_id,
        broadcast_m_rbs_setup_mod_list,
        broadcast_m_rbs_failed_to_be_setup_mod_list,
        broadcast_m_rbs_modified_list,
        broadcast_m_rbs_failed_to_be_modified_list,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                            gnb_cu_mbs_f1ap_id();
    uint64_t&                                            gnb_du_mbs_f1ap_id();
    broadcast_m_rbs_setup_mod_list_l&                    broadcast_m_rbs_setup_mod_list();
    broadcast_m_rbs_failed_to_be_setup_mod_list_l&       broadcast_m_rbs_failed_to_be_setup_mod_list();
    broadcast_m_rbs_modified_list_l&                     broadcast_m_rbs_modified_list();
    broadcast_m_rbs_failed_to_be_modified_list_l&        broadcast_m_rbs_failed_to_be_modified_list();
    crit_diagnostics_s&                                  crit_diagnostics();
    const uint64_t&                                      gnb_cu_mbs_f1ap_id() const;
    const uint64_t&                                      gnb_du_mbs_f1ap_id() const;
    const broadcast_m_rbs_setup_mod_list_l&              broadcast_m_rbs_setup_mod_list() const;
    const broadcast_m_rbs_failed_to_be_setup_mod_list_l& broadcast_m_rbs_failed_to_be_setup_mod_list() const;
    const broadcast_m_rbs_modified_list_l&               broadcast_m_rbs_modified_list() const;
    const broadcast_m_rbs_failed_to_be_modified_list_l&  broadcast_m_rbs_failed_to_be_modified_list() const;
    const crit_diagnostics_s&                            crit_diagnostics() const;

  private:
    types type_;
    choice_buffer_t<broadcast_m_rbs_failed_to_be_modified_list_l,
                    broadcast_m_rbs_failed_to_be_setup_mod_list_l,
                    broadcast_m_rbs_modified_list_l,
                    broadcast_m_rbs_setup_mod_list_l,
                    crit_diagnostics_s>
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

struct broadcast_context_mod_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      broadcast_m_rbs_setup_mod_list_present              = false;
  bool                                                      broadcast_m_rbs_failed_to_be_setup_mod_list_present = false;
  bool                                                      broadcast_m_rbs_modified_list_present               = false;
  bool                                                      broadcast_m_rbs_failed_to_be_modified_list_present  = false;
  bool                                                      crit_diagnostics_present                            = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<broadcast_m_rbs_setup_mod_item_ies_o>, 1, 32, true>>
      broadcast_m_rbs_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o>, 1, 32, true>>
      broadcast_m_rbs_failed_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<broadcast_m_rbs_modified_item_ies_o>, 1, 32, true>>
      broadcast_m_rbs_modified_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<broadcast_m_rbs_failed_to_be_modified_item_ies_o>, 1, 32, true>>
                                 broadcast_m_rbs_failed_to_be_modified_list;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;

  // sequence methods
  broadcast_context_mod_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastContextModificationResponse ::= SEQUENCE
using broadcast_context_mod_resp_s = elementary_procedure_option<broadcast_context_mod_resp_ies_container>;

// BroadcastContextReleaseCommandIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_release_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_mbs_f1ap_id, gnb_du_mbs_f1ap_id, cause, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&       gnb_cu_mbs_f1ap_id();
    uint64_t&       gnb_du_mbs_f1ap_id();
    cause_c&        cause();
    const uint64_t& gnb_cu_mbs_f1ap_id() const;
    const uint64_t& gnb_du_mbs_f1ap_id() const;
    const cause_c&  cause() const;

  private:
    types                    type_;
    choice_buffer_t<cause_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_context_release_cmd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<cause_c>                                       cause;

  // sequence methods
  broadcast_context_release_cmd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastContextReleaseCommand ::= SEQUENCE
using broadcast_context_release_cmd_s = elementary_procedure_option<broadcast_context_release_cmd_ies_container>;

// BroadcastContextReleaseCompleteIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_release_complete_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_mbs_f1ap_id, gnb_du_mbs_f1ap_id, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_mbs_f1ap_id();
    uint64_t&                 gnb_du_mbs_f1ap_id();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_mbs_f1ap_id() const;
    const uint64_t&           gnb_du_mbs_f1ap_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                               type_;
    choice_buffer_t<crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_context_release_complete_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  broadcast_context_release_complete_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastContextReleaseComplete ::= SEQUENCE
using broadcast_context_release_complete_s =
    elementary_procedure_option<broadcast_context_release_complete_ies_container>;

// BroadcastContextReleaseRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_release_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_mbs_f1ap_id, gnb_du_mbs_f1ap_id, cause, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&       gnb_cu_mbs_f1ap_id();
    uint64_t&       gnb_du_mbs_f1ap_id();
    cause_c&        cause();
    const uint64_t& gnb_cu_mbs_f1ap_id() const;
    const uint64_t& gnb_du_mbs_f1ap_id() const;
    const cause_c&  cause() const;

  private:
    types                    type_;
    choice_buffer_t<cause_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_context_release_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<cause_c>                                       cause;

  // sequence methods
  broadcast_context_release_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastContextReleaseRequest ::= SEQUENCE
using broadcast_context_release_request_s =
    elementary_procedure_option<broadcast_context_release_request_ies_container>;

// BroadcastContextSetupFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_mbs_f1ap_id, gnb_du_mbs_f1ap_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_mbs_f1ap_id();
    uint64_t&                 gnb_du_mbs_f1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_mbs_f1ap_id() const;
    const uint64_t&           gnb_du_mbs_f1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_context_setup_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      gnb_du_mbs_f1ap_id_present = false;
  bool                                                      crit_diagnostics_present   = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  broadcast_context_setup_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastContextSetupFailure ::= SEQUENCE
using broadcast_context_setup_fail_s = elementary_procedure_option<broadcast_context_setup_fail_ies_container>;

// BroadcastMRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_to_be_setup_item_s&       broadcast_m_rbs_to_be_setup_item() { return c; }
    const broadcast_m_rbs_to_be_setup_item_s& broadcast_m_rbs_to_be_setup_item() const { return c; }

  private:
    broadcast_m_rbs_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-ToBeSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_to_be_setup_item_ies_o>, 32>;

// BroadcastContextSetupRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_mbs_f1ap_id,
        mbs_session_id,
        mbs_service_area,
        mbs_cu_to_du_rrc_info,
        snssai,
        broadcast_m_rbs_to_be_setup_list,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                 gnb_cu_mbs_f1ap_id();
    mbs_session_id_s&                         mbs_session_id();
    mbs_service_area_c&                       mbs_service_area();
    mbs_cu_to_du_rrc_info_s&                  mbs_cu_to_du_rrc_info();
    snssai_s&                                 snssai();
    broadcast_m_rbs_to_be_setup_list_l&       broadcast_m_rbs_to_be_setup_list();
    const uint64_t&                           gnb_cu_mbs_f1ap_id() const;
    const mbs_session_id_s&                   mbs_session_id() const;
    const mbs_service_area_c&                 mbs_service_area() const;
    const mbs_cu_to_du_rrc_info_s&            mbs_cu_to_du_rrc_info() const;
    const snssai_s&                           snssai() const;
    const broadcast_m_rbs_to_be_setup_list_l& broadcast_m_rbs_to_be_setup_list() const;

  private:
    types type_;
    choice_buffer_t<broadcast_m_rbs_to_be_setup_list_l,
                    mbs_cu_to_du_rrc_info_s,
                    mbs_service_area_c,
                    mbs_session_id_s,
                    snssai_s>
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

struct broadcast_context_setup_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      mbs_service_area_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<mbs_session_id_s>                              mbs_session_id;
  ie_field_s<mbs_service_area_c>                            mbs_service_area;
  ie_field_s<mbs_cu_to_du_rrc_info_s>                       mbs_cu_to_du_rrc_info;
  ie_field_s<snssai_s>                                      snssai;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<broadcast_m_rbs_to_be_setup_item_ies_o>, 1, 32, true>>
      broadcast_m_rbs_to_be_setup_list;

  // sequence methods
  broadcast_context_setup_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastContextSetupRequest ::= SEQUENCE
using broadcast_context_setup_request_s = elementary_procedure_option<broadcast_context_setup_request_ies_container>;

// BroadcastMRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_failed_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_failed_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_failed_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_failed_to_be_setup_item_s&       broadcast_m_rbs_failed_to_be_setup_item() { return c; }
    const broadcast_m_rbs_failed_to_be_setup_item_s& broadcast_m_rbs_failed_to_be_setup_item() const { return c; }

  private:
    broadcast_m_rbs_failed_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_setup_item_s&       broadcast_m_rbs_setup_item() { return c; }
    const broadcast_m_rbs_setup_item_s& broadcast_m_rbs_setup_item() const { return c; }

  private:
    broadcast_m_rbs_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-FailedToBeSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_failed_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_failed_to_be_setup_item_ies_o>, 32>;

// BroadcastMRBs-Setup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using broadcast_m_rbs_setup_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_setup_item_ies_o>, 32>;

// BroadcastContextSetupResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_mbs_f1ap_id,
        gnb_du_mbs_f1ap_id,
        broadcast_m_rbs_setup_list,
        broadcast_m_rbs_failed_to_be_setup_list,
        broadcast_area_scope,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                        gnb_cu_mbs_f1ap_id();
    uint64_t&                                        gnb_du_mbs_f1ap_id();
    broadcast_m_rbs_setup_list_l&                    broadcast_m_rbs_setup_list();
    broadcast_m_rbs_failed_to_be_setup_list_l&       broadcast_m_rbs_failed_to_be_setup_list();
    broadcast_area_scope_c&                          broadcast_area_scope();
    crit_diagnostics_s&                              crit_diagnostics();
    const uint64_t&                                  gnb_cu_mbs_f1ap_id() const;
    const uint64_t&                                  gnb_du_mbs_f1ap_id() const;
    const broadcast_m_rbs_setup_list_l&              broadcast_m_rbs_setup_list() const;
    const broadcast_m_rbs_failed_to_be_setup_list_l& broadcast_m_rbs_failed_to_be_setup_list() const;
    const broadcast_area_scope_c&                    broadcast_area_scope() const;
    const crit_diagnostics_s&                        crit_diagnostics() const;

  private:
    types type_;
    choice_buffer_t<broadcast_area_scope_c,
                    broadcast_m_rbs_failed_to_be_setup_list_l,
                    broadcast_m_rbs_setup_list_l,
                    crit_diagnostics_s>
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

struct broadcast_context_setup_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      broadcast_m_rbs_failed_to_be_setup_list_present = false;
  bool                                                      broadcast_area_scope_present                    = false;
  bool                                                      crit_diagnostics_present                        = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<broadcast_m_rbs_setup_item_ies_o>, 1, 32, true>>
      broadcast_m_rbs_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<broadcast_m_rbs_failed_to_be_setup_item_ies_o>, 1, 32, true>>
                                     broadcast_m_rbs_failed_to_be_setup_list;
  ie_field_s<broadcast_area_scope_c> broadcast_area_scope;
  ie_field_s<crit_diagnostics_s>     crit_diagnostics;

  // sequence methods
  broadcast_context_setup_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastContextSetupResponse ::= SEQUENCE
using broadcast_context_setup_resp_s = elementary_procedure_option<broadcast_context_setup_resp_ies_container>;

// CUDURadioInformationTransferIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cu_du_radio_info_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cu_du_radio_info_type, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                      transaction_id();
    cu_du_radio_info_type_c&       cu_du_radio_info_type();
    const uint16_t&                transaction_id() const;
    const cu_du_radio_info_type_c& cu_du_radio_info_type() const;

  private:
    types                                    type_;
    choice_buffer_t<cu_du_radio_info_type_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct cu_du_radio_info_transfer_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cu_du_radio_info_type_c>               cu_du_radio_info_type;

  // sequence methods
  cu_du_radio_info_transfer_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CUDURadioInformationTransfer ::= SEQUENCE
using cu_du_radio_info_transfer_s = elementary_procedure_option<cu_du_radio_info_transfer_ies_container>;

// Candidate-SpCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct candidate_sp_cell_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { candidate_sp_cell_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::candidate_sp_cell_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    candidate_sp_cell_item_s&       candidate_sp_cell_item() { return c; }
    const candidate_sp_cell_item_s& candidate_sp_cell_item() const { return c; }

  private:
    candidate_sp_cell_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Candidate-SpCell-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using candidate_sp_cell_list_l = dyn_array<protocol_ie_single_container_s<candidate_sp_cell_item_ies_o>>;

// CellTrafficTraceIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cell_traffic_trace_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        trace_id,
        trace_collection_entity_ip_address,
        privacy_ind,
        trace_collection_entity_uri,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                                    gnb_cu_ue_f1ap_id();
    uint64_t&                                                    gnb_du_ue_f1ap_id();
    fixed_octstring<8, true>&                                    trace_id();
    bounded_bitstring<1, 160, true, true>&                       trace_collection_entity_ip_address();
    privacy_ind_e&                                               privacy_ind();
    visible_string<0, MAX_ASN_STRING_LENGTH, false, true>&       trace_collection_entity_uri();
    const uint64_t&                                              gnb_cu_ue_f1ap_id() const;
    const uint64_t&                                              gnb_du_ue_f1ap_id() const;
    const fixed_octstring<8, true>&                              trace_id() const;
    const bounded_bitstring<1, 160, true, true>&                 trace_collection_entity_ip_address() const;
    const privacy_ind_e&                                         privacy_ind() const;
    const visible_string<0, MAX_ASN_STRING_LENGTH, false, true>& trace_collection_entity_uri() const;

  private:
    types type_;
    choice_buffer_t<bounded_bitstring<1, 160, true, true>,
                    fixed_octstring<8, true>,
                    visible_string<0, MAX_ASN_STRING_LENGTH, false, true>>
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

struct cell_traffic_trace_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                              privacy_ind_present                 = false;
  bool                                                              trace_collection_entity_uri_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>         gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>         gnb_du_ue_f1ap_id;
  ie_field_s<fixed_octstring<8, true>>                              trace_id;
  ie_field_s<bounded_bitstring<1, 160, true, true>>                 trace_collection_entity_ip_address;
  ie_field_s<privacy_ind_e>                                         privacy_ind;
  ie_field_s<visible_string<0, MAX_ASN_STRING_LENGTH, false, true>> trace_collection_entity_uri;

  // sequence methods
  cell_traffic_trace_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellTrafficTrace ::= SEQUENCE
using cell_traffic_trace_s = elementary_procedure_option<cell_traffic_trace_ies_container>;

// Cells-Broadcast-Cancelled-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_broadcast_cancelled_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_broadcast_cancelled_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::cells_broadcast_cancelled_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_broadcast_cancelled_item_s&       cells_broadcast_cancelled_item() { return c; }
    const cells_broadcast_cancelled_item_s& cells_broadcast_cancelled_item() const { return c; }

  private:
    cells_broadcast_cancelled_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-Broadcast-Cancelled-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_broadcast_cancelled_list_l =
    dyn_array<protocol_ie_single_container_s<cells_broadcast_cancelled_list_item_ies_o>>;

// Cells-Broadcast-Completed-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_broadcast_completed_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_broadcast_completed_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::cells_broadcast_completed_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_broadcast_completed_item_s&       cells_broadcast_completed_item() { return c; }
    const cells_broadcast_completed_item_s& cells_broadcast_completed_item() const { return c; }

  private:
    cells_broadcast_completed_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-Broadcast-Completed-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_broadcast_completed_list_l =
    dyn_array<protocol_ie_single_container_s<cells_broadcast_completed_list_item_ies_o>>;

// Cells-Failed-to-be-Activated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_failed_to_be_activ_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_failed_to_be_activ_list_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::cells_failed_to_be_activ_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_failed_to_be_activ_list_item_s&       cells_failed_to_be_activ_list_item() { return c; }
    const cells_failed_to_be_activ_list_item_s& cells_failed_to_be_activ_list_item() const { return c; }

  private:
    cells_failed_to_be_activ_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-Failed-to-be-Activated-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_failed_to_be_activ_list_l =
    dyn_array<protocol_ie_single_container_s<cells_failed_to_be_activ_list_item_ies_o>>;

// Cells-Status-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_status_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_status_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::cells_status_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_status_item_s&       cells_status_item() { return c; }
    const cells_status_item_s& cells_status_item() const { return c; }

  private:
    cells_status_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-Status-List ::= SEQUENCE (SIZE (0..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using cells_status_list_l = dyn_array<protocol_ie_single_container_s<cells_status_item_ies_o>>;

// Cells-To-Be-Broadcast-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_to_be_broadcast_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_to_be_broadcast_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::cells_to_be_broadcast_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_to_be_broadcast_item_s&       cells_to_be_broadcast_item() { return c; }
    const cells_to_be_broadcast_item_s& cells_to_be_broadcast_item() const { return c; }

  private:
    cells_to_be_broadcast_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-To-Be-Broadcast-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_to_be_broadcast_list_l = dyn_array<protocol_ie_single_container_s<cells_to_be_broadcast_list_item_ies_o>>;

// Cells-to-be-Activated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_to_be_activ_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_to_be_activ_list_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::cells_to_be_activ_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_to_be_activ_list_item_s&       cells_to_be_activ_list_item() { return c; }
    const cells_to_be_activ_list_item_s& cells_to_be_activ_list_item() const { return c; }

  private:
    cells_to_be_activ_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-to-be-Activated-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_to_be_activ_list_l = dyn_array<protocol_ie_single_container_s<cells_to_be_activ_list_item_ies_o>>;

// Cells-to-be-Barred-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_to_be_barred_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_to_be_barred_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::cells_to_be_barred_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_to_be_barred_item_s&       cells_to_be_barred_item() { return c; }
    const cells_to_be_barred_item_s& cells_to_be_barred_item() const { return c; }

  private:
    cells_to_be_barred_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-to-be-Barred-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using cells_to_be_barred_list_l = dyn_array<protocol_ie_single_container_s<cells_to_be_barred_item_ies_o>>;

// Cells-to-be-Deactivated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_to_be_deactiv_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_to_be_deactiv_list_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::cells_to_be_deactiv_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_to_be_deactiv_list_item_s&       cells_to_be_deactiv_list_item() { return c; }
    const cells_to_be_deactiv_list_item_s& cells_to_be_deactiv_list_item() const { return c; }

  private:
    cells_to_be_deactiv_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-to-be-Deactivated-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_to_be_deactiv_list_l = dyn_array<protocol_ie_single_container_s<cells_to_be_deactiv_list_item_ies_o>>;

// DL-UP-TNL-Address-to-Update-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct dl_up_tnl_address_to_upd_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { dl_up_tnl_address_to_upd_list_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::dl_up_tnl_address_to_upd_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dl_up_tnl_address_to_upd_list_item_s&       dl_up_tnl_address_to_upd_list_item() { return c; }
    const dl_up_tnl_address_to_upd_list_item_s& dl_up_tnl_address_to_upd_list_item() const { return c; }

  private:
    dl_up_tnl_address_to_upd_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DL-UP-TNL-Address-to-Update-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using dl_up_tnl_address_to_upd_list_l =
    bounded_array<protocol_ie_single_container_s<dl_up_tnl_address_to_upd_list_item_ies_o>, 8>;

// DLRRCMessageTransferIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct dl_rrc_msg_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        old_gnb_du_ue_f1ap_id,
        srb_id,
        execute_dupl,
        rrc_container,
        rat_freq_prio_info,
        rrc_delivery_status_request,
        ue_context_not_retrievable,
        redirected_rrc_msg,
        plmn_assist_info_for_net_shar,
        new_gnb_cu_ue_f1ap_id,
        add_rrm_prio_idx,
        srb_map_info,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                               gnb_cu_ue_f1ap_id();
    uint64_t&                               gnb_du_ue_f1ap_id();
    uint64_t&                               old_gnb_du_ue_f1ap_id();
    uint8_t&                                srb_id();
    execute_dupl_e&                         execute_dupl();
    unbounded_octstring<true>&              rrc_container();
    rat_freq_prio_info_c&                   rat_freq_prio_info();
    rrc_delivery_status_request_e&          rrc_delivery_status_request();
    ue_context_not_retrievable_e&           ue_context_not_retrievable();
    unbounded_octstring<true>&              redirected_rrc_msg();
    fixed_octstring<3, true>&               plmn_assist_info_for_net_shar();
    uint64_t&                               new_gnb_cu_ue_f1ap_id();
    fixed_bitstring<32, false, true>&       add_rrm_prio_idx();
    fixed_bitstring<16, false, true>&       srb_map_info();
    const uint64_t&                         gnb_cu_ue_f1ap_id() const;
    const uint64_t&                         gnb_du_ue_f1ap_id() const;
    const uint64_t&                         old_gnb_du_ue_f1ap_id() const;
    const uint8_t&                          srb_id() const;
    const execute_dupl_e&                   execute_dupl() const;
    const unbounded_octstring<true>&        rrc_container() const;
    const rat_freq_prio_info_c&             rat_freq_prio_info() const;
    const rrc_delivery_status_request_e&    rrc_delivery_status_request() const;
    const ue_context_not_retrievable_e&     ue_context_not_retrievable() const;
    const unbounded_octstring<true>&        redirected_rrc_msg() const;
    const fixed_octstring<3, true>&         plmn_assist_info_for_net_shar() const;
    const uint64_t&                         new_gnb_cu_ue_f1ap_id() const;
    const fixed_bitstring<32, false, true>& add_rrm_prio_idx() const;
    const fixed_bitstring<16, false, true>& srb_map_info() const;

  private:
    types type_;
    choice_buffer_t<fixed_bitstring<32, false, true>,
                    fixed_octstring<3, true>,
                    rat_freq_prio_info_c,
                    unbounded_octstring<true>>
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

struct dl_rrc_msg_transfer_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      old_gnb_du_ue_f1ap_id_present         = false;
  bool                                                      execute_dupl_present                  = false;
  bool                                                      rat_freq_prio_info_present            = false;
  bool                                                      rrc_delivery_status_request_present   = false;
  bool                                                      ue_context_not_retrievable_present    = false;
  bool                                                      redirected_rrc_msg_present            = false;
  bool                                                      plmn_assist_info_for_net_shar_present = false;
  bool                                                      new_gnb_cu_ue_f1ap_id_present         = false;
  bool                                                      add_rrm_prio_idx_present              = false;
  bool                                                      srb_map_info_present                  = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> old_gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint8_t, 0, 3, true, true>>            srb_id;
  ie_field_s<execute_dupl_e>                                execute_dupl;
  ie_field_s<unbounded_octstring<true>>                     rrc_container;
  ie_field_s<rat_freq_prio_info_c>                          rat_freq_prio_info;
  ie_field_s<rrc_delivery_status_request_e>                 rrc_delivery_status_request;
  ie_field_s<ue_context_not_retrievable_e>                  ue_context_not_retrievable;
  ie_field_s<unbounded_octstring<true>>                     redirected_rrc_msg;
  ie_field_s<fixed_octstring<3, true>>                      plmn_assist_info_for_net_shar;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> new_gnb_cu_ue_f1ap_id;
  ie_field_s<fixed_bitstring<32, false, true>>              add_rrm_prio_idx;
  ie_field_s<fixed_bitstring<16, false, true>>              srb_map_info;

  // sequence methods
  dl_rrc_msg_transfer_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLRRCMessageTransfer ::= SEQUENCE
using dl_rrc_msg_transfer_s = elementary_procedure_option<dl_rrc_msg_transfer_ies_container>;

// DRB-Activity-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drb_activity_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drb_activity_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drb_activity_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drb_activity_item_s&       drb_activity_item() { return c; }
    const drb_activity_item_s& drb_activity_item() const { return c; }

  private:
    drb_activity_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-Activity-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drb_activity_list_l = dyn_array<protocol_ie_single_container_s<drb_activity_item_ies_o>>;

// DRB-Notify-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drb_notify_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drb_notify_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drb_notify_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drb_notify_item_s&       drb_notify_item() { return c; }
    const drb_notify_item_s& drb_notify_item() const { return c; }

  private:
    drb_notify_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-Notify-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drb_notify_list_l = dyn_array<protocol_ie_single_container_s<drb_notify_item_ies_o>>;

// DRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_failed_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_failed_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_failed_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_failed_to_be_modified_item_s&       drbs_failed_to_be_modified_item() { return c; }
    const drbs_failed_to_be_modified_item_s& drbs_failed_to_be_modified_item() const { return c; }

  private:
    drbs_failed_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-FailedToBeModified-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using drbs_failed_to_be_modified_list_l =
    dyn_array<protocol_ie_single_container_s<drbs_failed_to_be_modified_item_ies_o>>;

// DRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_failed_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_failed_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_failed_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_failed_to_be_setup_item_s&       drbs_failed_to_be_setup_item() { return c; }
    const drbs_failed_to_be_setup_item_s& drbs_failed_to_be_setup_item() const { return c; }

  private:
    drbs_failed_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-FailedToBeSetup-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_failed_to_be_setup_list_l = dyn_array<protocol_ie_single_container_s<drbs_failed_to_be_setup_item_ies_o>>;

// DRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_failed_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_failed_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_failed_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_failed_to_be_setup_mod_item_s&       drbs_failed_to_be_setup_mod_item() { return c; }
    const drbs_failed_to_be_setup_mod_item_s& drbs_failed_to_be_setup_mod_item() const { return c; }

  private:
    drbs_failed_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-FailedToBeSetupMod-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using drbs_failed_to_be_setup_mod_list_l =
    dyn_array<protocol_ie_single_container_s<drbs_failed_to_be_setup_mod_item_ies_o>>;

// DRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_modified_item_s&       drbs_modified_item() { return c; }
    const drbs_modified_item_s& drbs_modified_item() const { return c; }

  private:
    drbs_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-Modified-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_modified_list_l = dyn_array<protocol_ie_single_container_s<drbs_modified_item_ies_o>>;

// DRBs-ModifiedConf-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_modified_conf_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_modified_conf_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_modified_conf_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_modified_conf_item_s&       drbs_modified_conf_item() { return c; }
    const drbs_modified_conf_item_s& drbs_modified_conf_item() const { return c; }

  private:
    drbs_modified_conf_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-ModifiedConf-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_modified_conf_list_l = dyn_array<protocol_ie_single_container_s<drbs_modified_conf_item_ies_o>>;

// DRBs-Required-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_required_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_required_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_required_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_required_to_be_modified_item_s&       drbs_required_to_be_modified_item() { return c; }
    const drbs_required_to_be_modified_item_s& drbs_required_to_be_modified_item() const { return c; }

  private:
    drbs_required_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-Required-ToBeModified-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using drbs_required_to_be_modified_list_l =
    dyn_array<protocol_ie_single_container_s<drbs_required_to_be_modified_item_ies_o>>;

// DRBs-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_required_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_required_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_required_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_required_to_be_released_item_s&       drbs_required_to_be_released_item() { return c; }
    const drbs_required_to_be_released_item_s& drbs_required_to_be_released_item() const { return c; }

  private:
    drbs_required_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-Required-ToBeReleased-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using drbs_required_to_be_released_list_l =
    dyn_array<protocol_ie_single_container_s<drbs_required_to_be_released_item_ies_o>>;

// DRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_setup_item_s&       drbs_setup_item() { return c; }
    const drbs_setup_item_s& drbs_setup_item() const { return c; }

  private:
    drbs_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-Setup-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_setup_list_l = dyn_array<protocol_ie_single_container_s<drbs_setup_item_ies_o>>;

// DRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_setup_mod_item_s&       drbs_setup_mod_item() { return c; }
    const drbs_setup_mod_item_s& drbs_setup_mod_item() const { return c; }

  private:
    drbs_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-SetupMod-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_setup_mod_list_l = dyn_array<protocol_ie_single_container_s<drbs_setup_mod_item_ies_o>>;

// DRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_to_be_modified_item_s&       drbs_to_be_modified_item() { return c; }
    const drbs_to_be_modified_item_s& drbs_to_be_modified_item() const { return c; }

  private:
    drbs_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-ToBeModified-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_to_be_modified_list_l = dyn_array<protocol_ie_single_container_s<drbs_to_be_modified_item_ies_o>>;

// DRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_to_be_released_item_s&       drbs_to_be_released_item() { return c; }
    const drbs_to_be_released_item_s& drbs_to_be_released_item() const { return c; }

  private:
    drbs_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-ToBeReleased-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_to_be_released_list_l = dyn_array<protocol_ie_single_container_s<drbs_to_be_released_item_ies_o>>;

// DRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_to_be_setup_item_s&       drbs_to_be_setup_item() { return c; }
    const drbs_to_be_setup_item_s& drbs_to_be_setup_item() const { return c; }

  private:
    drbs_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-ToBeSetup-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_to_be_setup_list_l = dyn_array<protocol_ie_single_container_s<drbs_to_be_setup_item_ies_o>>;

// DRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::drbs_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_to_be_setup_mod_item_s&       drbs_to_be_setup_mod_item() { return c; }
    const drbs_to_be_setup_mod_item_s& drbs_to_be_setup_mod_item() const { return c; }

  private:
    drbs_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_to_be_setup_mod_list_l = dyn_array<protocol_ie_single_container_s<drbs_to_be_setup_mod_item_ies_o>>;

// DUCURadioInformationTransferIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct du_cu_radio_info_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, du_cu_radio_info_type, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                      transaction_id();
    du_cu_radio_info_type_c&       du_cu_radio_info_type();
    const uint16_t&                transaction_id() const;
    const du_cu_radio_info_type_c& du_cu_radio_info_type() const;

  private:
    types                                    type_;
    choice_buffer_t<du_cu_radio_info_type_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct du_cu_radio_info_transfer_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<du_cu_radio_info_type_c>               du_cu_radio_info_type;

  // sequence methods
  du_cu_radio_info_transfer_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DUCURadioInformationTransfer ::= SEQUENCE
using du_cu_radio_info_transfer_s = elementary_procedure_option<du_cu_radio_info_transfer_ies_container>;

// DeactivateTraceIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct deactiv_trace_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, trace_id, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                       gnb_cu_ue_f1ap_id();
    uint64_t&                       gnb_du_ue_f1ap_id();
    fixed_octstring<8, true>&       trace_id();
    const uint64_t&                 gnb_cu_ue_f1ap_id() const;
    const uint64_t&                 gnb_du_ue_f1ap_id() const;
    const fixed_octstring<8, true>& trace_id() const;

  private:
    types                                     type_;
    choice_buffer_t<fixed_octstring<8, true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct deactiv_trace_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<fixed_octstring<8, true>>                      trace_id;

  // sequence methods
  deactiv_trace_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DeactivateTrace ::= SEQUENCE
using deactiv_trace_s = elementary_procedure_option<deactiv_trace_ies_container>;

// Dedicated-SIDelivery-NeededUE-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ded_si_delivery_needed_ue_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ded_si_delivery_needed_ue_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ded_si_delivery_needed_ue_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ded_si_delivery_needed_ue_item_s&       ded_si_delivery_needed_ue_item() { return c; }
    const ded_si_delivery_needed_ue_item_s& ded_si_delivery_needed_ue_item() const { return c; }

  private:
    ded_si_delivery_needed_ue_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Dedicated-SIDelivery-NeededUE-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ded_si_delivery_needed_ue_list_l =
    dyn_array<protocol_ie_single_container_s<ded_si_delivery_needed_ue_item_ies_o>>;

// E-CIDMeasurementFailureIndication-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct e_c_id_meas_fail_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, lmf_ue_meas_id, ran_ue_meas_id, cause, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&       gnb_cu_ue_f1ap_id();
    uint64_t&       gnb_du_ue_f1ap_id();
    uint16_t&       lmf_ue_meas_id();
    uint16_t&       ran_ue_meas_id();
    cause_c&        cause();
    const uint64_t& gnb_cu_ue_f1ap_id() const;
    const uint64_t& gnb_du_ue_f1ap_id() const;
    const uint16_t& lmf_ue_meas_id() const;
    const uint16_t& ran_ue_meas_id() const;
    const cause_c&  cause() const;

  private:
    types                    type_;
    choice_buffer_t<cause_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct e_c_id_meas_fail_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint16_t, 1, 256, true, true>>         lmf_ue_meas_id;
  ie_field_s<integer<uint16_t, 1, 256, true, true>>         ran_ue_meas_id;
  ie_field_s<cause_c>                                       cause;

  // sequence methods
  e_c_id_meas_fail_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-CIDMeasurementFailureIndication ::= SEQUENCE
using e_c_id_meas_fail_ind_s = elementary_procedure_option<e_c_id_meas_fail_ind_ies_container>;

// E-CIDMeasurementInitiationFailure-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct e_c_id_meas_initiation_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        lmf_ue_meas_id,
        ran_ue_meas_id,
        cause,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    uint16_t&                 lmf_ue_meas_id();
    uint16_t&                 ran_ue_meas_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
    const uint16_t&           lmf_ue_meas_id() const;
    const uint16_t&           ran_ue_meas_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct e_c_id_meas_initiation_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint16_t, 1, 256, true, true>>         lmf_ue_meas_id;
  ie_field_s<integer<uint16_t, 1, 256, true, true>>         ran_ue_meas_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  e_c_id_meas_initiation_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-CIDMeasurementInitiationFailure ::= SEQUENCE
using e_c_id_meas_initiation_fail_s = elementary_procedure_option<e_c_id_meas_initiation_fail_ies_container>;

// E-CIDMeasurementInitiationRequest-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct e_c_id_meas_initiation_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        lmf_ue_meas_id,
        ran_ue_meas_id,
        e_c_id_report_characteristics,
        e_c_id_meas_periodicity,
        e_c_id_meas_quantities,
        pos_meas_periodicity_nr_ao_a,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                              gnb_cu_ue_f1ap_id();
    uint64_t&                              gnb_du_ue_f1ap_id();
    uint16_t&                              lmf_ue_meas_id();
    uint16_t&                              ran_ue_meas_id();
    e_c_id_report_characteristics_e&       e_c_id_report_characteristics();
    meas_periodicity_e&                    e_c_id_meas_periodicity();
    e_c_id_meas_quantities_l&              e_c_id_meas_quantities();
    pos_meas_periodicity_nr_ao_a_e&        pos_meas_periodicity_nr_ao_a();
    const uint64_t&                        gnb_cu_ue_f1ap_id() const;
    const uint64_t&                        gnb_du_ue_f1ap_id() const;
    const uint16_t&                        lmf_ue_meas_id() const;
    const uint16_t&                        ran_ue_meas_id() const;
    const e_c_id_report_characteristics_e& e_c_id_report_characteristics() const;
    const meas_periodicity_e&              e_c_id_meas_periodicity() const;
    const e_c_id_meas_quantities_l&        e_c_id_meas_quantities() const;
    const pos_meas_periodicity_nr_ao_a_e&  pos_meas_periodicity_nr_ao_a() const;

  private:
    types                                     type_;
    choice_buffer_t<e_c_id_meas_quantities_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct e_c_id_meas_initiation_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      e_c_id_meas_periodicity_present      = false;
  bool                                                      pos_meas_periodicity_nr_ao_a_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint16_t, 1, 256, true, true>>         lmf_ue_meas_id;
  ie_field_s<integer<uint16_t, 1, 256, true, true>>         ran_ue_meas_id;
  ie_field_s<e_c_id_report_characteristics_e>               e_c_id_report_characteristics;
  ie_field_s<meas_periodicity_e>                            e_c_id_meas_periodicity;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<e_c_id_meas_quantities_item_ies_o>, 1, 64, true>>
                                             e_c_id_meas_quantities;
  ie_field_s<pos_meas_periodicity_nr_ao_a_e> pos_meas_periodicity_nr_ao_a;

  // sequence methods
  e_c_id_meas_initiation_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-CIDMeasurementInitiationRequest ::= SEQUENCE
using e_c_id_meas_initiation_request_s = elementary_procedure_option<e_c_id_meas_initiation_request_ies_container>;

// E-CIDMeasurementInitiationResponse-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct e_c_id_meas_initiation_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        lmf_ue_meas_id,
        ran_ue_meas_id,
        e_c_id_meas_result,
        cell_portion_id,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                   gnb_cu_ue_f1ap_id();
    uint64_t&                   gnb_du_ue_f1ap_id();
    uint16_t&                   lmf_ue_meas_id();
    uint16_t&                   ran_ue_meas_id();
    e_c_id_meas_result_s&       e_c_id_meas_result();
    uint16_t&                   cell_portion_id();
    crit_diagnostics_s&         crit_diagnostics();
    const uint64_t&             gnb_cu_ue_f1ap_id() const;
    const uint64_t&             gnb_du_ue_f1ap_id() const;
    const uint16_t&             lmf_ue_meas_id() const;
    const uint16_t&             ran_ue_meas_id() const;
    const e_c_id_meas_result_s& e_c_id_meas_result() const;
    const uint16_t&             cell_portion_id() const;
    const crit_diagnostics_s&   crit_diagnostics() const;

  private:
    types                                                     type_;
    choice_buffer_t<crit_diagnostics_s, e_c_id_meas_result_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct e_c_id_meas_initiation_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      e_c_id_meas_result_present = false;
  bool                                                      cell_portion_id_present    = false;
  bool                                                      crit_diagnostics_present   = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint16_t, 1, 256, true, true>>         lmf_ue_meas_id;
  ie_field_s<integer<uint16_t, 1, 256, true, true>>         ran_ue_meas_id;
  ie_field_s<e_c_id_meas_result_s>                          e_c_id_meas_result;
  ie_field_s<integer<uint16_t, 0, 4095, true, true>>        cell_portion_id;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  e_c_id_meas_initiation_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-CIDMeasurementInitiationResponse ::= SEQUENCE
using e_c_id_meas_initiation_resp_s = elementary_procedure_option<e_c_id_meas_initiation_resp_ies_container>;

// E-CIDMeasurementReport-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct e_c_id_meas_report_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        lmf_ue_meas_id,
        ran_ue_meas_id,
        e_c_id_meas_result,
        cell_portion_id,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                   gnb_cu_ue_f1ap_id();
    uint64_t&                   gnb_du_ue_f1ap_id();
    uint16_t&                   lmf_ue_meas_id();
    uint16_t&                   ran_ue_meas_id();
    e_c_id_meas_result_s&       e_c_id_meas_result();
    uint16_t&                   cell_portion_id();
    const uint64_t&             gnb_cu_ue_f1ap_id() const;
    const uint64_t&             gnb_du_ue_f1ap_id() const;
    const uint16_t&             lmf_ue_meas_id() const;
    const uint16_t&             ran_ue_meas_id() const;
    const e_c_id_meas_result_s& e_c_id_meas_result() const;
    const uint16_t&             cell_portion_id() const;

  private:
    types                                 type_;
    choice_buffer_t<e_c_id_meas_result_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct e_c_id_meas_report_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      cell_portion_id_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint16_t, 1, 256, true, true>>         lmf_ue_meas_id;
  ie_field_s<integer<uint16_t, 1, 256, true, true>>         ran_ue_meas_id;
  ie_field_s<e_c_id_meas_result_s>                          e_c_id_meas_result;
  ie_field_s<integer<uint16_t, 0, 4095, true, true>>        cell_portion_id;

  // sequence methods
  e_c_id_meas_report_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-CIDMeasurementReport ::= SEQUENCE
using e_c_id_meas_report_s = elementary_procedure_option<e_c_id_meas_report_ies_container>;

// E-CIDMeasurementTerminationCommand-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct e_c_id_meas_termination_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, lmf_ue_meas_id, ran_ue_meas_id, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&       gnb_cu_ue_f1ap_id();
    uint64_t&       gnb_du_ue_f1ap_id();
    uint16_t&       lmf_ue_meas_id();
    uint16_t&       ran_ue_meas_id();
    const uint64_t& gnb_cu_ue_f1ap_id() const;
    const uint64_t& gnb_du_ue_f1ap_id() const;
    const uint16_t& lmf_ue_meas_id() const;
    const uint16_t& ran_ue_meas_id() const;

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct e_c_id_meas_termination_cmd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint16_t, 1, 256, true, true>>         lmf_ue_meas_id;
  ie_field_s<integer<uint16_t, 1, 256, true, true>>         ran_ue_meas_id;

  // sequence methods
  e_c_id_meas_termination_cmd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-CIDMeasurementTerminationCommand ::= SEQUENCE
using e_c_id_meas_termination_cmd_s = elementary_procedure_option<e_c_id_meas_termination_cmd_ies_container>;

// ErrorIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct error_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct error_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      gnb_cu_ue_f1ap_id_present = false;
  bool                                                      gnb_du_ue_f1ap_id_present = false;
  bool                                                      cause_present             = false;
  bool                                                      crit_diagnostics_present  = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>         transaction_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  error_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ErrorIndication ::= SEQUENCE
using error_ind_s = elementary_procedure_option<error_ind_ies_container>;

// SemipersistentSRS-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct semipersistent_srs_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { srs_spatial_relation_per_srs_res, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srs_spatial_relation_per_srs_res; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    spatial_relation_per_srs_res_s&       srs_spatial_relation_per_srs_res() { return c; }
    const spatial_relation_per_srs_res_s& srs_spatial_relation_per_srs_res() const { return c; }

  private:
    spatial_relation_per_srs_res_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-associatedLogicalF1-ConnectionItemRes ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_associated_lc_f1_conn_item_res_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_associated_lc_f1_conn_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ue_associated_lc_f1_conn_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_associated_lc_f1_conn_item_s&       ue_associated_lc_f1_conn_item() { return c; }
    const ue_associated_lc_f1_conn_item_s& ue_associated_lc_f1_conn_item() const { return c; }

  private:
    ue_associated_lc_f1_conn_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-Failed-To-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_tnl_assoc_failed_to_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_tnl_assoc_failed_to_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::gnb_cu_tnl_assoc_failed_to_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_cu_tnl_assoc_failed_to_setup_item_s&       gnb_cu_tnl_assoc_failed_to_setup_item() { return c; }
    const gnb_cu_tnl_assoc_failed_to_setup_item_s& gnb_cu_tnl_assoc_failed_to_setup_item() const { return c; }

  private:
    gnb_cu_tnl_assoc_failed_to_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_tnl_assoc_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_tnl_assoc_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::gnb_cu_tnl_assoc_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_cu_tnl_assoc_setup_item_s&       gnb_cu_tnl_assoc_setup_item() { return c; }
    const gnb_cu_tnl_assoc_setup_item_s& gnb_cu_tnl_assoc_setup_item() const { return c; }

  private:
    gnb_cu_tnl_assoc_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-To-Add-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_tnl_assoc_to_add_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_tnl_assoc_to_add_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::gnb_cu_tnl_assoc_to_add_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_cu_tnl_assoc_to_add_item_s&       gnb_cu_tnl_assoc_to_add_item() { return c; }
    const gnb_cu_tnl_assoc_to_add_item_s& gnb_cu_tnl_assoc_to_add_item() const { return c; }

  private:
    gnb_cu_tnl_assoc_to_add_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-To-Remove-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_tnl_assoc_to_rem_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_tnl_assoc_to_rem_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::gnb_cu_tnl_assoc_to_rem_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_cu_tnl_assoc_to_rem_item_s&       gnb_cu_tnl_assoc_to_rem_item() { return c; }
    const gnb_cu_tnl_assoc_to_rem_item_s& gnb_cu_tnl_assoc_to_rem_item() const { return c; }

  private:
    gnb_cu_tnl_assoc_to_rem_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-To-Update-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_tnl_assoc_to_upd_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_tnl_assoc_to_upd_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::gnb_cu_tnl_assoc_to_upd_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_cu_tnl_assoc_to_upd_item_s&       gnb_cu_tnl_assoc_to_upd_item() { return c; }
    const gnb_cu_tnl_assoc_to_upd_item_s& gnb_cu_tnl_assoc_to_upd_item() const { return c; }

  private:
    gnb_cu_tnl_assoc_to_upd_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-DU-Served-Cells-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_served_cells_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_du_served_cells_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::gnb_du_served_cells_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_du_served_cells_item_s&       gnb_du_served_cells_item() { return c; }
    const gnb_du_served_cells_item_s& gnb_du_served_cells_item() const { return c; }

  private:
    gnb_du_served_cells_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-DU-TNL-Association-To-Remove-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_tnl_assoc_to_rem_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_du_tnl_assoc_to_rem_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::gnb_du_tnl_assoc_to_rem_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_du_tnl_assoc_to_rem_item_s&       gnb_du_tnl_assoc_to_rem_item() { return c; }
    const gnb_du_tnl_assoc_to_rem_item_s& gnb_du_tnl_assoc_to_rem_item() const { return c; }

  private:
    gnb_du_tnl_assoc_to_rem_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// IAB-Allocated-TNL-Address-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct iab_allocated_tnl_address_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { iab_allocated_tnl_address_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::iab_allocated_tnl_address_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    iab_allocated_tnl_address_item_s&       iab_allocated_tnl_address_item() { return c; }
    const iab_allocated_tnl_address_item_s& iab_allocated_tnl_address_item() const { return c; }

  private:
    iab_allocated_tnl_address_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// IAB-TNL-Addresses-To-Remove-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct iab_tnl_addresses_to_rem_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { iab_tnl_addresses_to_rem_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::iab_tnl_addresses_to_rem_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    iab_tnl_addresses_to_rem_item_s&       iab_tnl_addresses_to_rem_item() { return c; }
    const iab_tnl_addresses_to_rem_item_s& iab_tnl_addresses_to_rem_item() const { return c; }

  private:
    iab_tnl_addresses_to_rem_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MC-PagingCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct mc_paging_cell_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mc_paging_cell_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::mc_paging_cell_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mc_paging_cell_item_s&       mc_paging_cell_item() { return c; }
    const mc_paging_cell_item_s& mc_paging_cell_item() const { return c; }

  private:
    mc_paging_cell_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastF1UContext-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_f1_u_context_failed_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_f1_u_context_failed_to_be_setup_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::multicast_f1_u_context_failed_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_f1_u_context_failed_to_be_setup_item_s& multicast_f1_u_context_failed_to_be_setup_item() { return c; }
    const multicast_f1_u_context_failed_to_be_setup_item_s& multicast_f1_u_context_failed_to_be_setup_item() const
    {
      return c;
    }

  private:
    multicast_f1_u_context_failed_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastF1UContext-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_f1_u_context_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_f1_u_context_setup_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::multicast_f1_u_context_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_f1_u_context_setup_item_s&       multicast_f1_u_context_setup_item() { return c; }
    const multicast_f1_u_context_setup_item_s& multicast_f1_u_context_setup_item() const { return c; }

  private:
    multicast_f1_u_context_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastF1UContext-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_f1_u_context_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_f1_u_context_to_be_setup_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::multicast_f1_u_context_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_f1_u_context_to_be_setup_item_s&       multicast_f1_u_context_to_be_setup_item() { return c; }
    const multicast_f1_u_context_to_be_setup_item_s& multicast_f1_u_context_to_be_setup_item() const { return c; }

  private:
    multicast_f1_u_context_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_failed_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_failed_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::multicast_m_rbs_failed_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_failed_to_be_modified_item_s&       multicast_m_rbs_failed_to_be_modified_item() { return c; }
    const multicast_m_rbs_failed_to_be_modified_item_s& multicast_m_rbs_failed_to_be_modified_item() const { return c; }

  private:
    multicast_m_rbs_failed_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_failed_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_failed_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::multicast_m_rbs_failed_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_failed_to_be_setup_item_s&       multicast_m_rbs_failed_to_be_setup_item() { return c; }
    const multicast_m_rbs_failed_to_be_setup_item_s& multicast_m_rbs_failed_to_be_setup_item() const { return c; }

  private:
    multicast_m_rbs_failed_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_failed_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_failed_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::multicast_m_rbs_failed_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_failed_to_be_setup_mod_item_s&       multicast_m_rbs_failed_to_be_setup_mod_item() { return c; }
    const multicast_m_rbs_failed_to_be_setup_mod_item_s& multicast_m_rbs_failed_to_be_setup_mod_item() const
    {
      return c;
    }

  private:
    multicast_m_rbs_failed_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::multicast_m_rbs_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_modified_item_s&       multicast_m_rbs_modified_item() { return c; }
    const multicast_m_rbs_modified_item_s& multicast_m_rbs_modified_item() const { return c; }

  private:
    multicast_m_rbs_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::multicast_m_rbs_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_setup_item_s&       multicast_m_rbs_setup_item() { return c; }
    const multicast_m_rbs_setup_item_s& multicast_m_rbs_setup_item() const { return c; }

  private:
    multicast_m_rbs_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::multicast_m_rbs_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_setup_mod_item_s&       multicast_m_rbs_setup_mod_item() { return c; }
    const multicast_m_rbs_setup_mod_item_s& multicast_m_rbs_setup_mod_item() const { return c; }

  private:
    multicast_m_rbs_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::multicast_m_rbs_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_to_be_modified_item_s&       multicast_m_rbs_to_be_modified_item() { return c; }
    const multicast_m_rbs_to_be_modified_item_s& multicast_m_rbs_to_be_modified_item() const { return c; }

  private:
    multicast_m_rbs_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::multicast_m_rbs_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_to_be_released_item_s&       multicast_m_rbs_to_be_released_item() { return c; }
    const multicast_m_rbs_to_be_released_item_s& multicast_m_rbs_to_be_released_item() const { return c; }

  private:
    multicast_m_rbs_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::multicast_m_rbs_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_to_be_setup_item_s&       multicast_m_rbs_to_be_setup_item() { return c; }
    const multicast_m_rbs_to_be_setup_item_s& multicast_m_rbs_to_be_setup_item() const { return c; }

  private:
    multicast_m_rbs_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::multicast_m_rbs_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_to_be_setup_mod_item_s&       multicast_m_rbs_to_be_setup_mod_item() { return c; }
    const multicast_m_rbs_to_be_setup_mod_item_s& multicast_m_rbs_to_be_setup_mod_item() const { return c; }

  private:
    multicast_m_rbs_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NR-CGI-List-For-Restart-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct nr_cgi_list_for_restart_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { nr_cgi_list_for_restart_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::nr_cgi_list_for_restart_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nr_cgi_list_for_restart_item_s&       nr_cgi_list_for_restart_item() { return c; }
    const nr_cgi_list_for_restart_item_s& nr_cgi_list_for_restart_item() const { return c; }

  private:
    nr_cgi_list_for_restart_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Neighbour-Cell-Information-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct neighbour_cell_info_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { neighbour_cell_info_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::neighbour_cell_info_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    neighbour_cell_info_item_s&       neighbour_cell_info_item() { return c; }
    const neighbour_cell_info_item_s& neighbour_cell_info_item() const { return c; }

  private:
    neighbour_cell_info_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWS-Failed-NR-CGI-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pws_failed_nr_cgi_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { pws_failed_nr_cgi_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::pws_failed_nr_cgi_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pws_failed_nr_cgi_item_s&       pws_failed_nr_cgi_item() { return c; }
    const pws_failed_nr_cgi_item_s& pws_failed_nr_cgi_item() const { return c; }

  private:
    pws_failed_nr_cgi_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PagingCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct paging_cell_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { paging_cell_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::paging_cell_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    paging_cell_item_s&       paging_cell_item() { return c; }
    const paging_cell_item_s& paging_cell_item() const { return c; }

  private:
    paging_cell_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Potential-SpCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct potential_sp_cell_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { potential_sp_cell_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::potential_sp_cell_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    potential_sp_cell_item_s&       potential_sp_cell_item() { return c; }
    const potential_sp_cell_item_s& potential_sp_cell_item() const { return c; }

  private:
    potential_sp_cell_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Protected-EUTRA-Resources-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct protected_eutra_res_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { protected_eutra_res_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::protected_eutra_res_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    protected_eutra_res_item_s&       protected_eutra_res_item() { return c; }
    const protected_eutra_res_item_s& protected_eutra_res_item() const { return c; }

  private:
    protected_eutra_res_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ResetAll ::= ENUMERATED
struct reset_all_opts {
  enum options { reset_all, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<reset_all_opts, true> reset_all_e;

// ResetType-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using reset_type_ext_ies_o = protocol_ies_empty_o;

// SCell-FailedtoSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct scell_failedto_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { scell_failedto_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::scell_failedto_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scell_failedto_setup_item_s&       scell_failedto_setup_item() { return c; }
    const scell_failedto_setup_item_s& scell_failedto_setup_item() const { return c; }

  private:
    scell_failedto_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SCell-FailedtoSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct scell_failedto_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { scell_failedto_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::scell_failedto_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scell_failedto_setup_mod_item_s&       scell_failedto_setup_mod_item() { return c; }
    const scell_failedto_setup_mod_item_s& scell_failedto_setup_mod_item() const { return c; }

  private:
    scell_failedto_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SCell-ToBeRemoved-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct scell_to_be_remd_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { scell_to_be_remd_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::scell_to_be_remd_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scell_to_be_remd_item_s&       scell_to_be_remd_item() { return c; }
    const scell_to_be_remd_item_s& scell_to_be_remd_item() const { return c; }

  private:
    scell_to_be_remd_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SCell-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct scell_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { scell_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::scell_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scell_to_be_setup_item_s&       scell_to_be_setup_item() { return c; }
    const scell_to_be_setup_item_s& scell_to_be_setup_item() const { return c; }

  private:
    scell_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SCell-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct scell_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { scell_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::scell_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scell_to_be_setup_mod_item_s&       scell_to_be_setup_mod_item() { return c; }
    const scell_to_be_setup_mod_item_s& scell_to_be_setup_mod_item() const { return c; }

  private:
    scell_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_failed_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_failed_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::sl_drbs_failed_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_failed_to_be_modified_item_s&       sl_drbs_failed_to_be_modified_item() { return c; }
    const sl_drbs_failed_to_be_modified_item_s& sl_drbs_failed_to_be_modified_item() const { return c; }

  private:
    sl_drbs_failed_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_failed_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_failed_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::sl_drbs_failed_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_failed_to_be_setup_item_s&       sl_drbs_failed_to_be_setup_item() { return c; }
    const sl_drbs_failed_to_be_setup_item_s& sl_drbs_failed_to_be_setup_item() const { return c; }

  private:
    sl_drbs_failed_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_failed_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_failed_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::sl_drbs_failed_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_failed_to_be_setup_mod_item_s&       sl_drbs_failed_to_be_setup_mod_item() { return c; }
    const sl_drbs_failed_to_be_setup_mod_item_s& sl_drbs_failed_to_be_setup_mod_item() const { return c; }

  private:
    sl_drbs_failed_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::sl_drbs_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_modified_item_s&       sl_drbs_modified_item() { return c; }
    const sl_drbs_modified_item_s& sl_drbs_modified_item() const { return c; }

  private:
    sl_drbs_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-ModifiedConf-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_modified_conf_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_modified_conf_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::sl_drbs_modified_conf_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_modified_conf_item_s&       sl_drbs_modified_conf_item() { return c; }
    const sl_drbs_modified_conf_item_s& sl_drbs_modified_conf_item() const { return c; }

  private:
    sl_drbs_modified_conf_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-Required-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_required_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_required_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::sl_drbs_required_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_required_to_be_modified_item_s&       sl_drbs_required_to_be_modified_item() { return c; }
    const sl_drbs_required_to_be_modified_item_s& sl_drbs_required_to_be_modified_item() const { return c; }

  private:
    sl_drbs_required_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_required_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_required_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::sl_drbs_required_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_required_to_be_released_item_s&       sl_drbs_required_to_be_released_item() { return c; }
    const sl_drbs_required_to_be_released_item_s& sl_drbs_required_to_be_released_item() const { return c; }

  private:
    sl_drbs_required_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::sl_drbs_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_setup_item_s&       sl_drbs_setup_item() { return c; }
    const sl_drbs_setup_item_s& sl_drbs_setup_item() const { return c; }

  private:
    sl_drbs_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::sl_drbs_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_setup_mod_item_s&       sl_drbs_setup_mod_item() { return c; }
    const sl_drbs_setup_mod_item_s& sl_drbs_setup_mod_item() const { return c; }

  private:
    sl_drbs_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::sl_drbs_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_to_be_modified_item_s&       sl_drbs_to_be_modified_item() { return c; }
    const sl_drbs_to_be_modified_item_s& sl_drbs_to_be_modified_item() const { return c; }

  private:
    sl_drbs_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::sl_drbs_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_to_be_released_item_s&       sl_drbs_to_be_released_item() { return c; }
    const sl_drbs_to_be_released_item_s& sl_drbs_to_be_released_item() const { return c; }

  private:
    sl_drbs_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::sl_drbs_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_to_be_setup_item_s&       sl_drbs_to_be_setup_item() { return c; }
    const sl_drbs_to_be_setup_item_s& sl_drbs_to_be_setup_item() const { return c; }

  private:
    sl_drbs_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::sl_drbs_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_to_be_setup_mod_item_s&       sl_drbs_to_be_setup_mod_item() { return c; }
    const sl_drbs_to_be_setup_mod_item_s& sl_drbs_to_be_setup_mod_item() const { return c; }

  private:
    sl_drbs_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_failed_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_failed_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_failed_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_failed_to_be_setup_item_s&       srbs_failed_to_be_setup_item() { return c; }
    const srbs_failed_to_be_setup_item_s& srbs_failed_to_be_setup_item() const { return c; }

  private:
    srbs_failed_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_failed_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_failed_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_failed_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_failed_to_be_setup_mod_item_s&       srbs_failed_to_be_setup_mod_item() { return c; }
    const srbs_failed_to_be_setup_mod_item_s& srbs_failed_to_be_setup_mod_item() const { return c; }

  private:
    srbs_failed_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_modified_item_s&       srbs_modified_item() { return c; }
    const srbs_modified_item_s& srbs_modified_item() const { return c; }

  private:
    srbs_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_required_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_required_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_required_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_required_to_be_released_item_s&       srbs_required_to_be_released_item() { return c; }
    const srbs_required_to_be_released_item_s& srbs_required_to_be_released_item() const { return c; }

  private:
    srbs_required_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_setup_item_s&       srbs_setup_item() { return c; }
    const srbs_setup_item_s& srbs_setup_item() const { return c; }

  private:
    srbs_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_setup_mod_item_s&       srbs_setup_mod_item() { return c; }
    const srbs_setup_mod_item_s& srbs_setup_mod_item() const { return c; }

  private:
    srbs_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_to_be_released_item_s&       srbs_to_be_released_item() { return c; }
    const srbs_to_be_released_item_s& srbs_to_be_released_item() const { return c; }

  private:
    srbs_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_to_be_setup_item_s&       srbs_to_be_setup_item() { return c; }
    const srbs_to_be_setup_item_s& srbs_to_be_setup_item() const { return c; }

  private:
    srbs_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::srbs_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_to_be_setup_mod_item_s&       srbs_to_be_setup_mod_item() { return c; }
    const srbs_to_be_setup_mod_item_s& srbs_to_be_setup_mod_item() const { return c; }

  private:
    srbs_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRSType-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using srs_type_ext_ies_o = protocol_ies_empty_o;

// SemipersistentSRS ::= SEQUENCE
struct semipersistent_srs_s {
  bool                                                   ext                          = false;
  bool                                                   srs_spatial_relation_present = false;
  uint8_t                                                srs_res_set_id               = 0;
  spatial_relation_info_s                                srs_spatial_relation;
  protocol_ext_container_l<semipersistent_srs_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Served-Cells-To-Add-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct served_cells_to_add_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { served_cells_to_add_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::served_cells_to_add_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    served_cells_to_add_item_s&       served_cells_to_add_item() { return c; }
    const served_cells_to_add_item_s& served_cells_to_add_item() const { return c; }

  private:
    served_cells_to_add_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Served-Cells-To-Delete-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct served_cells_to_delete_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { served_cells_to_delete_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::served_cells_to_delete_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    served_cells_to_delete_item_s&       served_cells_to_delete_item() { return c; }
    const served_cells_to_delete_item_s& served_cells_to_delete_item() const { return c; }

  private:
    served_cells_to_delete_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Served-Cells-To-Modify-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct served_cells_to_modify_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { served_cells_to_modify_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::served_cells_to_modify_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    served_cells_to_modify_item_s&       served_cells_to_modify_item() { return c; }
    const served_cells_to_modify_item_s& served_cells_to_modify_item() const { return c; }

  private:
    served_cells_to_modify_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ServingCellMO-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct serving_cell_mo_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { serving_cell_mo_list_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::serving_cell_mo_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    serving_cell_mo_list_item_s&       serving_cell_mo_list_item() { return c; }
    const serving_cell_mo_list_item_s& serving_cell_mo_list_item() const { return c; }

  private:
    serving_cell_mo_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TRPInformationItemTRPResp ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct trp_info_item_trp_resp_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { trp_info_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::trp_info_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    trp_info_item_s&       trp_info_item() { return c; }
    const trp_info_item_s& trp_info_item() const { return c; }

  private:
    trp_info_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TRPInformationTypeItemTRPReq ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct trp_info_type_item_trp_req_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { trp_info_type_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::trp_info_type_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    trp_info_type_item_e&       trp_info_type_item() { return c; }
    const trp_info_type_item_e& trp_info_type_item() const { return c; }

  private:
    trp_info_type_item_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-MulticastMRBs-ConfirmedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_multicast_m_rbs_confirmed_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ue_multicast_m_rbs_confirmed_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_multicast_m_rbs_confirmed_to_be_modified_item_s& ue_multicast_m_rbs_confirmed_to_be_modified_item() { return c; }
    const ue_multicast_m_rbs_confirmed_to_be_modified_item_s& ue_multicast_m_rbs_confirmed_to_be_modified_item() const
    {
      return c;
    }

  private:
    ue_multicast_m_rbs_confirmed_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-MulticastMRBs-RequiredToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_multicast_m_rbs_required_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_multicast_m_rbs_required_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ue_multicast_m_rbs_required_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_multicast_m_rbs_required_to_be_modified_item_s& ue_multicast_m_rbs_required_to_be_modified_item() { return c; }
    const ue_multicast_m_rbs_required_to_be_modified_item_s& ue_multicast_m_rbs_required_to_be_modified_item() const
    {
      return c;
    }

  private:
    ue_multicast_m_rbs_required_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-MulticastMRBs-RequiredToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_multicast_m_rbs_required_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_multicast_m_rbs_required_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ue_multicast_m_rbs_required_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_multicast_m_rbs_required_to_be_released_item_s& ue_multicast_m_rbs_required_to_be_released_item() { return c; }
    const ue_multicast_m_rbs_required_to_be_released_item_s& ue_multicast_m_rbs_required_to_be_released_item() const
    {
      return c;
    }

  private:
    ue_multicast_m_rbs_required_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-MulticastMRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_multicast_m_rbs_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_multicast_m_rbs_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ue_multicast_m_rbs_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_multicast_m_rbs_setup_item_s&       ue_multicast_m_rbs_setup_item() { return c; }
    const ue_multicast_m_rbs_setup_item_s& ue_multicast_m_rbs_setup_item() const { return c; }

  private:
    ue_multicast_m_rbs_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-MulticastMRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_multicast_m_rbs_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_multicast_m_rbs_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ue_multicast_m_rbs_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_multicast_m_rbs_to_be_released_item_s&       ue_multicast_m_rbs_to_be_released_item() { return c; }
    const ue_multicast_m_rbs_to_be_released_item_s& ue_multicast_m_rbs_to_be_released_item() const { return c; }

  private:
    ue_multicast_m_rbs_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-MulticastMRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_multicast_m_rbs_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_multicast_m_rbs_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ue_multicast_m_rbs_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_multicast_m_rbs_to_be_setup_item_s&       ue_multicast_m_rbs_to_be_setup_item() { return c; }
    const ue_multicast_m_rbs_to_be_setup_item_s& ue_multicast_m_rbs_to_be_setup_item() const { return c; }

  private:
    ue_multicast_m_rbs_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-MulticastMRBs-ToBeSetup-atModify-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_multicast_m_rbs_to_be_setup_at_modify_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ue_multicast_m_rbs_to_be_setup_at_modify_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_multicast_m_rbs_to_be_setup_at_modify_item_s&       ue_multicast_m_rbs_to_be_setup_at_modify_item() { return c; }
    const ue_multicast_m_rbs_to_be_setup_at_modify_item_s& ue_multicast_m_rbs_to_be_setup_at_modify_item() const
    {
      return c;
    }

  private:
    ue_multicast_m_rbs_to_be_setup_at_modify_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-associatedLogicalF1-ConnectionItemResAck ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_associated_lc_f1_conn_item_res_ack_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_associated_lc_f1_conn_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ue_associated_lc_f1_conn_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_associated_lc_f1_conn_item_s&       ue_associated_lc_f1_conn_item() { return c; }
    const ue_associated_lc_f1_conn_item_s& ue_associated_lc_f1_conn_item() const { return c; }

  private:
    ue_associated_lc_f1_conn_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-associatedLogicalF1-ConnectionListRes ::= SEQUENCE (SIZE (1..65536)) OF
// ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using ue_associated_lc_f1_conn_list_res_l =
    dyn_array<protocol_ie_single_container_s<ue_associated_lc_f1_conn_item_res_o>>;

// UEIdentity-List-For-Paging-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_id_list_for_paging_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_id_list_for_paging_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ue_id_list_for_paging_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_id_list_for_paging_item_s&       ue_id_list_for_paging_item() { return c; }
    const ue_id_list_for_paging_item_s& ue_id_list_for_paging_item() const { return c; }

  private:
    ue_id_list_for_paging_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UL-UP-TNL-Address-to-Update-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ul_up_tnl_address_to_upd_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ul_up_tnl_address_to_upd_list_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ul_up_tnl_address_to_upd_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ul_up_tnl_address_to_upd_list_item_s&       ul_up_tnl_address_to_upd_list_item() { return c; }
    const ul_up_tnl_address_to_upd_list_item_s& ul_up_tnl_address_to_upd_list_item() const { return c; }

  private:
    ul_up_tnl_address_to_upd_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UL-UP-TNL-Information-to-Update-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ul_up_tnl_info_to_upd_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ul_up_tnl_info_to_upd_list_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ul_up_tnl_info_to_upd_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ul_up_tnl_info_to_upd_list_item_s&       ul_up_tnl_info_to_upd_list_item() { return c; }
    const ul_up_tnl_info_to_upd_list_item_s& ul_up_tnl_info_to_upd_list_item() const { return c; }

  private:
    ul_up_tnl_info_to_upd_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-Failed-To-Setup-List ::= SEQUENCE (SIZE (1..32)) OF
// ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using gnb_cu_tnl_assoc_failed_to_setup_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_cu_tnl_assoc_failed_to_setup_item_ies_o>, 32>;

// GNB-CU-TNL-Association-Setup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using gnb_cu_tnl_assoc_setup_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_cu_tnl_assoc_setup_item_ies_o>, 32>;

// GNB-CU-TNL-Association-To-Add-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using gnb_cu_tnl_assoc_to_add_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_add_item_ies_o>, 32>;

// GNB-CU-TNL-Association-To-Remove-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using gnb_cu_tnl_assoc_to_rem_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_rem_item_ies_o>, 32>;

// GNB-CU-TNL-Association-To-Update-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using gnb_cu_tnl_assoc_to_upd_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_upd_item_ies_o>, 32>;

// GNB-DU-Served-Cells-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using gnb_du_served_cells_list_l = dyn_array<protocol_ie_single_container_s<gnb_du_served_cells_item_ies_o>>;

// GNB-DU-TNL-Association-To-Remove-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using gnb_du_tnl_assoc_to_rem_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_du_tnl_assoc_to_rem_item_ies_o>, 32>;

// IAB-Allocated-TNL-Address-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using iab_allocated_tnl_address_list_l =
    dyn_array<protocol_ie_single_container_s<iab_allocated_tnl_address_list_item_ies_o>>;

// IAB-TNL-Addresses-To-Remove-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using iab_tnl_addresses_to_rem_list_l = dyn_array<protocol_ie_single_container_s<iab_tnl_addresses_to_rem_item_ies_o>>;

// MC-PagingCell-list ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using mc_paging_cell_list_l = dyn_array<protocol_ie_single_container_s<mc_paging_cell_item_ies_o>>;

// MulticastF1UContext-FailedToBeSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES
// : IEsSetParam}
using multicast_f1_u_context_failed_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_f1_u_context_failed_to_be_setup_item_ies_o>, 32>;

// MulticastF1UContext-Setup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_f1_u_context_setup_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_f1_u_context_setup_item_ies_o>, 32>;

// MulticastF1UContext-ToBeSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_f1_u_context_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_f1_u_context_to_be_setup_item_ies_o>, 32>;

// MulticastMRBs-FailedToBeModified-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_failed_to_be_modified_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_failed_to_be_modified_item_ies_o>, 32>;

// MulticastMRBs-FailedToBeSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_failed_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_failed_to_be_setup_item_ies_o>, 32>;

// MulticastMRBs-FailedToBeSetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_failed_to_be_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_failed_to_be_setup_mod_item_ies_o>, 32>;

// MulticastMRBs-Modified-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_modified_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_modified_item_ies_o>, 32>;

// MulticastMRBs-Setup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using multicast_m_rbs_setup_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_setup_item_ies_o>, 32>;

// MulticastMRBs-SetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_setup_mod_item_ies_o>, 32>;

// MulticastMRBs-ToBeModified-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_to_be_modified_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_to_be_modified_item_ies_o>, 32>;

// MulticastMRBs-ToBeReleased-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_to_be_released_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_to_be_released_item_ies_o>, 32>;

// MulticastMRBs-ToBeSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_to_be_setup_item_ies_o>, 32>;

// MulticastMRBs-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_to_be_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_to_be_setup_mod_item_ies_o>, 32>;

// NR-CGI-List-For-Restart-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using nr_cgi_list_for_restart_list_l =
    dyn_array<protocol_ie_single_container_s<nr_cgi_list_for_restart_list_item_ies_o>>;

// Neighbour-Cell-Information-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using neighbour_cell_info_list_l = dyn_array<protocol_ie_single_container_s<neighbour_cell_info_item_ies_o>>;

// PWS-Failed-NR-CGI-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using pws_failed_nr_cgi_list_l = dyn_array<protocol_ie_single_container_s<pws_failed_nr_cgi_list_item_ies_o>>;

// PagingCell-list ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using paging_cell_list_l = dyn_array<protocol_ie_single_container_s<paging_cell_item_ies_o>>;

// Potential-SpCell-List ::= SEQUENCE (SIZE (0..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using potential_sp_cell_list_l = dyn_array<protocol_ie_single_container_s<potential_sp_cell_item_ies_o>>;

// Protected-EUTRA-Resources-List ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using protected_eutra_res_list_l = dyn_array<protocol_ie_single_container_s<protected_eutra_res_item_ies_o>>;

// ResetType ::= CHOICE
struct reset_type_c {
  struct types_opts {
    enum options { f1_interface, part_of_f1_interface, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

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
  reset_all_e& f1_interface()
  {
    assert_choice_type(types::f1_interface, type_, "ResetType");
    return c.get<reset_all_e>();
  }
  ue_associated_lc_f1_conn_list_res_l& part_of_f1_interface()
  {
    assert_choice_type(types::part_of_f1_interface, type_, "ResetType");
    return c.get<ue_associated_lc_f1_conn_list_res_l>();
  }
  protocol_ie_single_container_s<reset_type_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "ResetType");
    return c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>();
  }
  const reset_all_e& f1_interface() const
  {
    assert_choice_type(types::f1_interface, type_, "ResetType");
    return c.get<reset_all_e>();
  }
  const ue_associated_lc_f1_conn_list_res_l& part_of_f1_interface() const
  {
    assert_choice_type(types::part_of_f1_interface, type_, "ResetType");
    return c.get<ue_associated_lc_f1_conn_list_res_l>();
  }
  const protocol_ie_single_container_s<reset_type_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "ResetType");
    return c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>();
  }
  reset_all_e&                                          set_f1_interface();
  ue_associated_lc_f1_conn_list_res_l&                  set_part_of_f1_interface();
  protocol_ie_single_container_s<reset_type_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                      type_;
  choice_buffer_t<protocol_ie_single_container_s<reset_type_ext_ies_o>, ue_associated_lc_f1_conn_list_res_l> c;

  void destroy_();
};

// SCell-FailedtoSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using scell_failedto_setup_list_l = bounded_array<protocol_ie_single_container_s<scell_failedto_setup_item_ies_o>, 32>;

// SCell-FailedtoSetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using scell_failedto_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<scell_failedto_setup_mod_item_ies_o>, 32>;

// SCell-ToBeRemoved-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using scell_to_be_remd_list_l = bounded_array<protocol_ie_single_container_s<scell_to_be_remd_item_ies_o>, 32>;

// SCell-ToBeSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using scell_to_be_setup_list_l = bounded_array<protocol_ie_single_container_s<scell_to_be_setup_item_ies_o>, 32>;

// SCell-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using scell_to_be_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<scell_to_be_setup_mod_item_ies_o>, 32>;

// SLDRBs-FailedToBeModified-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using sl_drbs_failed_to_be_modified_list_l =
    dyn_array<protocol_ie_single_container_s<sl_drbs_failed_to_be_modified_item_ies_o>>;

// SLDRBs-FailedToBeSetup-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using sl_drbs_failed_to_be_setup_list_l =
    dyn_array<protocol_ie_single_container_s<sl_drbs_failed_to_be_setup_item_ies_o>>;

// SLDRBs-FailedToBeSetupMod-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using sl_drbs_failed_to_be_setup_mod_list_l =
    dyn_array<protocol_ie_single_container_s<sl_drbs_failed_to_be_setup_mod_item_ies_o>>;

// SLDRBs-Modified-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using sl_drbs_modified_list_l = dyn_array<protocol_ie_single_container_s<sl_drbs_modified_item_ies_o>>;

// SLDRBs-ModifiedConf-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using sl_drbs_modified_conf_list_l = dyn_array<protocol_ie_single_container_s<sl_drbs_modified_conf_item_ies_o>>;

// SLDRBs-Required-ToBeModified-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using sl_drbs_required_to_be_modified_list_l =
    dyn_array<protocol_ie_single_container_s<sl_drbs_required_to_be_modified_item_ies_o>>;

// SLDRBs-Required-ToBeReleased-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using sl_drbs_required_to_be_released_list_l =
    dyn_array<protocol_ie_single_container_s<sl_drbs_required_to_be_released_item_ies_o>>;

// SLDRBs-Setup-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using sl_drbs_setup_list_l = dyn_array<protocol_ie_single_container_s<sl_drbs_setup_item_ies_o>>;

// SLDRBs-SetupMod-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using sl_drbs_setup_mod_list_l = dyn_array<protocol_ie_single_container_s<sl_drbs_setup_mod_item_ies_o>>;

// SLDRBs-ToBeModified-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using sl_drbs_to_be_modified_list_l = dyn_array<protocol_ie_single_container_s<sl_drbs_to_be_modified_item_ies_o>>;

// SLDRBs-ToBeReleased-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using sl_drbs_to_be_released_list_l = dyn_array<protocol_ie_single_container_s<sl_drbs_to_be_released_item_ies_o>>;

// SLDRBs-ToBeSetup-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using sl_drbs_to_be_setup_list_l = dyn_array<protocol_ie_single_container_s<sl_drbs_to_be_setup_item_ies_o>>;

// SLDRBs-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using sl_drbs_to_be_setup_mod_list_l = dyn_array<protocol_ie_single_container_s<sl_drbs_to_be_setup_mod_item_ies_o>>;

// SRBs-FailedToBeSetup-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_failed_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<srbs_failed_to_be_setup_item_ies_o>, 8>;

// SRBs-FailedToBeSetupMod-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using srbs_failed_to_be_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<srbs_failed_to_be_setup_mod_item_ies_o>, 8>;

// SRBs-Modified-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_modified_list_l = bounded_array<protocol_ie_single_container_s<srbs_modified_item_ies_o>, 8>;

// SRBs-Required-ToBeReleased-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using srbs_required_to_be_released_list_l =
    bounded_array<protocol_ie_single_container_s<srbs_required_to_be_released_item_ies_o>, 8>;

// SRBs-Setup-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_setup_list_l = bounded_array<protocol_ie_single_container_s<srbs_setup_item_ies_o>, 8>;

// SRBs-SetupMod-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_setup_mod_list_l = bounded_array<protocol_ie_single_container_s<srbs_setup_mod_item_ies_o>, 8>;

// SRBs-ToBeReleased-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_to_be_released_list_l = bounded_array<protocol_ie_single_container_s<srbs_to_be_released_item_ies_o>, 8>;

// SRBs-ToBeSetup-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_to_be_setup_list_l = bounded_array<protocol_ie_single_container_s<srbs_to_be_setup_item_ies_o>, 8>;

// SRBs-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_to_be_setup_mod_list_l = bounded_array<protocol_ie_single_container_s<srbs_to_be_setup_mod_item_ies_o>, 8>;

// SRSType ::= CHOICE
struct srs_type_c {
  struct types_opts {
    enum options { semipersistent_srs, aperiodic_srs, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  srs_type_c() = default;
  srs_type_c(const srs_type_c& other);
  srs_type_c& operator=(const srs_type_c& other);
  ~srs_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  semipersistent_srs_s& semipersistent_srs()
  {
    assert_choice_type(types::semipersistent_srs, type_, "SRSType");
    return c.get<semipersistent_srs_s>();
  }
  aperiodic_srs_s& aperiodic_srs()
  {
    assert_choice_type(types::aperiodic_srs, type_, "SRSType");
    return c.get<aperiodic_srs_s>();
  }
  protocol_ie_single_container_s<srs_type_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "SRSType");
    return c.get<protocol_ie_single_container_s<srs_type_ext_ies_o>>();
  }
  const semipersistent_srs_s& semipersistent_srs() const
  {
    assert_choice_type(types::semipersistent_srs, type_, "SRSType");
    return c.get<semipersistent_srs_s>();
  }
  const aperiodic_srs_s& aperiodic_srs() const
  {
    assert_choice_type(types::aperiodic_srs, type_, "SRSType");
    return c.get<aperiodic_srs_s>();
  }
  const protocol_ie_single_container_s<srs_type_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "SRSType");
    return c.get<protocol_ie_single_container_s<srs_type_ext_ies_o>>();
  }
  semipersistent_srs_s&                               set_semipersistent_srs();
  aperiodic_srs_s&                                    set_aperiodic_srs();
  protocol_ie_single_container_s<srs_type_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                      type_;
  choice_buffer_t<aperiodic_srs_s, protocol_ie_single_container_s<srs_type_ext_ies_o>, semipersistent_srs_s> c;

  void destroy_();
};

// Served-Cells-To-Add-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using served_cells_to_add_list_l = dyn_array<protocol_ie_single_container_s<served_cells_to_add_item_ies_o>>;

// Served-Cells-To-Delete-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using served_cells_to_delete_list_l = dyn_array<protocol_ie_single_container_s<served_cells_to_delete_item_ies_o>>;

// Served-Cells-To-Modify-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using served_cells_to_modify_list_l = dyn_array<protocol_ie_single_container_s<served_cells_to_modify_item_ies_o>>;

// ServingCellMO-List ::= SEQUENCE (SIZE (1..16)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using serving_cell_mo_list_l = bounded_array<protocol_ie_single_container_s<serving_cell_mo_list_item_ies_o>, 16>;

// TRPInformationListTRPResp ::= SEQUENCE (SIZE (1..65535)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using trp_info_list_trp_resp_l = dyn_array<protocol_ie_single_container_s<trp_info_item_trp_resp_o>>;

// TRPInformationTypeListTRPReq ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using trp_info_type_list_trp_req_l = dyn_array<protocol_ie_single_container_s<trp_info_type_item_trp_req_o>>;

// UE-MulticastMRBs-ConfirmedToBeModified-List ::= SEQUENCE (SIZE (1..64)) OF
// ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using ue_multicast_m_rbs_confirmed_to_be_modified_list_l =
    dyn_array<protocol_ie_single_container_s<ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o>>;

// UE-MulticastMRBs-RequiredToBeModified-List ::= SEQUENCE (SIZE (1..64)) OF
// ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using ue_multicast_m_rbs_required_to_be_modified_list_l =
    dyn_array<protocol_ie_single_container_s<ue_multicast_m_rbs_required_to_be_modified_item_ies_o>>;

// UE-MulticastMRBs-RequiredToBeReleased-List ::= SEQUENCE (SIZE (1..64)) OF
// ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using ue_multicast_m_rbs_required_to_be_released_list_l =
    dyn_array<protocol_ie_single_container_s<ue_multicast_m_rbs_required_to_be_released_item_ies_o>>;

// UE-MulticastMRBs-Setup-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ue_multicast_m_rbs_setup_list_l = dyn_array<protocol_ie_single_container_s<ue_multicast_m_rbs_setup_item_ies_o>>;

// UE-MulticastMRBs-ToBeReleased-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ue_multicast_m_rbs_to_be_released_list_l =
    dyn_array<protocol_ie_single_container_s<ue_multicast_m_rbs_to_be_released_item_ies_o>>;

// UE-MulticastMRBs-ToBeSetup-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ue_multicast_m_rbs_to_be_setup_list_l =
    dyn_array<protocol_ie_single_container_s<ue_multicast_m_rbs_to_be_setup_item_ies_o>>;

// UE-MulticastMRBs-ToBeSetup-atModify-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES
// : IEsSetParam}
using ue_multicast_m_rbs_to_be_setup_at_modify_list_l =
    dyn_array<protocol_ie_single_container_s<ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o>>;

// UE-associatedLogicalF1-ConnectionListResAck ::= SEQUENCE (SIZE (1..65536)) OF
// ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using ue_associated_lc_f1_conn_list_res_ack_l =
    dyn_array<protocol_ie_single_container_s<ue_associated_lc_f1_conn_item_res_ack_o>>;

// UEIdentity-List-For-Paging-List ::= SEQUENCE (SIZE (1..4096)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ue_id_list_for_paging_list_l = dyn_array<protocol_ie_single_container_s<ue_id_list_for_paging_item_ies_o>>;

// UL-UP-TNL-Address-to-Update-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ul_up_tnl_address_to_upd_list_l =
    bounded_array<protocol_ie_single_container_s<ul_up_tnl_address_to_upd_list_item_ies_o>, 8>;

// UL-UP-TNL-Information-to-Update-List ::= SEQUENCE (SIZE (1..32678)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ul_up_tnl_info_to_upd_list_l = dyn_array<protocol_ie_single_container_s<ul_up_tnl_info_to_upd_list_item_ies_o>>;

// F1RemovalFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_removal_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// F1RemovalRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_removal_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::transaction_id; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&       transaction_id() { return c; }
    const uint16_t& transaction_id() const { return c; }

  private:
    uint16_t c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// F1RemovalResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_removal_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                               type_;
    choice_buffer_t<crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// F1SetupFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// F1SetupRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        gnb_du_id,
        gnb_du_name,
        gnb_du_served_cells_list,
        gnb_du_rrc_version,
        transport_layer_address_info,
        bap_address,
        extended_gnb_du_name,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                                   transaction_id();
    uint64_t&                                   gnb_du_id();
    printable_string<1, 150, true, true>&       gnb_du_name();
    gnb_du_served_cells_list_l&                 gnb_du_served_cells_list();
    rrc_version_s&                              gnb_du_rrc_version();
    transport_layer_address_info_s&             transport_layer_address_info();
    fixed_bitstring<10, false, true>&           bap_address();
    extended_gnb_du_name_s&                     extended_gnb_du_name();
    const uint16_t&                             transaction_id() const;
    const uint64_t&                             gnb_du_id() const;
    const printable_string<1, 150, true, true>& gnb_du_name() const;
    const gnb_du_served_cells_list_l&           gnb_du_served_cells_list() const;
    const rrc_version_s&                        gnb_du_rrc_version() const;
    const transport_layer_address_info_s&       transport_layer_address_info() const;
    const fixed_bitstring<10, false, true>&     bap_address() const;
    const extended_gnb_du_name_s&               extended_gnb_du_name() const;

  private:
    types type_;
    choice_buffer_t<extended_gnb_du_name_s,
                    fixed_bitstring<10, false, true>,
                    gnb_du_served_cells_list_l,
                    printable_string<1, 150, true, true>,
                    rrc_version_s,
                    transport_layer_address_info_s>
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

// F1SetupResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        gnb_cu_name,
        cells_to_be_activ_list,
        gnb_cu_rrc_version,
        transport_layer_address_info,
        ul_bh_non_up_traffic_map,
        bap_address,
        extended_gnb_cu_name,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                                   transaction_id();
    printable_string<1, 150, true, true>&       gnb_cu_name();
    cells_to_be_activ_list_l&                   cells_to_be_activ_list();
    rrc_version_s&                              gnb_cu_rrc_version();
    transport_layer_address_info_s&             transport_layer_address_info();
    ul_bh_non_up_traffic_map_s&                 ul_bh_non_up_traffic_map();
    fixed_bitstring<10, false, true>&           bap_address();
    extended_gnb_cu_name_s&                     extended_gnb_cu_name();
    const uint16_t&                             transaction_id() const;
    const printable_string<1, 150, true, true>& gnb_cu_name() const;
    const cells_to_be_activ_list_l&             cells_to_be_activ_list() const;
    const rrc_version_s&                        gnb_cu_rrc_version() const;
    const transport_layer_address_info_s&       transport_layer_address_info() const;
    const ul_bh_non_up_traffic_map_s&           ul_bh_non_up_traffic_map() const;
    const fixed_bitstring<10, false, true>&     bap_address() const;
    const extended_gnb_cu_name_s&               extended_gnb_cu_name() const;

  private:
    types type_;
    choice_buffer_t<cells_to_be_activ_list_l,
                    extended_gnb_cu_name_s,
                    fixed_bitstring<10, false, true>,
                    printable_string<1, 150, true, true>,
                    rrc_version_s,
                    transport_layer_address_info_s,
                    ul_bh_non_up_traffic_map_s>
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

// GNBCUConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_cfg_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        cells_failed_to_be_activ_list,
        crit_diagnostics,
        gnb_cu_tnl_assoc_setup_list,
        gnb_cu_tnl_assoc_failed_to_setup_list,
        ded_si_delivery_needed_ue_list,
        transport_layer_address_info,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                                      transaction_id();
    cells_failed_to_be_activ_list_l&               cells_failed_to_be_activ_list();
    crit_diagnostics_s&                            crit_diagnostics();
    gnb_cu_tnl_assoc_setup_list_l&                 gnb_cu_tnl_assoc_setup_list();
    gnb_cu_tnl_assoc_failed_to_setup_list_l&       gnb_cu_tnl_assoc_failed_to_setup_list();
    ded_si_delivery_needed_ue_list_l&              ded_si_delivery_needed_ue_list();
    transport_layer_address_info_s&                transport_layer_address_info();
    const uint16_t&                                transaction_id() const;
    const cells_failed_to_be_activ_list_l&         cells_failed_to_be_activ_list() const;
    const crit_diagnostics_s&                      crit_diagnostics() const;
    const gnb_cu_tnl_assoc_setup_list_l&           gnb_cu_tnl_assoc_setup_list() const;
    const gnb_cu_tnl_assoc_failed_to_setup_list_l& gnb_cu_tnl_assoc_failed_to_setup_list() const;
    const ded_si_delivery_needed_ue_list_l&        ded_si_delivery_needed_ue_list() const;
    const transport_layer_address_info_s&          transport_layer_address_info() const;

  private:
    types type_;
    choice_buffer_t<cells_failed_to_be_activ_list_l,
                    crit_diagnostics_s,
                    ded_si_delivery_needed_ue_list_l,
                    gnb_cu_tnl_assoc_failed_to_setup_list_l,
                    gnb_cu_tnl_assoc_setup_list_l,
                    transport_layer_address_info_s>
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

// GNBCUConfigurationUpdateFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_cfg_upd_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNBCUConfigurationUpdateIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_cfg_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        cells_to_be_activ_list,
        cells_to_be_deactiv_list,
        gnb_cu_tnl_assoc_to_add_list,
        gnb_cu_tnl_assoc_to_rem_list,
        gnb_cu_tnl_assoc_to_upd_list,
        cells_to_be_barred_list,
        protected_eutra_res_list,
        neighbour_cell_info_list,
        transport_layer_address_info,
        ul_bh_non_up_traffic_map,
        bap_address,
        cco_assist_info,
        cells_for_son_list,
        gnb_cu_name,
        extended_gnb_cu_name,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                                   transaction_id();
    cells_to_be_activ_list_l&                   cells_to_be_activ_list();
    cells_to_be_deactiv_list_l&                 cells_to_be_deactiv_list();
    gnb_cu_tnl_assoc_to_add_list_l&             gnb_cu_tnl_assoc_to_add_list();
    gnb_cu_tnl_assoc_to_rem_list_l&             gnb_cu_tnl_assoc_to_rem_list();
    gnb_cu_tnl_assoc_to_upd_list_l&             gnb_cu_tnl_assoc_to_upd_list();
    cells_to_be_barred_list_l&                  cells_to_be_barred_list();
    protected_eutra_res_list_l&                 protected_eutra_res_list();
    neighbour_cell_info_list_l&                 neighbour_cell_info_list();
    transport_layer_address_info_s&             transport_layer_address_info();
    ul_bh_non_up_traffic_map_s&                 ul_bh_non_up_traffic_map();
    fixed_bitstring<10, false, true>&           bap_address();
    cco_assist_info_s&                          cco_assist_info();
    cells_for_son_list_l&                       cells_for_son_list();
    printable_string<1, 150, true, true>&       gnb_cu_name();
    extended_gnb_cu_name_s&                     extended_gnb_cu_name();
    const uint16_t&                             transaction_id() const;
    const cells_to_be_activ_list_l&             cells_to_be_activ_list() const;
    const cells_to_be_deactiv_list_l&           cells_to_be_deactiv_list() const;
    const gnb_cu_tnl_assoc_to_add_list_l&       gnb_cu_tnl_assoc_to_add_list() const;
    const gnb_cu_tnl_assoc_to_rem_list_l&       gnb_cu_tnl_assoc_to_rem_list() const;
    const gnb_cu_tnl_assoc_to_upd_list_l&       gnb_cu_tnl_assoc_to_upd_list() const;
    const cells_to_be_barred_list_l&            cells_to_be_barred_list() const;
    const protected_eutra_res_list_l&           protected_eutra_res_list() const;
    const neighbour_cell_info_list_l&           neighbour_cell_info_list() const;
    const transport_layer_address_info_s&       transport_layer_address_info() const;
    const ul_bh_non_up_traffic_map_s&           ul_bh_non_up_traffic_map() const;
    const fixed_bitstring<10, false, true>&     bap_address() const;
    const cco_assist_info_s&                    cco_assist_info() const;
    const cells_for_son_list_l&                 cells_for_son_list() const;
    const printable_string<1, 150, true, true>& gnb_cu_name() const;
    const extended_gnb_cu_name_s&               extended_gnb_cu_name() const;

  private:
    types type_;
    choice_buffer_t<cco_assist_info_s,
                    cells_for_son_list_l,
                    cells_to_be_activ_list_l,
                    cells_to_be_barred_list_l,
                    cells_to_be_deactiv_list_l,
                    extended_gnb_cu_name_s,
                    fixed_bitstring<10, false, true>,
                    gnb_cu_tnl_assoc_to_add_list_l,
                    gnb_cu_tnl_assoc_to_rem_list_l,
                    gnb_cu_tnl_assoc_to_upd_list_l,
                    neighbour_cell_info_list_l,
                    printable_string<1, 150, true, true>,
                    protected_eutra_res_list_l,
                    transport_layer_address_info_s,
                    ul_bh_non_up_traffic_map_s>
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

// GNBDUConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_cfg_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        cells_to_be_activ_list,
        crit_diagnostics,
        cells_to_be_deactiv_list,
        transport_layer_address_info,
        ul_bh_non_up_traffic_map,
        bap_address,
        cells_for_son_list,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                               transaction_id();
    cells_to_be_activ_list_l&               cells_to_be_activ_list();
    crit_diagnostics_s&                     crit_diagnostics();
    cells_to_be_deactiv_list_l&             cells_to_be_deactiv_list();
    transport_layer_address_info_s&         transport_layer_address_info();
    ul_bh_non_up_traffic_map_s&             ul_bh_non_up_traffic_map();
    fixed_bitstring<10, false, true>&       bap_address();
    cells_for_son_list_l&                   cells_for_son_list();
    const uint16_t&                         transaction_id() const;
    const cells_to_be_activ_list_l&         cells_to_be_activ_list() const;
    const crit_diagnostics_s&               crit_diagnostics() const;
    const cells_to_be_deactiv_list_l&       cells_to_be_deactiv_list() const;
    const transport_layer_address_info_s&   transport_layer_address_info() const;
    const ul_bh_non_up_traffic_map_s&       ul_bh_non_up_traffic_map() const;
    const fixed_bitstring<10, false, true>& bap_address() const;
    const cells_for_son_list_l&             cells_for_son_list() const;

  private:
    types type_;
    choice_buffer_t<cells_for_son_list_l,
                    cells_to_be_activ_list_l,
                    cells_to_be_deactiv_list_l,
                    crit_diagnostics_s,
                    fixed_bitstring<10, false, true>,
                    transport_layer_address_info_s,
                    ul_bh_non_up_traffic_map_s>
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

// GNBDUConfigurationUpdateFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_cfg_upd_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNBDUConfigurationUpdateIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_cfg_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        served_cells_to_add_list,
        served_cells_to_modify_list,
        served_cells_to_delete_list,
        cells_status_list,
        ded_si_delivery_needed_ue_list,
        gnb_du_id,
        gnb_du_tnl_assoc_to_rem_list,
        transport_layer_address_info,
        coverage_mod_notif,
        gnb_du_name,
        extended_gnb_du_name,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                                   transaction_id();
    served_cells_to_add_list_l&                 served_cells_to_add_list();
    served_cells_to_modify_list_l&              served_cells_to_modify_list();
    served_cells_to_delete_list_l&              served_cells_to_delete_list();
    cells_status_list_l&                        cells_status_list();
    ded_si_delivery_needed_ue_list_l&           ded_si_delivery_needed_ue_list();
    uint64_t&                                   gnb_du_id();
    gnb_du_tnl_assoc_to_rem_list_l&             gnb_du_tnl_assoc_to_rem_list();
    transport_layer_address_info_s&             transport_layer_address_info();
    coverage_mod_notif_s&                       coverage_mod_notif();
    printable_string<1, 150, true, true>&       gnb_du_name();
    extended_gnb_du_name_s&                     extended_gnb_du_name();
    const uint16_t&                             transaction_id() const;
    const served_cells_to_add_list_l&           served_cells_to_add_list() const;
    const served_cells_to_modify_list_l&        served_cells_to_modify_list() const;
    const served_cells_to_delete_list_l&        served_cells_to_delete_list() const;
    const cells_status_list_l&                  cells_status_list() const;
    const ded_si_delivery_needed_ue_list_l&     ded_si_delivery_needed_ue_list() const;
    const uint64_t&                             gnb_du_id() const;
    const gnb_du_tnl_assoc_to_rem_list_l&       gnb_du_tnl_assoc_to_rem_list() const;
    const transport_layer_address_info_s&       transport_layer_address_info() const;
    const coverage_mod_notif_s&                 coverage_mod_notif() const;
    const printable_string<1, 150, true, true>& gnb_du_name() const;
    const extended_gnb_du_name_s&               extended_gnb_du_name() const;

  private:
    types type_;
    choice_buffer_t<cells_status_list_l,
                    coverage_mod_notif_s,
                    ded_si_delivery_needed_ue_list_l,
                    extended_gnb_du_name_s,
                    gnb_du_tnl_assoc_to_rem_list_l,
                    printable_string<1, 150, true, true>,
                    served_cells_to_add_list_l,
                    served_cells_to_delete_list_l,
                    served_cells_to_modify_list_l,
                    transport_layer_address_info_s>
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

// GNBDUResourceConfigurationAcknowledgeIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_res_cfg_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                               type_;
    choice_buffer_t<crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNBDUResourceConfigurationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_res_cfg_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNBDUResourceConfigurationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_res_cfg_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        activ_cells_to_be_upd_list,
        child_nodes_list,
        neighbour_node_cells_list,
        serving_cells_list,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                           transaction_id();
    activ_cells_to_be_upd_list_l&       activ_cells_to_be_upd_list();
    child_nodes_list_l&                 child_nodes_list();
    neighbour_node_cells_list_l&        neighbour_node_cells_list();
    serving_cells_list_l&               serving_cells_list();
    const uint16_t&                     transaction_id() const;
    const activ_cells_to_be_upd_list_l& activ_cells_to_be_upd_list() const;
    const child_nodes_list_l&           child_nodes_list() const;
    const neighbour_node_cells_list_l&  neighbour_node_cells_list() const;
    const serving_cells_list_l&         serving_cells_list() const;

  private:
    types type_;
    choice_buffer_t<activ_cells_to_be_upd_list_l, child_nodes_list_l, neighbour_node_cells_list_l, serving_cells_list_l>
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

// GNBDUResourceCoordinationRequest-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_res_coordination_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        request_type,
        eutra_nr_cell_res_coordination_req_container,
        ignore_res_coordination_container,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                                  transaction_id();
    request_type_e&                            request_type();
    unbounded_octstring<true>&                 eutra_nr_cell_res_coordination_req_container();
    ignore_res_coordination_container_e&       ignore_res_coordination_container();
    const uint16_t&                            transaction_id() const;
    const request_type_e&                      request_type() const;
    const unbounded_octstring<true>&           eutra_nr_cell_res_coordination_req_container() const;
    const ignore_res_coordination_container_e& ignore_res_coordination_container() const;

  private:
    types                                      type_;
    choice_buffer_t<unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNBDUResourceCoordinationResponse-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_res_coordination_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, eutra_nr_cell_res_coordination_req_ack_container, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                        transaction_id();
    unbounded_octstring<true>&       eutra_nr_cell_res_coordination_req_ack_container();
    const uint16_t&                  transaction_id() const;
    const unbounded_octstring<true>& eutra_nr_cell_res_coordination_req_ack_container() const;

  private:
    types                                      type_;
    choice_buffer_t<unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNBDUStatusIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_status_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, gnb_du_overload_info, iab_congestion_ind, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                     transaction_id();
    gnb_du_overload_info_e&       gnb_du_overload_info();
    iab_congestion_ind_s&         iab_congestion_ind();
    const uint16_t&               transaction_id() const;
    const gnb_du_overload_info_e& gnb_du_overload_info() const;
    const iab_congestion_ind_s&   iab_congestion_ind() const;

  private:
    types                                 type_;
    choice_buffer_t<iab_congestion_ind_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// IABTNLAddressFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct iab_tnl_address_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// IABTNLAddressRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct iab_tnl_address_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        ia_bv4_addresses_requested,
        iab_ip_v6_request_type,
        iab_tnl_addresses_to_rem_list,
        iab_tnl_addresses_exception,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                              transaction_id();
    ia_bv4_addresses_requested_s&          ia_bv4_addresses_requested();
    iab_ip_v6_request_type_c&              iab_ip_v6_request_type();
    iab_tnl_addresses_to_rem_list_l&       iab_tnl_addresses_to_rem_list();
    iab_tnl_addresses_exception_s&         iab_tnl_addresses_exception();
    const uint16_t&                        transaction_id() const;
    const ia_bv4_addresses_requested_s&    ia_bv4_addresses_requested() const;
    const iab_ip_v6_request_type_c&        iab_ip_v6_request_type() const;
    const iab_tnl_addresses_to_rem_list_l& iab_tnl_addresses_to_rem_list() const;
    const iab_tnl_addresses_exception_s&   iab_tnl_addresses_exception() const;

  private:
    types type_;
    choice_buffer_t<ia_bv4_addresses_requested_s,
                    iab_ip_v6_request_type_c,
                    iab_tnl_addresses_exception_s,
                    iab_tnl_addresses_to_rem_list_l>
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

// IABTNLAddressResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct iab_tnl_address_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, iab_allocated_tnl_address_list, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                               transaction_id();
    iab_allocated_tnl_address_list_l&       iab_allocated_tnl_address_list();
    const uint16_t&                         transaction_id() const;
    const iab_allocated_tnl_address_list_l& iab_allocated_tnl_address_list() const;

  private:
    types                                             type_;
    choice_buffer_t<iab_allocated_tnl_address_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// IABUPConfigurationUpdateFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct iab_up_cfg_upd_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// IABUPConfigurationUpdateRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct iab_up_cfg_upd_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, ul_up_tnl_info_to_upd_list, ul_up_tnl_address_to_upd_list, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                              transaction_id();
    ul_up_tnl_info_to_upd_list_l&          ul_up_tnl_info_to_upd_list();
    ul_up_tnl_address_to_upd_list_l&       ul_up_tnl_address_to_upd_list();
    const uint16_t&                        transaction_id() const;
    const ul_up_tnl_info_to_upd_list_l&    ul_up_tnl_info_to_upd_list() const;
    const ul_up_tnl_address_to_upd_list_l& ul_up_tnl_address_to_upd_list() const;

  private:
    types                                                                          type_;
    choice_buffer_t<ul_up_tnl_address_to_upd_list_l, ul_up_tnl_info_to_upd_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// IABUPConfigurationUpdateResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct iab_up_cfg_upd_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, crit_diagnostics, dl_up_tnl_address_to_upd_list, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                              transaction_id();
    crit_diagnostics_s&                    crit_diagnostics();
    dl_up_tnl_address_to_upd_list_l&       dl_up_tnl_address_to_upd_list();
    const uint16_t&                        transaction_id() const;
    const crit_diagnostics_s&              crit_diagnostics() const;
    const dl_up_tnl_address_to_upd_list_l& dl_up_tnl_address_to_upd_list() const;

  private:
    types                                                                type_;
    choice_buffer_t<crit_diagnostics_s, dl_up_tnl_address_to_upd_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// InitialULRRCMessageTransferIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct init_ul_rrc_msg_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_du_ue_f1ap_id,
        nr_cgi,
        c_rnti,
        rrc_container,
        du_to_cu_rrc_container,
        sul_access_ind,
        transaction_id,
        ran_ue_id,
        rrc_container_rrc_setup_complete,
        nr_red_cap_ue_ind,
        sdt_info,
        sidelink_relay_cfg,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                        gnb_du_ue_f1ap_id();
    nr_cgi_s&                        nr_cgi();
    uint32_t&                        c_rnti();
    unbounded_octstring<true>&       rrc_container();
    unbounded_octstring<true>&       du_to_cu_rrc_container();
    sul_access_ind_e&                sul_access_ind();
    uint16_t&                        transaction_id();
    fixed_octstring<8, true>&        ran_ue_id();
    unbounded_octstring<true>&       rrc_container_rrc_setup_complete();
    nr_red_cap_ue_ind_e&             nr_red_cap_ue_ind();
    sdt_info_s&                      sdt_info();
    sidelink_relay_cfg_s&            sidelink_relay_cfg();
    const uint64_t&                  gnb_du_ue_f1ap_id() const;
    const nr_cgi_s&                  nr_cgi() const;
    const uint32_t&                  c_rnti() const;
    const unbounded_octstring<true>& rrc_container() const;
    const unbounded_octstring<true>& du_to_cu_rrc_container() const;
    const sul_access_ind_e&          sul_access_ind() const;
    const uint16_t&                  transaction_id() const;
    const fixed_octstring<8, true>&  ran_ue_id() const;
    const unbounded_octstring<true>& rrc_container_rrc_setup_complete() const;
    const nr_red_cap_ue_ind_e&       nr_red_cap_ue_ind() const;
    const sdt_info_s&                sdt_info() const;
    const sidelink_relay_cfg_s&      sidelink_relay_cfg() const;

  private:
    types type_;
    choice_buffer_t<fixed_octstring<8, true>, nr_cgi_s, sdt_info_s, sidelink_relay_cfg_s, unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MeasurementActivation-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct meas_activation_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        activation_request_type,
        prs_meas_info_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                        gnb_cu_ue_f1ap_id();
    uint64_t&                        gnb_du_ue_f1ap_id();
    activation_request_type_e&       activation_request_type();
    prs_meas_info_list_l&            prs_meas_info_list();
    const uint64_t&                  gnb_cu_ue_f1ap_id() const;
    const uint64_t&                  gnb_du_ue_f1ap_id() const;
    const activation_request_type_e& activation_request_type() const;
    const prs_meas_info_list_l&      prs_meas_info_list() const;

  private:
    types                                 type_;
    choice_buffer_t<prs_meas_info_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MeasurementPreconfigurationConfirm-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct meas_precfg_confirm_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        pos_meas_gap_pre_cfg_list,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                          gnb_cu_ue_f1ap_id();
    uint64_t&                          gnb_du_ue_f1ap_id();
    pos_meas_gap_pre_cfg_list_s&       pos_meas_gap_pre_cfg_list();
    crit_diagnostics_s&                crit_diagnostics();
    const uint64_t&                    gnb_cu_ue_f1ap_id() const;
    const uint64_t&                    gnb_du_ue_f1ap_id() const;
    const pos_meas_gap_pre_cfg_list_s& pos_meas_gap_pre_cfg_list() const;
    const crit_diagnostics_s&          crit_diagnostics() const;

  private:
    types                                                            type_;
    choice_buffer_t<crit_diagnostics_s, pos_meas_gap_pre_cfg_list_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MeasurementPreconfigurationRefuse-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct meas_precfg_refuse_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MeasurementPreconfigurationRequired-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct meas_precfg_required_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, trp_prs_info_list, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                  gnb_cu_ue_f1ap_id();
    uint64_t&                  gnb_du_ue_f1ap_id();
    trp_prs_info_list_l&       trp_prs_info_list();
    const uint64_t&            gnb_cu_ue_f1ap_id() const;
    const uint64_t&            gnb_du_ue_f1ap_id() const;
    const trp_prs_info_list_l& trp_prs_info_list() const;

  private:
    types                                type_;
    choice_buffer_t<trp_prs_info_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastContextModificationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_mod_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_mbs_f1ap_id, gnb_du_mbs_f1ap_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_mbs_f1ap_id();
    uint64_t&                 gnb_du_mbs_f1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_mbs_f1ap_id() const;
    const uint64_t&           gnb_du_mbs_f1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastContextModificationRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_mod_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_mbs_f1ap_id,
        gnb_du_mbs_f1ap_id,
        mbs_service_area,
        multicast_m_rbs_to_be_setup_mod_list,
        multicast_m_rbs_to_be_modified_list,
        multicast_m_rbs_to_be_released_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                     gnb_cu_mbs_f1ap_id();
    uint64_t&                                     gnb_du_mbs_f1ap_id();
    mbs_service_area_c&                           mbs_service_area();
    multicast_m_rbs_to_be_setup_mod_list_l&       multicast_m_rbs_to_be_setup_mod_list();
    multicast_m_rbs_to_be_modified_list_l&        multicast_m_rbs_to_be_modified_list();
    multicast_m_rbs_to_be_released_list_l&        multicast_m_rbs_to_be_released_list();
    const uint64_t&                               gnb_cu_mbs_f1ap_id() const;
    const uint64_t&                               gnb_du_mbs_f1ap_id() const;
    const mbs_service_area_c&                     mbs_service_area() const;
    const multicast_m_rbs_to_be_setup_mod_list_l& multicast_m_rbs_to_be_setup_mod_list() const;
    const multicast_m_rbs_to_be_modified_list_l&  multicast_m_rbs_to_be_modified_list() const;
    const multicast_m_rbs_to_be_released_list_l&  multicast_m_rbs_to_be_released_list() const;

  private:
    types type_;
    choice_buffer_t<mbs_service_area_c,
                    multicast_m_rbs_to_be_modified_list_l,
                    multicast_m_rbs_to_be_released_list_l,
                    multicast_m_rbs_to_be_setup_mod_list_l>
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

// MulticastContextModificationResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_mod_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_mbs_f1ap_id,
        gnb_du_mbs_f1ap_id,
        multicast_m_rbs_setup_mod_list,
        multicast_m_rbs_failed_to_be_setup_mod_list,
        multicast_m_rbs_modified_list,
        multicast_m_rbs_failed_to_be_modified_list,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                            gnb_cu_mbs_f1ap_id();
    uint64_t&                                            gnb_du_mbs_f1ap_id();
    multicast_m_rbs_setup_mod_list_l&                    multicast_m_rbs_setup_mod_list();
    multicast_m_rbs_failed_to_be_setup_mod_list_l&       multicast_m_rbs_failed_to_be_setup_mod_list();
    multicast_m_rbs_modified_list_l&                     multicast_m_rbs_modified_list();
    multicast_m_rbs_failed_to_be_modified_list_l&        multicast_m_rbs_failed_to_be_modified_list();
    crit_diagnostics_s&                                  crit_diagnostics();
    const uint64_t&                                      gnb_cu_mbs_f1ap_id() const;
    const uint64_t&                                      gnb_du_mbs_f1ap_id() const;
    const multicast_m_rbs_setup_mod_list_l&              multicast_m_rbs_setup_mod_list() const;
    const multicast_m_rbs_failed_to_be_setup_mod_list_l& multicast_m_rbs_failed_to_be_setup_mod_list() const;
    const multicast_m_rbs_modified_list_l&               multicast_m_rbs_modified_list() const;
    const multicast_m_rbs_failed_to_be_modified_list_l&  multicast_m_rbs_failed_to_be_modified_list() const;
    const crit_diagnostics_s&                            crit_diagnostics() const;

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    multicast_m_rbs_failed_to_be_modified_list_l,
                    multicast_m_rbs_failed_to_be_setup_mod_list_l,
                    multicast_m_rbs_modified_list_l,
                    multicast_m_rbs_setup_mod_list_l>
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

// MulticastContextReleaseCommandIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_release_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_mbs_f1ap_id, gnb_du_mbs_f1ap_id, cause, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&       gnb_cu_mbs_f1ap_id();
    uint64_t&       gnb_du_mbs_f1ap_id();
    cause_c&        cause();
    const uint64_t& gnb_cu_mbs_f1ap_id() const;
    const uint64_t& gnb_du_mbs_f1ap_id() const;
    const cause_c&  cause() const;

  private:
    types                    type_;
    choice_buffer_t<cause_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastContextReleaseCompleteIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_release_complete_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_mbs_f1ap_id, gnb_du_mbs_f1ap_id, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_mbs_f1ap_id();
    uint64_t&                 gnb_du_mbs_f1ap_id();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_mbs_f1ap_id() const;
    const uint64_t&           gnb_du_mbs_f1ap_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                               type_;
    choice_buffer_t<crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastContextReleaseRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_release_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_mbs_f1ap_id, gnb_du_mbs_f1ap_id, cause, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&       gnb_cu_mbs_f1ap_id();
    uint64_t&       gnb_du_mbs_f1ap_id();
    cause_c&        cause();
    const uint64_t& gnb_cu_mbs_f1ap_id() const;
    const uint64_t& gnb_du_mbs_f1ap_id() const;
    const cause_c&  cause() const;

  private:
    types                    type_;
    choice_buffer_t<cause_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastContextSetupFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_mbs_f1ap_id, gnb_du_mbs_f1ap_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_mbs_f1ap_id();
    uint64_t&                 gnb_du_mbs_f1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_mbs_f1ap_id() const;
    const uint64_t&           gnb_du_mbs_f1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastContextSetupRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_mbs_f1ap_id,
        mbs_session_id,
        mbs_service_area,
        snssai,
        multicast_m_rbs_to_be_setup_list,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                 gnb_cu_mbs_f1ap_id();
    mbs_session_id_s&                         mbs_session_id();
    mbs_service_area_c&                       mbs_service_area();
    snssai_s&                                 snssai();
    multicast_m_rbs_to_be_setup_list_l&       multicast_m_rbs_to_be_setup_list();
    const uint64_t&                           gnb_cu_mbs_f1ap_id() const;
    const mbs_session_id_s&                   mbs_session_id() const;
    const mbs_service_area_c&                 mbs_service_area() const;
    const snssai_s&                           snssai() const;
    const multicast_m_rbs_to_be_setup_list_l& multicast_m_rbs_to_be_setup_list() const;

  private:
    types                                                                                               type_;
    choice_buffer_t<mbs_service_area_c, mbs_session_id_s, multicast_m_rbs_to_be_setup_list_l, snssai_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastContextSetupResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_mbs_f1ap_id,
        gnb_du_mbs_f1ap_id,
        multicast_m_rbs_setup_list,
        multicast_m_rbs_failed_to_be_setup_list,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                        gnb_cu_mbs_f1ap_id();
    uint64_t&                                        gnb_du_mbs_f1ap_id();
    multicast_m_rbs_setup_list_l&                    multicast_m_rbs_setup_list();
    multicast_m_rbs_failed_to_be_setup_list_l&       multicast_m_rbs_failed_to_be_setup_list();
    crit_diagnostics_s&                              crit_diagnostics();
    const uint64_t&                                  gnb_cu_mbs_f1ap_id() const;
    const uint64_t&                                  gnb_du_mbs_f1ap_id() const;
    const multicast_m_rbs_setup_list_l&              multicast_m_rbs_setup_list() const;
    const multicast_m_rbs_failed_to_be_setup_list_l& multicast_m_rbs_failed_to_be_setup_list() const;
    const crit_diagnostics_s&                        crit_diagnostics() const;

  private:
    types                                                                                                        type_;
    choice_buffer_t<crit_diagnostics_s, multicast_m_rbs_failed_to_be_setup_list_l, multicast_m_rbs_setup_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastDistributionReleaseCommandIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_distribution_release_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_mbs_f1ap_id,
        gnb_du_mbs_f1ap_id,
        mbs_multicast_f1_u_context_descriptor,
        cause,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                      gnb_cu_mbs_f1ap_id();
    uint64_t&                                      gnb_du_mbs_f1ap_id();
    mbs_multicast_f1_u_context_descriptor_s&       mbs_multicast_f1_u_context_descriptor();
    cause_c&                                       cause();
    const uint64_t&                                gnb_cu_mbs_f1ap_id() const;
    const uint64_t&                                gnb_du_mbs_f1ap_id() const;
    const mbs_multicast_f1_u_context_descriptor_s& mbs_multicast_f1_u_context_descriptor() const;
    const cause_c&                                 cause() const;

  private:
    types                                                             type_;
    choice_buffer_t<cause_c, mbs_multicast_f1_u_context_descriptor_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastDistributionReleaseCompleteIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_distribution_release_complete_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_mbs_f1ap_id,
        gnb_du_mbs_f1ap_id,
        mbs_multicast_f1_u_context_descriptor,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                      gnb_cu_mbs_f1ap_id();
    uint64_t&                                      gnb_du_mbs_f1ap_id();
    mbs_multicast_f1_u_context_descriptor_s&       mbs_multicast_f1_u_context_descriptor();
    crit_diagnostics_s&                            crit_diagnostics();
    const uint64_t&                                gnb_cu_mbs_f1ap_id() const;
    const uint64_t&                                gnb_du_mbs_f1ap_id() const;
    const mbs_multicast_f1_u_context_descriptor_s& mbs_multicast_f1_u_context_descriptor() const;
    const crit_diagnostics_s&                      crit_diagnostics() const;

  private:
    types                                                                        type_;
    choice_buffer_t<crit_diagnostics_s, mbs_multicast_f1_u_context_descriptor_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastDistributionSetupFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_distribution_setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_mbs_f1ap_id,
        gnb_du_mbs_f1ap_id,
        mbs_multicast_f1_u_context_descriptor,
        cause,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                      gnb_cu_mbs_f1ap_id();
    uint64_t&                                      gnb_du_mbs_f1ap_id();
    mbs_multicast_f1_u_context_descriptor_s&       mbs_multicast_f1_u_context_descriptor();
    cause_c&                                       cause();
    crit_diagnostics_s&                            crit_diagnostics();
    const uint64_t&                                gnb_cu_mbs_f1ap_id() const;
    const uint64_t&                                gnb_du_mbs_f1ap_id() const;
    const mbs_multicast_f1_u_context_descriptor_s& mbs_multicast_f1_u_context_descriptor() const;
    const cause_c&                                 cause() const;
    const crit_diagnostics_s&                      crit_diagnostics() const;

  private:
    types                                                                                 type_;
    choice_buffer_t<cause_c, crit_diagnostics_s, mbs_multicast_f1_u_context_descriptor_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastDistributionSetupRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_distribution_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_mbs_f1ap_id,
        gnb_du_mbs_f1ap_id,
        mbs_multicast_f1_u_context_descriptor,
        multicast_f1_u_context_to_be_setup_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                        gnb_cu_mbs_f1ap_id();
    uint64_t&                                        gnb_du_mbs_f1ap_id();
    mbs_multicast_f1_u_context_descriptor_s&         mbs_multicast_f1_u_context_descriptor();
    multicast_f1_u_context_to_be_setup_list_l&       multicast_f1_u_context_to_be_setup_list();
    const uint64_t&                                  gnb_cu_mbs_f1ap_id() const;
    const uint64_t&                                  gnb_du_mbs_f1ap_id() const;
    const mbs_multicast_f1_u_context_descriptor_s&   mbs_multicast_f1_u_context_descriptor() const;
    const multicast_f1_u_context_to_be_setup_list_l& multicast_f1_u_context_to_be_setup_list() const;

  private:
    types                                                                                               type_;
    choice_buffer_t<mbs_multicast_f1_u_context_descriptor_s, multicast_f1_u_context_to_be_setup_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastDistributionSetupResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_distribution_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_mbs_f1ap_id,
        gnb_du_mbs_f1ap_id,
        mbs_multicast_f1_u_context_descriptor,
        multicast_f1_u_context_setup_list,
        multicast_f1_u_context_failed_to_be_setup_list,
        crit_diagnostics,
        multicast_f1_u_context_ref_cu,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                               gnb_cu_mbs_f1ap_id();
    uint64_t&                                               gnb_du_mbs_f1ap_id();
    mbs_multicast_f1_u_context_descriptor_s&                mbs_multicast_f1_u_context_descriptor();
    multicast_f1_u_context_setup_list_l&                    multicast_f1_u_context_setup_list();
    multicast_f1_u_context_failed_to_be_setup_list_l&       multicast_f1_u_context_failed_to_be_setup_list();
    crit_diagnostics_s&                                     crit_diagnostics();
    fixed_octstring<4, true>&                               multicast_f1_u_context_ref_cu();
    const uint64_t&                                         gnb_cu_mbs_f1ap_id() const;
    const uint64_t&                                         gnb_du_mbs_f1ap_id() const;
    const mbs_multicast_f1_u_context_descriptor_s&          mbs_multicast_f1_u_context_descriptor() const;
    const multicast_f1_u_context_setup_list_l&              multicast_f1_u_context_setup_list() const;
    const multicast_f1_u_context_failed_to_be_setup_list_l& multicast_f1_u_context_failed_to_be_setup_list() const;
    const crit_diagnostics_s&                               crit_diagnostics() const;
    const fixed_octstring<4, true>&                         multicast_f1_u_context_ref_cu() const;

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    fixed_octstring<4, true>,
                    mbs_multicast_f1_u_context_descriptor_s,
                    multicast_f1_u_context_failed_to_be_setup_list_l,
                    multicast_f1_u_context_setup_list_l>
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

// MulticastGroupPagingIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_group_paging_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mbs_session_id, ue_id_list_for_paging_list, mc_paging_cell_list, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    mbs_session_id_s&                   mbs_session_id();
    ue_id_list_for_paging_list_l&       ue_id_list_for_paging_list();
    mc_paging_cell_list_l&              mc_paging_cell_list();
    const mbs_session_id_s&             mbs_session_id() const;
    const ue_id_list_for_paging_list_l& ue_id_list_for_paging_list() const;
    const mc_paging_cell_list_l&        mc_paging_cell_list() const;

  private:
    types                                                                                  type_;
    choice_buffer_t<mbs_session_id_s, mc_paging_cell_list_l, ue_id_list_for_paging_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NetworkAccessRateReductionIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct network_access_rate_reduction_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, uac_assist_info, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                transaction_id();
    uac_assist_info_s&       uac_assist_info();
    const uint16_t&          transaction_id() const;
    const uac_assist_info_s& uac_assist_info() const;

  private:
    types                              type_;
    choice_buffer_t<uac_assist_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NotifyIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct notify_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, drb_notify_list, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                gnb_cu_ue_f1ap_id();
    uint64_t&                gnb_du_ue_f1ap_id();
    drb_notify_list_l&       drb_notify_list();
    const uint64_t&          gnb_cu_ue_f1ap_id() const;
    const uint64_t&          gnb_du_ue_f1ap_id() const;
    const drb_notify_list_l& drb_notify_list() const;

  private:
    types                              type_;
    choice_buffer_t<drb_notify_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDCMeasurementFailureIndication-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pdc_meas_fail_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, ran_ue_pdc_meas_id, cause, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&       gnb_cu_ue_f1ap_id();
    uint64_t&       gnb_du_ue_f1ap_id();
    uint8_t&        ran_ue_pdc_meas_id();
    cause_c&        cause();
    const uint64_t& gnb_cu_ue_f1ap_id() const;
    const uint64_t& gnb_du_ue_f1ap_id() const;
    const uint8_t&  ran_ue_pdc_meas_id() const;
    const cause_c&  cause() const;

  private:
    types                    type_;
    choice_buffer_t<cause_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDCMeasurementInitiationFailure-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pdc_meas_initiation_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        ran_ue_pdc_meas_id,
        cause,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    uint8_t&                  ran_ue_pdc_meas_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
    const uint8_t&            ran_ue_pdc_meas_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDCMeasurementInitiationRequest-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pdc_meas_initiation_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        ran_ue_pdc_meas_id,
        pdc_report_type,
        pdc_meas_periodicity,
        pdc_meas_quantities,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                     gnb_cu_ue_f1ap_id();
    uint64_t&                     gnb_du_ue_f1ap_id();
    uint8_t&                      ran_ue_pdc_meas_id();
    pdc_report_type_e&            pdc_report_type();
    pdc_meas_periodicity_e&       pdc_meas_periodicity();
    pdc_meas_quantities_l&        pdc_meas_quantities();
    const uint64_t&               gnb_cu_ue_f1ap_id() const;
    const uint64_t&               gnb_du_ue_f1ap_id() const;
    const uint8_t&                ran_ue_pdc_meas_id() const;
    const pdc_report_type_e&      pdc_report_type() const;
    const pdc_meas_periodicity_e& pdc_meas_periodicity() const;
    const pdc_meas_quantities_l&  pdc_meas_quantities() const;

  private:
    types                                  type_;
    choice_buffer_t<pdc_meas_quantities_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDCMeasurementInitiationResponse-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pdc_meas_initiation_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        ran_ue_pdc_meas_id,
        pdc_meas_result,
        crit_diagnostics,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    uint8_t&                  ran_ue_pdc_meas_id();
    pdc_meas_result_s&        pdc_meas_result();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
    const uint8_t&            ran_ue_pdc_meas_id() const;
    const pdc_meas_result_s&  pdc_meas_result() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                                  type_;
    choice_buffer_t<crit_diagnostics_s, pdc_meas_result_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDCMeasurementReport-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pdc_meas_report_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, ran_ue_pdc_meas_id, pdc_meas_result, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                gnb_cu_ue_f1ap_id();
    uint64_t&                gnb_du_ue_f1ap_id();
    uint8_t&                 ran_ue_pdc_meas_id();
    pdc_meas_result_s&       pdc_meas_result();
    const uint64_t&          gnb_cu_ue_f1ap_id() const;
    const uint64_t&          gnb_du_ue_f1ap_id() const;
    const uint8_t&           ran_ue_pdc_meas_id() const;
    const pdc_meas_result_s& pdc_meas_result() const;

  private:
    types                              type_;
    choice_buffer_t<pdc_meas_result_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PDCMeasurementTerminationCommand-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pdc_meas_termination_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, ran_ue_pdc_meas_id, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&       gnb_cu_ue_f1ap_id();
    uint64_t&       gnb_du_ue_f1ap_id();
    uint8_t&        ran_ue_pdc_meas_id();
    const uint64_t& gnb_cu_ue_f1ap_id() const;
    const uint64_t& gnb_du_ue_f1ap_id() const;
    const uint8_t&  ran_ue_pdc_meas_id() const;

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PRSConfigurationFailure-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct prs_cfg_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PRSConfigurationRequest-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct prs_cfg_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { prs_cfg_request_type, prstrp_list, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    prs_cfg_request_type_e&       prs_cfg_request_type();
    prstrp_list_l&                prstrp_list();
    const prs_cfg_request_type_e& prs_cfg_request_type() const;
    const prstrp_list_l&          prstrp_list() const;

  private:
    types                          type_;
    choice_buffer_t<prstrp_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PRSConfigurationResponse-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct prs_cfg_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, prs_tx_trp_list, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    prs_tx_trp_list_l&        prs_tx_trp_list();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const prs_tx_trp_list_l&  prs_tx_trp_list() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                                  type_;
    choice_buffer_t<crit_diagnostics_s, prs_tx_trp_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSCancelRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pws_cancel_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        numof_broadcast_request,
        broadcast_to_be_cancelled_list,
        cancel_all_warning_msgs_ind,
        notif_info,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                               transaction_id();
    uint32_t&                               numof_broadcast_request();
    broadcast_to_be_cancelled_list_l&       broadcast_to_be_cancelled_list();
    cancel_all_warning_msgs_ind_e&          cancel_all_warning_msgs_ind();
    notif_info_s&                           notif_info();
    const uint16_t&                         transaction_id() const;
    const uint32_t&                         numof_broadcast_request() const;
    const broadcast_to_be_cancelled_list_l& broadcast_to_be_cancelled_list() const;
    const cancel_all_warning_msgs_ind_e&    cancel_all_warning_msgs_ind() const;
    const notif_info_s&                     notif_info() const;

  private:
    types                                                           type_;
    choice_buffer_t<broadcast_to_be_cancelled_list_l, notif_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSCancelResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pws_cancel_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cells_broadcast_cancelled_list, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                               transaction_id();
    cells_broadcast_cancelled_list_l&       cells_broadcast_cancelled_list();
    crit_diagnostics_s&                     crit_diagnostics();
    const uint16_t&                         transaction_id() const;
    const cells_broadcast_cancelled_list_l& cells_broadcast_cancelled_list() const;
    const crit_diagnostics_s&               crit_diagnostics() const;

  private:
    types                                                                 type_;
    choice_buffer_t<cells_broadcast_cancelled_list_l, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSFailureIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pws_fail_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, pws_failed_nr_cgi_list, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                       transaction_id();
    pws_failed_nr_cgi_list_l&       pws_failed_nr_cgi_list();
    const uint16_t&                 transaction_id() const;
    const pws_failed_nr_cgi_list_l& pws_failed_nr_cgi_list() const;

  private:
    types                                     type_;
    choice_buffer_t<pws_failed_nr_cgi_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWSRestartIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pws_restart_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, nr_cgi_list_for_restart_list, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                             transaction_id();
    nr_cgi_list_for_restart_list_l&       nr_cgi_list_for_restart_list();
    const uint16_t&                       transaction_id() const;
    const nr_cgi_list_for_restart_list_l& nr_cgi_list_for_restart_list() const;

  private:
    types                                           type_;
    choice_buffer_t<nr_cgi_list_for_restart_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PagingIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct paging_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ue_id_idx_value,
        paging_id,
        paging_drx,
        paging_prio,
        paging_cell_list,
        paging_origin,
        ran_ue_paging_drx,
        cn_ue_paging_drx,
        nr_paginge_drx_info,
        nr_paginge_drx_infofor_rrc_inactive,
        paging_cause,
        pe_ip_s_assist_info,
        ue_paging_cap,
        extended_ue_id_idx_value,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    ue_id_idx_value_c&                           ue_id_idx_value();
    paging_id_c&                                 paging_id();
    paging_drx_e&                                paging_drx();
    paging_prio_e&                               paging_prio();
    paging_cell_list_l&                          paging_cell_list();
    paging_origin_e&                             paging_origin();
    paging_drx_e&                                ran_ue_paging_drx();
    paging_drx_e&                                cn_ue_paging_drx();
    nr_paginge_drx_info_s&                       nr_paginge_drx_info();
    nr_paginge_drx_infofor_rrc_inactive_s&       nr_paginge_drx_infofor_rrc_inactive();
    paging_cause_e&                              paging_cause();
    pe_ip_s_assist_info_s&                       pe_ip_s_assist_info();
    ue_paging_cap_s&                             ue_paging_cap();
    fixed_bitstring<16, false, true>&            extended_ue_id_idx_value();
    const ue_id_idx_value_c&                     ue_id_idx_value() const;
    const paging_id_c&                           paging_id() const;
    const paging_drx_e&                          paging_drx() const;
    const paging_prio_e&                         paging_prio() const;
    const paging_cell_list_l&                    paging_cell_list() const;
    const paging_origin_e&                       paging_origin() const;
    const paging_drx_e&                          ran_ue_paging_drx() const;
    const paging_drx_e&                          cn_ue_paging_drx() const;
    const nr_paginge_drx_info_s&                 nr_paginge_drx_info() const;
    const nr_paginge_drx_infofor_rrc_inactive_s& nr_paginge_drx_infofor_rrc_inactive() const;
    const paging_cause_e&                        paging_cause() const;
    const pe_ip_s_assist_info_s&                 pe_ip_s_assist_info() const;
    const ue_paging_cap_s&                       ue_paging_cap() const;
    const fixed_bitstring<16, false, true>&      extended_ue_id_idx_value() const;

  private:
    types type_;
    choice_buffer_t<fixed_bitstring<16, false, true>,
                    nr_paginge_drx_info_s,
                    nr_paginge_drx_infofor_rrc_inactive_s,
                    paging_cell_list_l,
                    paging_id_c,
                    pe_ip_s_assist_info_s,
                    ue_id_idx_value_c,
                    ue_paging_cap_s>
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

// PosSystemInformationDeliveryCommandIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pos_sys_info_delivery_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, nr_cgi, pos_sitype_list, confirmed_ue_id, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                transaction_id();
    nr_cgi_s&                nr_cgi();
    pos_sitype_list_l&       pos_sitype_list();
    uint64_t&                confirmed_ue_id();
    const uint16_t&          transaction_id() const;
    const nr_cgi_s&          nr_cgi() const;
    const pos_sitype_list_l& pos_sitype_list() const;
    const uint64_t&          confirmed_ue_id() const;

  private:
    types                                        type_;
    choice_buffer_t<nr_cgi_s, pos_sitype_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningActivationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_activation_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningActivationRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_activation_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, srs_type, activation_time, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                               gnb_cu_ue_f1ap_id();
    uint64_t&                               gnb_du_ue_f1ap_id();
    srs_type_c&                             srs_type();
    fixed_bitstring<64, false, true>&       activation_time();
    const uint64_t&                         gnb_cu_ue_f1ap_id() const;
    const uint64_t&                         gnb_du_ue_f1ap_id() const;
    const srs_type_c&                       srs_type() const;
    const fixed_bitstring<64, false, true>& activation_time() const;

  private:
    types                                                         type_;
    choice_buffer_t<fixed_bitstring<64, false, true>, srs_type_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningActivationResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_activation_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, sys_frame_num, slot_num, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    uint16_t&                 sys_frame_num();
    uint8_t&                  slot_num();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
    const uint16_t&           sys_frame_num() const;
    const uint8_t&            slot_num() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                               type_;
    choice_buffer_t<crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningAssistanceInformationControlIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_assist_info_ctrl_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        pos_assist_info,
        pos_broadcast,
        positioning_broadcast_cells,
        routing_id,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                            transaction_id();
    unbounded_octstring<true>&           pos_assist_info();
    pos_broadcast_e&                     pos_broadcast();
    positioning_broadcast_cells_l&       positioning_broadcast_cells();
    unbounded_octstring<true>&           routing_id();
    const uint16_t&                      transaction_id() const;
    const unbounded_octstring<true>&     pos_assist_info() const;
    const pos_broadcast_e&               pos_broadcast() const;
    const positioning_broadcast_cells_l& positioning_broadcast_cells() const;
    const unbounded_octstring<true>&     routing_id() const;

  private:
    types                                                                     type_;
    choice_buffer_t<positioning_broadcast_cells_l, unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningAssistanceInformationFeedbackIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_assist_info_feedback_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        pos_assist_info_fail_list,
        positioning_broadcast_cells,
        routing_id,
        crit_diagnostics,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                            transaction_id();
    unbounded_octstring<true>&           pos_assist_info_fail_list();
    positioning_broadcast_cells_l&       positioning_broadcast_cells();
    unbounded_octstring<true>&           routing_id();
    crit_diagnostics_s&                  crit_diagnostics();
    const uint16_t&                      transaction_id() const;
    const unbounded_octstring<true>&     pos_assist_info_fail_list() const;
    const positioning_broadcast_cells_l& positioning_broadcast_cells() const;
    const unbounded_octstring<true>&     routing_id() const;
    const crit_diagnostics_s&            crit_diagnostics() const;

  private:
    types                                                                                         type_;
    choice_buffer_t<crit_diagnostics_s, positioning_broadcast_cells_l, unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningDeactivationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_deactivation_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, abort_tx, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&         gnb_cu_ue_f1ap_id();
    uint64_t&         gnb_du_ue_f1ap_id();
    abort_tx_c&       abort_tx();
    const uint64_t&   gnb_cu_ue_f1ap_id() const;
    const uint64_t&   gnb_du_ue_f1ap_id() const;
    const abort_tx_c& abort_tx() const;

  private:
    types                       type_;
    choice_buffer_t<abort_tx_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningInformationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_info_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningInformationRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_info_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        requested_srs_tx_characteristics,
        ue_report_info,
        srs_pos_rrc_inactive_query_ind,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                 gnb_cu_ue_f1ap_id();
    uint64_t&                                 gnb_du_ue_f1ap_id();
    requested_srs_tx_characteristics_s&       requested_srs_tx_characteristics();
    ue_report_info_s&                         ue_report_info();
    srs_pos_rrc_inactive_query_ind_e&         srs_pos_rrc_inactive_query_ind();
    const uint64_t&                           gnb_cu_ue_f1ap_id() const;
    const uint64_t&                           gnb_du_ue_f1ap_id() const;
    const requested_srs_tx_characteristics_s& requested_srs_tx_characteristics() const;
    const ue_report_info_s&                   ue_report_info() const;
    const srs_pos_rrc_inactive_query_ind_e&   srs_pos_rrc_inactive_query_ind() const;

  private:
    types                                                                 type_;
    choice_buffer_t<requested_srs_tx_characteristics_s, ue_report_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningInformationResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_info_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        srs_cfg,
        sfn_initisation_time,
        crit_diagnostics,
        srs_pos_rrc_inactive_cfg,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                               gnb_cu_ue_f1ap_id();
    uint64_t&                               gnb_du_ue_f1ap_id();
    srs_cfg_s&                              srs_cfg();
    fixed_bitstring<64, false, true>&       sfn_initisation_time();
    crit_diagnostics_s&                     crit_diagnostics();
    unbounded_octstring<true>&              srs_pos_rrc_inactive_cfg();
    const uint64_t&                         gnb_cu_ue_f1ap_id() const;
    const uint64_t&                         gnb_du_ue_f1ap_id() const;
    const srs_cfg_s&                        srs_cfg() const;
    const fixed_bitstring<64, false, true>& sfn_initisation_time() const;
    const crit_diagnostics_s&               crit_diagnostics() const;
    const unbounded_octstring<true>&        srs_pos_rrc_inactive_cfg() const;

  private:
    types                                                                                                       type_;
    choice_buffer_t<crit_diagnostics_s, fixed_bitstring<64, false, true>, srs_cfg_s, unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningInformationUpdateIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_info_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, srs_cfg, sfn_initisation_time, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                               gnb_cu_ue_f1ap_id();
    uint64_t&                               gnb_du_ue_f1ap_id();
    srs_cfg_s&                              srs_cfg();
    fixed_bitstring<64, false, true>&       sfn_initisation_time();
    const uint64_t&                         gnb_cu_ue_f1ap_id() const;
    const uint64_t&                         gnb_du_ue_f1ap_id() const;
    const srs_cfg_s&                        srs_cfg() const;
    const fixed_bitstring<64, false, true>& sfn_initisation_time() const;

  private:
    types                                                        type_;
    choice_buffer_t<fixed_bitstring<64, false, true>, srs_cfg_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningMeasurementAbortIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_meas_abort_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, lmf_meas_id, ran_meas_id, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&       transaction_id();
    uint32_t&       lmf_meas_id();
    uint32_t&       ran_meas_id();
    const uint16_t& transaction_id() const;
    const uint32_t& lmf_meas_id() const;
    const uint32_t& ran_meas_id() const;

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningMeasurementFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_meas_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, lmf_meas_id, ran_meas_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    uint32_t&                 lmf_meas_id();
    uint32_t&                 ran_meas_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const uint32_t&           lmf_meas_id() const;
    const uint32_t&           ran_meas_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningMeasurementFailureIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_meas_fail_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, lmf_meas_id, ran_meas_id, cause, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&       transaction_id();
    uint32_t&       lmf_meas_id();
    uint32_t&       ran_meas_id();
    cause_c&        cause();
    const uint16_t& transaction_id() const;
    const uint32_t& lmf_meas_id() const;
    const uint32_t& ran_meas_id() const;
    const cause_c&  cause() const;

  private:
    types                    type_;
    choice_buffer_t<cause_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningMeasurementReportIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_meas_report_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, lmf_meas_id, ran_meas_id, pos_meas_result_list, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                     transaction_id();
    uint32_t&                     lmf_meas_id();
    uint32_t&                     ran_meas_id();
    pos_meas_result_list_l&       pos_meas_result_list();
    const uint16_t&               transaction_id() const;
    const uint32_t&               lmf_meas_id() const;
    const uint32_t&               ran_meas_id() const;
    const pos_meas_result_list_l& pos_meas_result_list() const;

  private:
    types                                   type_;
    choice_buffer_t<pos_meas_result_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningMeasurementRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_meas_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        lmf_meas_id,
        ran_meas_id,
        trp_meas_request_list,
        pos_report_characteristics,
        pos_meas_periodicity,
        pos_meas_quantities,
        sfn_initisation_time,
        srs_cfg,
        meas_beam_info_request,
        sys_frame_num,
        slot_num,
        pos_meas_periodicity_extended,
        resp_time,
        meas_characteristics_request_ind,
        meas_time_occasion,
        pos_meas_amount,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                               transaction_id();
    uint32_t&                               lmf_meas_id();
    uint32_t&                               ran_meas_id();
    trp_meas_request_list_l&                trp_meas_request_list();
    pos_report_characteristics_e&           pos_report_characteristics();
    meas_periodicity_e&                     pos_meas_periodicity();
    pos_meas_quantities_l&                  pos_meas_quantities();
    fixed_bitstring<64, false, true>&       sfn_initisation_time();
    srs_cfg_s&                              srs_cfg();
    meas_beam_info_request_e&               meas_beam_info_request();
    uint16_t&                               sys_frame_num();
    uint8_t&                                slot_num();
    meas_periodicity_extended_e&            pos_meas_periodicity_extended();
    resp_time_s&                            resp_time();
    fixed_bitstring<16, false, true>&       meas_characteristics_request_ind();
    meas_time_occasion_e&                   meas_time_occasion();
    pos_meas_amount_e&                      pos_meas_amount();
    const uint16_t&                         transaction_id() const;
    const uint32_t&                         lmf_meas_id() const;
    const uint32_t&                         ran_meas_id() const;
    const trp_meas_request_list_l&          trp_meas_request_list() const;
    const pos_report_characteristics_e&     pos_report_characteristics() const;
    const meas_periodicity_e&               pos_meas_periodicity() const;
    const pos_meas_quantities_l&            pos_meas_quantities() const;
    const fixed_bitstring<64, false, true>& sfn_initisation_time() const;
    const srs_cfg_s&                        srs_cfg() const;
    const meas_beam_info_request_e&         meas_beam_info_request() const;
    const uint16_t&                         sys_frame_num() const;
    const uint8_t&                          slot_num() const;
    const meas_periodicity_extended_e&      pos_meas_periodicity_extended() const;
    const resp_time_s&                      resp_time() const;
    const fixed_bitstring<16, false, true>& meas_characteristics_request_ind() const;
    const meas_time_occasion_e&             meas_time_occasion() const;
    const pos_meas_amount_e&                pos_meas_amount() const;

  private:
    types type_;
    choice_buffer_t<fixed_bitstring<64, false, true>,
                    pos_meas_quantities_l,
                    resp_time_s,
                    srs_cfg_s,
                    trp_meas_request_list_l>
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

// PositioningMeasurementResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_meas_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, lmf_meas_id, ran_meas_id, pos_meas_result_list, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                     transaction_id();
    uint32_t&                     lmf_meas_id();
    uint32_t&                     ran_meas_id();
    pos_meas_result_list_l&       pos_meas_result_list();
    crit_diagnostics_s&           crit_diagnostics();
    const uint16_t&               transaction_id() const;
    const uint32_t&               lmf_meas_id() const;
    const uint32_t&               ran_meas_id() const;
    const pos_meas_result_list_l& pos_meas_result_list() const;
    const crit_diagnostics_s&     crit_diagnostics() const;

  private:
    types                                                       type_;
    choice_buffer_t<crit_diagnostics_s, pos_meas_result_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningMeasurementUpdateIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_meas_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        lmf_meas_id,
        ran_meas_id,
        srs_cfg,
        trp_meas_upd_list,
        meas_characteristics_request_ind,
        meas_time_occasion,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                               transaction_id();
    uint32_t&                               lmf_meas_id();
    uint32_t&                               ran_meas_id();
    srs_cfg_s&                              srs_cfg();
    trp_meas_upd_list_l&                    trp_meas_upd_list();
    fixed_bitstring<16, false, true>&       meas_characteristics_request_ind();
    meas_time_occasion_e&                   meas_time_occasion();
    const uint16_t&                         transaction_id() const;
    const uint32_t&                         lmf_meas_id() const;
    const uint32_t&                         ran_meas_id() const;
    const srs_cfg_s&                        srs_cfg() const;
    const trp_meas_upd_list_l&              trp_meas_upd_list() const;
    const fixed_bitstring<16, false, true>& meas_characteristics_request_ind() const;
    const meas_time_occasion_e&             meas_time_occasion() const;

  private:
    types                                                                             type_;
    choice_buffer_t<fixed_bitstring<16, false, true>, srs_cfg_s, trp_meas_upd_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// QoEInformationTransfer-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct qo_e_info_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, qo_e_info, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&          gnb_cu_ue_f1ap_id();
    uint64_t&          gnb_du_ue_f1ap_id();
    qo_e_info_s&       qo_e_info();
    const uint64_t&    gnb_cu_ue_f1ap_id() const;
    const uint64_t&    gnb_du_ue_f1ap_id() const;
    const qo_e_info_s& qo_e_info() const;

  private:
    types                        type_;
    choice_buffer_t<qo_e_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RRCDeliveryReportIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct rrc_delivery_report_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, rrc_delivery_status, srb_id, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                    gnb_cu_ue_f1ap_id();
    uint64_t&                    gnb_du_ue_f1ap_id();
    rrc_delivery_status_s&       rrc_delivery_status();
    uint8_t&                     srb_id();
    const uint64_t&              gnb_cu_ue_f1ap_id() const;
    const uint64_t&              gnb_du_ue_f1ap_id() const;
    const rrc_delivery_status_s& rrc_delivery_status() const;
    const uint8_t&               srb_id() const;

  private:
    types                                  type_;
    choice_buffer_t<rrc_delivery_status_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ReferenceTimeInformationReportIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ref_time_info_report_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, time_ref_info, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&              transaction_id();
    time_ref_info_s&       time_ref_info();
    const uint16_t&        transaction_id() const;
    const time_ref_info_s& time_ref_info() const;

  private:
    types                            type_;
    choice_buffer_t<time_ref_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ReferenceTimeInformationReportingControlIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ref_time_info_report_ctrl_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, report_request_type, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                    transaction_id();
    report_request_type_s&       report_request_type();
    const uint16_t&              transaction_id() const;
    const report_request_type_s& report_request_type() const;

  private:
    types                                  type_;
    choice_buffer_t<report_request_type_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ResetAcknowledgeIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct reset_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, ue_associated_lc_f1_conn_list_res_ack, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                                      transaction_id();
    ue_associated_lc_f1_conn_list_res_ack_l&       ue_associated_lc_f1_conn_list_res_ack();
    crit_diagnostics_s&                            crit_diagnostics();
    const uint16_t&                                transaction_id() const;
    const ue_associated_lc_f1_conn_list_res_ack_l& ue_associated_lc_f1_conn_list_res_ack() const;
    const crit_diagnostics_s&                      crit_diagnostics() const;

  private:
    types                                                                        type_;
    choice_buffer_t<crit_diagnostics_s, ue_associated_lc_f1_conn_list_res_ack_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ResetIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct reset_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, reset_type, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&           transaction_id();
    cause_c&            cause();
    reset_type_c&       reset_type();
    const uint16_t&     transaction_id() const;
    const cause_c&      cause() const;
    const reset_type_c& reset_type() const;

  private:
    types                                  type_;
    choice_buffer_t<cause_c, reset_type_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ResourceStatusFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct res_status_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, gnb_cu_meas_id, gnb_du_meas_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    uint16_t&                 gnb_cu_meas_id();
    uint16_t&                 gnb_du_meas_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const uint16_t&           gnb_cu_meas_id() const;
    const uint16_t&           gnb_du_meas_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ResourceStatusRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct res_status_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        gnb_cu_meas_id,
        gnb_du_meas_id,
        regist_request,
        report_characteristics,
        cell_to_report_list,
        report_periodicity,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                               transaction_id();
    uint16_t&                               gnb_cu_meas_id();
    uint16_t&                               gnb_du_meas_id();
    regist_request_e&                       regist_request();
    fixed_bitstring<32, false, true>&       report_characteristics();
    cell_to_report_list_l&                  cell_to_report_list();
    report_periodicity_e&                   report_periodicity();
    const uint16_t&                         transaction_id() const;
    const uint16_t&                         gnb_cu_meas_id() const;
    const uint16_t&                         gnb_du_meas_id() const;
    const regist_request_e&                 regist_request() const;
    const fixed_bitstring<32, false, true>& report_characteristics() const;
    const cell_to_report_list_l&            cell_to_report_list() const;
    const report_periodicity_e&             report_periodicity() const;

  private:
    types                                                                    type_;
    choice_buffer_t<cell_to_report_list_l, fixed_bitstring<32, false, true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ResourceStatusResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct res_status_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, gnb_cu_meas_id, gnb_du_meas_id, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    uint16_t&                 gnb_cu_meas_id();
    uint16_t&                 gnb_du_meas_id();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const uint16_t&           gnb_cu_meas_id() const;
    const uint16_t&           gnb_du_meas_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                               type_;
    choice_buffer_t<crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ResourceStatusUpdateIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct res_status_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        gnb_cu_meas_id,
        gnb_du_meas_id,
        hardware_load_ind,
        tnl_capacity_ind,
        cell_meas_result_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                      transaction_id();
    uint16_t&                      gnb_cu_meas_id();
    uint16_t&                      gnb_du_meas_id();
    hardware_load_ind_s&           hardware_load_ind();
    tnl_capacity_ind_s&            tnl_capacity_ind();
    cell_meas_result_list_l&       cell_meas_result_list();
    const uint16_t&                transaction_id() const;
    const uint16_t&                gnb_cu_meas_id() const;
    const uint16_t&                gnb_du_meas_id() const;
    const hardware_load_ind_s&     hardware_load_ind() const;
    const tnl_capacity_ind_s&      tnl_capacity_ind() const;
    const cell_meas_result_list_l& cell_meas_result_list() const;

  private:
    types                                                                             type_;
    choice_buffer_t<cell_meas_result_list_l, hardware_load_ind_s, tnl_capacity_ind_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SystemInformationDeliveryCommandIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sys_info_delivery_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, nr_cgi, sitype_list, confirmed_ue_id, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&            transaction_id();
    nr_cgi_s&            nr_cgi();
    sitype_list_l&       sitype_list();
    uint64_t&            confirmed_ue_id();
    const uint16_t&      transaction_id() const;
    const nr_cgi_s&      nr_cgi() const;
    const sitype_list_l& sitype_list() const;
    const uint64_t&      confirmed_ue_id() const;

  private:
    types                                    type_;
    choice_buffer_t<nr_cgi_s, sitype_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TRPInformationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct trp_info_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TRPInformationRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct trp_info_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, trp_list, trp_info_type_list_trp_req, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                           transaction_id();
    trp_list_l&                         trp_list();
    trp_info_type_list_trp_req_l&       trp_info_type_list_trp_req();
    const uint16_t&                     transaction_id() const;
    const trp_list_l&                   trp_list() const;
    const trp_info_type_list_trp_req_l& trp_info_type_list_trp_req() const;

  private:
    types                                                     type_;
    choice_buffer_t<trp_info_type_list_trp_req_l, trp_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TRPInformationResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct trp_info_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, trp_info_list_trp_resp, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                       transaction_id();
    trp_info_list_trp_resp_l&       trp_info_list_trp_resp();
    crit_diagnostics_s&             crit_diagnostics();
    const uint16_t&                 transaction_id() const;
    const trp_info_list_trp_resp_l& trp_info_list_trp_resp() const;
    const crit_diagnostics_s&       crit_diagnostics() const;

  private:
    types                                                         type_;
    choice_buffer_t<crit_diagnostics_s, trp_info_list_trp_resp_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TraceStartIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct trace_start_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, trace_activation, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    trace_activation_s&       trace_activation();
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
    const trace_activation_s& trace_activation() const;

  private:
    types                               type_;
    choice_buffer_t<trace_activation_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextModificationConfirmIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_mod_confirm_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        res_coordination_transfer_container,
        drbs_modified_conf_list,
        rrc_container,
        crit_diagnostics,
        execute_dupl,
        res_coordination_transfer_info,
        sl_drbs_modified_conf_list,
        uu_rlc_ch_modified_list,
        pc5_rlc_ch_modified_list,
        ue_multicast_m_rbs_confirmed_to_be_modified_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                                 gnb_cu_ue_f1ap_id();
    uint64_t&                                                 gnb_du_ue_f1ap_id();
    unbounded_octstring<true>&                                res_coordination_transfer_container();
    drbs_modified_conf_list_l&                                drbs_modified_conf_list();
    unbounded_octstring<true>&                                rrc_container();
    crit_diagnostics_s&                                       crit_diagnostics();
    execute_dupl_e&                                           execute_dupl();
    res_coordination_transfer_info_s&                         res_coordination_transfer_info();
    sl_drbs_modified_conf_list_l&                             sl_drbs_modified_conf_list();
    uu_rlc_ch_modified_list_l&                                uu_rlc_ch_modified_list();
    pc5_rlc_ch_modified_list_l&                               pc5_rlc_ch_modified_list();
    ue_multicast_m_rbs_confirmed_to_be_modified_list_l&       ue_multicast_m_rbs_confirmed_to_be_modified_list();
    const uint64_t&                                           gnb_cu_ue_f1ap_id() const;
    const uint64_t&                                           gnb_du_ue_f1ap_id() const;
    const unbounded_octstring<true>&                          res_coordination_transfer_container() const;
    const drbs_modified_conf_list_l&                          drbs_modified_conf_list() const;
    const unbounded_octstring<true>&                          rrc_container() const;
    const crit_diagnostics_s&                                 crit_diagnostics() const;
    const execute_dupl_e&                                     execute_dupl() const;
    const res_coordination_transfer_info_s&                   res_coordination_transfer_info() const;
    const sl_drbs_modified_conf_list_l&                       sl_drbs_modified_conf_list() const;
    const uu_rlc_ch_modified_list_l&                          uu_rlc_ch_modified_list() const;
    const pc5_rlc_ch_modified_list_l&                         pc5_rlc_ch_modified_list() const;
    const ue_multicast_m_rbs_confirmed_to_be_modified_list_l& ue_multicast_m_rbs_confirmed_to_be_modified_list() const;

  private:
    types type_;
    choice_buffer_t<crit_diagnostics_s,
                    drbs_modified_conf_list_l,
                    pc5_rlc_ch_modified_list_l,
                    res_coordination_transfer_info_s,
                    sl_drbs_modified_conf_list_l,
                    ue_multicast_m_rbs_confirmed_to_be_modified_list_l,
                    unbounded_octstring<true>,
                    uu_rlc_ch_modified_list_l>
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

// UEContextModificationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_mod_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        cause,
        crit_diagnostics,
        requested_target_cell_global_id,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    nr_cgi_s&                 requested_target_cell_global_id();
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;
    const nr_cgi_s&           requested_target_cell_global_id() const;

  private:
    types                                                  type_;
    choice_buffer_t<cause_c, crit_diagnostics_s, nr_cgi_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextModificationRefuseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_mod_refuse_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextModificationRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_mod_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        sp_cell_id,
        serv_cell_idx,
        sp_cell_ul_cfg,
        drx_cycle,
        cu_to_du_rrc_info,
        tx_action_ind,
        res_coordination_transfer_container,
        rrc_recfg_complete_ind,
        rrc_container,
        scell_to_be_setup_mod_list,
        scell_to_be_remd_list,
        srbs_to_be_setup_mod_list,
        drbs_to_be_setup_mod_list,
        drbs_to_be_modified_list,
        srbs_to_be_released_list,
        drbs_to_be_released_list,
        inactivity_monitoring_request,
        rat_freq_prio_info,
        drx_cfg_ind,
        rlc_fail_ind,
        ul_tx_direct_current_list_info,
        gnb_du_cfg_query,
        gnb_du_ue_ambr_ul,
        execute_dupl,
        rrc_delivery_status_request,
        res_coordination_transfer_info,
        serving_cell_mo,
        needfor_gap,
        full_cfg,
        add_rrm_prio_idx,
        lower_layer_presence_status_change,
        bh_chs_to_be_setup_mod_list,
        bh_chs_to_be_modified_list,
        bh_chs_to_be_released_list,
        nr_v2x_services_authorized,
        ltev2x_services_authorized,
        nr_ue_sidelink_aggr_max_bitrate,
        lte_ue_sidelink_aggr_max_bitrate,
        pc5_link_ambr,
        sl_drbs_to_be_setup_mod_list,
        sl_drbs_to_be_modified_list,
        sl_drbs_to_be_released_list,
        conditional_intra_du_mob_info,
        f1_c_transfer_path,
        scg_ind,
        ul_tx_direct_current_two_carrier_list_info,
        iab_conditional_rrc_msg_delivery_ind,
        f1_c_transfer_path_nr_dc,
        mdt_polluted_meas_ind,
        scg_activation_request,
        cg_sdt_query_ind,
        five_g_pro_se_authorized,
        five_g_pro_se_ue_pc5_aggr_max_bitrate,
        five_g_pro_se_pc5_link_ambr,
        upd_remote_ue_local_id,
        uu_rlc_ch_to_be_setup_list,
        uu_rlc_ch_to_be_modified_list,
        uu_rlc_ch_to_be_released_list,
        pc5_rlc_ch_to_be_setup_list,
        pc5_rlc_ch_to_be_modified_list,
        pc5_rlc_ch_to_be_released_list,
        path_switch_cfg,
        gnb_du_ue_slice_max_bit_rate_list,
        multicast_mbs_session_setup_list,
        multicast_mbs_session_rem_list,
        ue_multicast_m_rbs_to_be_setup_at_modify_list,
        ue_multicast_m_rbs_to_be_released_list,
        sldrx_cycle_list,
        management_based_mdt_plmn_mod_list,
        sdt_bearer_cfg_query_ind,
        daps_ho_status,
        serving_cell_mo_list,
        ul_tx_direct_current_more_carrier_info,
        cp_acmcg_info,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                              gnb_cu_ue_f1ap_id();
    uint64_t&                                              gnb_du_ue_f1ap_id();
    nr_cgi_s&                                              sp_cell_id();
    uint8_t&                                               serv_cell_idx();
    cell_ul_cfg_e&                                         sp_cell_ul_cfg();
    drx_cycle_s&                                           drx_cycle();
    cu_to_du_rrc_info_s&                                   cu_to_du_rrc_info();
    tx_action_ind_e&                                       tx_action_ind();
    unbounded_octstring<true>&                             res_coordination_transfer_container();
    rrc_recfg_complete_ind_e&                              rrc_recfg_complete_ind();
    unbounded_octstring<true>&                             rrc_container();
    scell_to_be_setup_mod_list_l&                          scell_to_be_setup_mod_list();
    scell_to_be_remd_list_l&                               scell_to_be_remd_list();
    srbs_to_be_setup_mod_list_l&                           srbs_to_be_setup_mod_list();
    drbs_to_be_setup_mod_list_l&                           drbs_to_be_setup_mod_list();
    drbs_to_be_modified_list_l&                            drbs_to_be_modified_list();
    srbs_to_be_released_list_l&                            srbs_to_be_released_list();
    drbs_to_be_released_list_l&                            drbs_to_be_released_list();
    inactivity_monitoring_request_e&                       inactivity_monitoring_request();
    rat_freq_prio_info_c&                                  rat_freq_prio_info();
    drx_cfg_ind_e&                                         drx_cfg_ind();
    rlc_fail_ind_s&                                        rlc_fail_ind();
    unbounded_octstring<true>&                             ul_tx_direct_current_list_info();
    gnb_du_cfg_query_e&                                    gnb_du_cfg_query();
    uint64_t&                                              gnb_du_ue_ambr_ul();
    execute_dupl_e&                                        execute_dupl();
    rrc_delivery_status_request_e&                         rrc_delivery_status_request();
    res_coordination_transfer_info_s&                      res_coordination_transfer_info();
    uint8_t&                                               serving_cell_mo();
    needfor_gap_e&                                         needfor_gap();
    full_cfg_e&                                            full_cfg();
    fixed_bitstring<32, false, true>&                      add_rrm_prio_idx();
    lower_layer_presence_status_change_e&                  lower_layer_presence_status_change();
    bh_chs_to_be_setup_mod_list_l&                         bh_chs_to_be_setup_mod_list();
    bh_chs_to_be_modified_list_l&                          bh_chs_to_be_modified_list();
    bh_chs_to_be_released_list_l&                          bh_chs_to_be_released_list();
    nr_v2x_services_authorized_s&                          nr_v2x_services_authorized();
    ltev2x_services_authorized_s&                          ltev2x_services_authorized();
    nr_ue_sidelink_aggr_max_bitrate_s&                     nr_ue_sidelink_aggr_max_bitrate();
    lte_ue_sidelink_aggr_max_bitrate_s&                    lte_ue_sidelink_aggr_max_bitrate();
    uint64_t&                                              pc5_link_ambr();
    sl_drbs_to_be_setup_mod_list_l&                        sl_drbs_to_be_setup_mod_list();
    sl_drbs_to_be_modified_list_l&                         sl_drbs_to_be_modified_list();
    sl_drbs_to_be_released_list_l&                         sl_drbs_to_be_released_list();
    conditional_intra_du_mob_info_s&                       conditional_intra_du_mob_info();
    f1_c_transfer_path_s&                                  f1_c_transfer_path();
    scg_ind_e&                                             scg_ind();
    unbounded_octstring<true>&                             ul_tx_direct_current_two_carrier_list_info();
    iab_conditional_rrc_msg_delivery_ind_e&                iab_conditional_rrc_msg_delivery_ind();
    f1_c_transfer_path_nr_dc_s&                            f1_c_transfer_path_nr_dc();
    mdt_polluted_meas_ind_e&                               mdt_polluted_meas_ind();
    scg_activation_request_e&                              scg_activation_request();
    cg_sdt_query_ind_e&                                    cg_sdt_query_ind();
    five_g_pro_se_authorized_s&                            five_g_pro_se_authorized();
    nr_ue_sidelink_aggr_max_bitrate_s&                     five_g_pro_se_ue_pc5_aggr_max_bitrate();
    uint64_t&                                              five_g_pro_se_pc5_link_ambr();
    uint16_t&                                              upd_remote_ue_local_id();
    uu_rlc_ch_to_be_setup_list_l&                          uu_rlc_ch_to_be_setup_list();
    uu_rlc_ch_to_be_modified_list_l&                       uu_rlc_ch_to_be_modified_list();
    uu_rlc_ch_to_be_released_list_l&                       uu_rlc_ch_to_be_released_list();
    pc5_rlc_ch_to_be_setup_list_l&                         pc5_rlc_ch_to_be_setup_list();
    pc5_rlc_ch_to_be_modified_list_l&                      pc5_rlc_ch_to_be_modified_list();
    pc5_rlc_ch_to_be_released_list_l&                      pc5_rlc_ch_to_be_released_list();
    path_switch_cfg_s&                                     path_switch_cfg();
    gnb_du_ue_slice_max_bit_rate_list_l&                   gnb_du_ue_slice_max_bit_rate_list();
    multicast_mbs_session_list_l&                          multicast_mbs_session_setup_list();
    multicast_mbs_session_list_l&                          multicast_mbs_session_rem_list();
    ue_multicast_m_rbs_to_be_setup_at_modify_list_l&       ue_multicast_m_rbs_to_be_setup_at_modify_list();
    ue_multicast_m_rbs_to_be_released_list_l&              ue_multicast_m_rbs_to_be_released_list();
    sldrx_cycle_list_l&                                    sldrx_cycle_list();
    mdt_plmn_mod_list_l&                                   management_based_mdt_plmn_mod_list();
    sdt_bearer_cfg_query_ind_e&                            sdt_bearer_cfg_query_ind();
    daps_ho_status_e&                                      daps_ho_status();
    serving_cell_mo_list_l&                                serving_cell_mo_list();
    unbounded_octstring<true>&                             ul_tx_direct_current_more_carrier_info();
    cp_acmcg_info_s&                                       cp_acmcg_info();
    const uint64_t&                                        gnb_cu_ue_f1ap_id() const;
    const uint64_t&                                        gnb_du_ue_f1ap_id() const;
    const nr_cgi_s&                                        sp_cell_id() const;
    const uint8_t&                                         serv_cell_idx() const;
    const cell_ul_cfg_e&                                   sp_cell_ul_cfg() const;
    const drx_cycle_s&                                     drx_cycle() const;
    const cu_to_du_rrc_info_s&                             cu_to_du_rrc_info() const;
    const tx_action_ind_e&                                 tx_action_ind() const;
    const unbounded_octstring<true>&                       res_coordination_transfer_container() const;
    const rrc_recfg_complete_ind_e&                        rrc_recfg_complete_ind() const;
    const unbounded_octstring<true>&                       rrc_container() const;
    const scell_to_be_setup_mod_list_l&                    scell_to_be_setup_mod_list() const;
    const scell_to_be_remd_list_l&                         scell_to_be_remd_list() const;
    const srbs_to_be_setup_mod_list_l&                     srbs_to_be_setup_mod_list() const;
    const drbs_to_be_setup_mod_list_l&                     drbs_to_be_setup_mod_list() const;
    const drbs_to_be_modified_list_l&                      drbs_to_be_modified_list() const;
    const srbs_to_be_released_list_l&                      srbs_to_be_released_list() const;
    const drbs_to_be_released_list_l&                      drbs_to_be_released_list() const;
    const inactivity_monitoring_request_e&                 inactivity_monitoring_request() const;
    const rat_freq_prio_info_c&                            rat_freq_prio_info() const;
    const drx_cfg_ind_e&                                   drx_cfg_ind() const;
    const rlc_fail_ind_s&                                  rlc_fail_ind() const;
    const unbounded_octstring<true>&                       ul_tx_direct_current_list_info() const;
    const gnb_du_cfg_query_e&                              gnb_du_cfg_query() const;
    const uint64_t&                                        gnb_du_ue_ambr_ul() const;
    const execute_dupl_e&                                  execute_dupl() const;
    const rrc_delivery_status_request_e&                   rrc_delivery_status_request() const;
    const res_coordination_transfer_info_s&                res_coordination_transfer_info() const;
    const uint8_t&                                         serving_cell_mo() const;
    const needfor_gap_e&                                   needfor_gap() const;
    const full_cfg_e&                                      full_cfg() const;
    const fixed_bitstring<32, false, true>&                add_rrm_prio_idx() const;
    const lower_layer_presence_status_change_e&            lower_layer_presence_status_change() const;
    const bh_chs_to_be_setup_mod_list_l&                   bh_chs_to_be_setup_mod_list() const;
    const bh_chs_to_be_modified_list_l&                    bh_chs_to_be_modified_list() const;
    const bh_chs_to_be_released_list_l&                    bh_chs_to_be_released_list() const;
    const nr_v2x_services_authorized_s&                    nr_v2x_services_authorized() const;
    const ltev2x_services_authorized_s&                    ltev2x_services_authorized() const;
    const nr_ue_sidelink_aggr_max_bitrate_s&               nr_ue_sidelink_aggr_max_bitrate() const;
    const lte_ue_sidelink_aggr_max_bitrate_s&              lte_ue_sidelink_aggr_max_bitrate() const;
    const uint64_t&                                        pc5_link_ambr() const;
    const sl_drbs_to_be_setup_mod_list_l&                  sl_drbs_to_be_setup_mod_list() const;
    const sl_drbs_to_be_modified_list_l&                   sl_drbs_to_be_modified_list() const;
    const sl_drbs_to_be_released_list_l&                   sl_drbs_to_be_released_list() const;
    const conditional_intra_du_mob_info_s&                 conditional_intra_du_mob_info() const;
    const f1_c_transfer_path_s&                            f1_c_transfer_path() const;
    const scg_ind_e&                                       scg_ind() const;
    const unbounded_octstring<true>&                       ul_tx_direct_current_two_carrier_list_info() const;
    const iab_conditional_rrc_msg_delivery_ind_e&          iab_conditional_rrc_msg_delivery_ind() const;
    const f1_c_transfer_path_nr_dc_s&                      f1_c_transfer_path_nr_dc() const;
    const mdt_polluted_meas_ind_e&                         mdt_polluted_meas_ind() const;
    const scg_activation_request_e&                        scg_activation_request() const;
    const cg_sdt_query_ind_e&                              cg_sdt_query_ind() const;
    const five_g_pro_se_authorized_s&                      five_g_pro_se_authorized() const;
    const nr_ue_sidelink_aggr_max_bitrate_s&               five_g_pro_se_ue_pc5_aggr_max_bitrate() const;
    const uint64_t&                                        five_g_pro_se_pc5_link_ambr() const;
    const uint16_t&                                        upd_remote_ue_local_id() const;
    const uu_rlc_ch_to_be_setup_list_l&                    uu_rlc_ch_to_be_setup_list() const;
    const uu_rlc_ch_to_be_modified_list_l&                 uu_rlc_ch_to_be_modified_list() const;
    const uu_rlc_ch_to_be_released_list_l&                 uu_rlc_ch_to_be_released_list() const;
    const pc5_rlc_ch_to_be_setup_list_l&                   pc5_rlc_ch_to_be_setup_list() const;
    const pc5_rlc_ch_to_be_modified_list_l&                pc5_rlc_ch_to_be_modified_list() const;
    const pc5_rlc_ch_to_be_released_list_l&                pc5_rlc_ch_to_be_released_list() const;
    const path_switch_cfg_s&                               path_switch_cfg() const;
    const gnb_du_ue_slice_max_bit_rate_list_l&             gnb_du_ue_slice_max_bit_rate_list() const;
    const multicast_mbs_session_list_l&                    multicast_mbs_session_setup_list() const;
    const multicast_mbs_session_list_l&                    multicast_mbs_session_rem_list() const;
    const ue_multicast_m_rbs_to_be_setup_at_modify_list_l& ue_multicast_m_rbs_to_be_setup_at_modify_list() const;
    const ue_multicast_m_rbs_to_be_released_list_l&        ue_multicast_m_rbs_to_be_released_list() const;
    const sldrx_cycle_list_l&                              sldrx_cycle_list() const;
    const mdt_plmn_mod_list_l&                             management_based_mdt_plmn_mod_list() const;
    const sdt_bearer_cfg_query_ind_e&                      sdt_bearer_cfg_query_ind() const;
    const daps_ho_status_e&                                daps_ho_status() const;
    const serving_cell_mo_list_l&                          serving_cell_mo_list() const;
    const unbounded_octstring<true>&                       ul_tx_direct_current_more_carrier_info() const;
    const cp_acmcg_info_s&                                 cp_acmcg_info() const;

  private:
    types type_;
    choice_buffer_t<bh_chs_to_be_modified_list_l,
                    bh_chs_to_be_released_list_l,
                    bh_chs_to_be_setup_mod_list_l,
                    conditional_intra_du_mob_info_s,
                    cp_acmcg_info_s,
                    cu_to_du_rrc_info_s,
                    drbs_to_be_modified_list_l,
                    drbs_to_be_released_list_l,
                    drbs_to_be_setup_mod_list_l,
                    drx_cycle_s,
                    f1_c_transfer_path_nr_dc_s,
                    f1_c_transfer_path_s,
                    five_g_pro_se_authorized_s,
                    fixed_bitstring<32, false, true>,
                    gnb_du_ue_slice_max_bit_rate_list_l,
                    lte_ue_sidelink_aggr_max_bitrate_s,
                    ltev2x_services_authorized_s,
                    mdt_plmn_mod_list_l,
                    multicast_mbs_session_list_l,
                    nr_cgi_s,
                    nr_ue_sidelink_aggr_max_bitrate_s,
                    nr_v2x_services_authorized_s,
                    path_switch_cfg_s,
                    pc5_rlc_ch_to_be_modified_list_l,
                    pc5_rlc_ch_to_be_released_list_l,
                    pc5_rlc_ch_to_be_setup_list_l,
                    rat_freq_prio_info_c,
                    res_coordination_transfer_info_s,
                    rlc_fail_ind_s,
                    scell_to_be_remd_list_l,
                    scell_to_be_setup_mod_list_l,
                    serving_cell_mo_list_l,
                    sl_drbs_to_be_modified_list_l,
                    sl_drbs_to_be_released_list_l,
                    sl_drbs_to_be_setup_mod_list_l,
                    sldrx_cycle_list_l,
                    srbs_to_be_released_list_l,
                    srbs_to_be_setup_mod_list_l,
                    ue_multicast_m_rbs_to_be_released_list_l,
                    ue_multicast_m_rbs_to_be_setup_at_modify_list_l,
                    unbounded_octstring<true>,
                    uu_rlc_ch_to_be_modified_list_l,
                    uu_rlc_ch_to_be_released_list_l,
                    uu_rlc_ch_to_be_setup_list_l>
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

// UEContextModificationRequiredIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_mod_required_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        res_coordination_transfer_container,
        du_to_cu_rrc_info,
        drbs_required_to_be_modified_list,
        srbs_required_to_be_released_list,
        drbs_required_to_be_released_list,
        cause,
        bh_chs_required_to_be_released_list,
        sl_drbs_required_to_be_modified_list,
        sl_drbs_required_to_be_released_list,
        target_cells_to_cancel,
        uu_rlc_ch_required_to_be_modified_list,
        uu_rlc_ch_required_to_be_released_list,
        pc5_rlc_ch_required_to_be_modified_list,
        pc5_rlc_ch_required_to_be_released_list,
        ue_multicast_m_rbs_required_to_be_modified_list,
        ue_multicast_m_rbs_required_to_be_released_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                                gnb_cu_ue_f1ap_id();
    uint64_t&                                                gnb_du_ue_f1ap_id();
    unbounded_octstring<true>&                               res_coordination_transfer_container();
    du_to_cu_rrc_info_s&                                     du_to_cu_rrc_info();
    drbs_required_to_be_modified_list_l&                     drbs_required_to_be_modified_list();
    srbs_required_to_be_released_list_l&                     srbs_required_to_be_released_list();
    drbs_required_to_be_released_list_l&                     drbs_required_to_be_released_list();
    cause_c&                                                 cause();
    bh_chs_required_to_be_released_list_l&                   bh_chs_required_to_be_released_list();
    sl_drbs_required_to_be_modified_list_l&                  sl_drbs_required_to_be_modified_list();
    sl_drbs_required_to_be_released_list_l&                  sl_drbs_required_to_be_released_list();
    target_cell_list_l&                                      target_cells_to_cancel();
    uu_rlc_ch_required_to_be_modified_list_l&                uu_rlc_ch_required_to_be_modified_list();
    uu_rlc_ch_required_to_be_released_list_l&                uu_rlc_ch_required_to_be_released_list();
    pc5_rlc_ch_required_to_be_modified_list_l&               pc5_rlc_ch_required_to_be_modified_list();
    pc5_rlc_ch_required_to_be_released_list_l&               pc5_rlc_ch_required_to_be_released_list();
    ue_multicast_m_rbs_required_to_be_modified_list_l&       ue_multicast_m_rbs_required_to_be_modified_list();
    ue_multicast_m_rbs_required_to_be_released_list_l&       ue_multicast_m_rbs_required_to_be_released_list();
    const uint64_t&                                          gnb_cu_ue_f1ap_id() const;
    const uint64_t&                                          gnb_du_ue_f1ap_id() const;
    const unbounded_octstring<true>&                         res_coordination_transfer_container() const;
    const du_to_cu_rrc_info_s&                               du_to_cu_rrc_info() const;
    const drbs_required_to_be_modified_list_l&               drbs_required_to_be_modified_list() const;
    const srbs_required_to_be_released_list_l&               srbs_required_to_be_released_list() const;
    const drbs_required_to_be_released_list_l&               drbs_required_to_be_released_list() const;
    const cause_c&                                           cause() const;
    const bh_chs_required_to_be_released_list_l&             bh_chs_required_to_be_released_list() const;
    const sl_drbs_required_to_be_modified_list_l&            sl_drbs_required_to_be_modified_list() const;
    const sl_drbs_required_to_be_released_list_l&            sl_drbs_required_to_be_released_list() const;
    const target_cell_list_l&                                target_cells_to_cancel() const;
    const uu_rlc_ch_required_to_be_modified_list_l&          uu_rlc_ch_required_to_be_modified_list() const;
    const uu_rlc_ch_required_to_be_released_list_l&          uu_rlc_ch_required_to_be_released_list() const;
    const pc5_rlc_ch_required_to_be_modified_list_l&         pc5_rlc_ch_required_to_be_modified_list() const;
    const pc5_rlc_ch_required_to_be_released_list_l&         pc5_rlc_ch_required_to_be_released_list() const;
    const ue_multicast_m_rbs_required_to_be_modified_list_l& ue_multicast_m_rbs_required_to_be_modified_list() const;
    const ue_multicast_m_rbs_required_to_be_released_list_l& ue_multicast_m_rbs_required_to_be_released_list() const;

  private:
    types type_;
    choice_buffer_t<bh_chs_required_to_be_released_list_l,
                    cause_c,
                    drbs_required_to_be_modified_list_l,
                    drbs_required_to_be_released_list_l,
                    du_to_cu_rrc_info_s,
                    pc5_rlc_ch_required_to_be_modified_list_l,
                    pc5_rlc_ch_required_to_be_released_list_l,
                    sl_drbs_required_to_be_modified_list_l,
                    sl_drbs_required_to_be_released_list_l,
                    srbs_required_to_be_released_list_l,
                    target_cell_list_l,
                    ue_multicast_m_rbs_required_to_be_modified_list_l,
                    ue_multicast_m_rbs_required_to_be_released_list_l,
                    unbounded_octstring<true>,
                    uu_rlc_ch_required_to_be_modified_list_l,
                    uu_rlc_ch_required_to_be_released_list_l>
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

// UEContextModificationResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_mod_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        res_coordination_transfer_container,
        du_to_cu_rrc_info,
        drbs_setup_mod_list,
        drbs_modified_list,
        srbs_failed_to_be_setup_mod_list,
        drbs_failed_to_be_setup_mod_list,
        scell_failedto_setup_mod_list,
        drbs_failed_to_be_modified_list,
        inactivity_monitoring_resp,
        crit_diagnostics,
        c_rnti,
        associated_scell_list,
        srbs_setup_mod_list,
        srbs_modified_list,
        full_cfg,
        bh_chs_setup_mod_list,
        bh_chs_modified_list,
        bh_chs_failed_to_be_setup_mod_list,
        bh_chs_failed_to_be_modified_list,
        sl_drbs_setup_mod_list,
        sl_drbs_modified_list,
        sl_drbs_failed_to_be_setup_mod_list,
        sl_drbs_failed_to_be_modified_list,
        requested_target_cell_global_id,
        scg_activation_status,
        uu_rlc_ch_setup_list,
        uu_rlc_ch_failed_to_be_setup_list,
        uu_rlc_ch_modified_list,
        uu_rlc_ch_failed_to_be_modified_list,
        pc5_rlc_ch_setup_list,
        pc5_rlc_ch_failed_to_be_setup_list,
        pc5_rlc_ch_modified_list,
        pc5_rlc_ch_failed_to_be_modified_list,
        sdt_bearer_cfg_info,
        ue_multicast_m_rbs_setup_list,
        serving_cell_mo_encoded_in_cgc_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                      gnb_cu_ue_f1ap_id();
    uint64_t&                                      gnb_du_ue_f1ap_id();
    unbounded_octstring<true>&                     res_coordination_transfer_container();
    du_to_cu_rrc_info_s&                           du_to_cu_rrc_info();
    drbs_setup_mod_list_l&                         drbs_setup_mod_list();
    drbs_modified_list_l&                          drbs_modified_list();
    srbs_failed_to_be_setup_mod_list_l&            srbs_failed_to_be_setup_mod_list();
    drbs_failed_to_be_setup_mod_list_l&            drbs_failed_to_be_setup_mod_list();
    scell_failedto_setup_mod_list_l&               scell_failedto_setup_mod_list();
    drbs_failed_to_be_modified_list_l&             drbs_failed_to_be_modified_list();
    inactivity_monitoring_resp_e&                  inactivity_monitoring_resp();
    crit_diagnostics_s&                            crit_diagnostics();
    uint32_t&                                      c_rnti();
    associated_scell_list_l&                       associated_scell_list();
    srbs_setup_mod_list_l&                         srbs_setup_mod_list();
    srbs_modified_list_l&                          srbs_modified_list();
    full_cfg_e&                                    full_cfg();
    bh_chs_setup_mod_list_l&                       bh_chs_setup_mod_list();
    bh_chs_modified_list_l&                        bh_chs_modified_list();
    bh_chs_failed_to_be_setup_mod_list_l&          bh_chs_failed_to_be_setup_mod_list();
    bh_chs_failed_to_be_modified_list_l&           bh_chs_failed_to_be_modified_list();
    sl_drbs_setup_mod_list_l&                      sl_drbs_setup_mod_list();
    sl_drbs_modified_list_l&                       sl_drbs_modified_list();
    sl_drbs_failed_to_be_setup_mod_list_l&         sl_drbs_failed_to_be_setup_mod_list();
    sl_drbs_failed_to_be_modified_list_l&          sl_drbs_failed_to_be_modified_list();
    nr_cgi_s&                                      requested_target_cell_global_id();
    scg_activation_status_e&                       scg_activation_status();
    uu_rlc_ch_setup_list_l&                        uu_rlc_ch_setup_list();
    uu_rlc_ch_failed_to_be_setup_list_l&           uu_rlc_ch_failed_to_be_setup_list();
    uu_rlc_ch_modified_list_l&                     uu_rlc_ch_modified_list();
    uu_rlc_ch_failed_to_be_modified_list_l&        uu_rlc_ch_failed_to_be_modified_list();
    pc5_rlc_ch_setup_list_l&                       pc5_rlc_ch_setup_list();
    pc5_rlc_ch_failed_to_be_setup_list_l&          pc5_rlc_ch_failed_to_be_setup_list();
    pc5_rlc_ch_modified_list_l&                    pc5_rlc_ch_modified_list();
    pc5_rlc_ch_failed_to_be_modified_list_l&       pc5_rlc_ch_failed_to_be_modified_list();
    sdt_bearer_cfg_info_s&                         sdt_bearer_cfg_info();
    ue_multicast_m_rbs_setup_list_l&               ue_multicast_m_rbs_setup_list();
    serving_cell_mo_encoded_in_cgc_list_l&         serving_cell_mo_encoded_in_cgc_list();
    const uint64_t&                                gnb_cu_ue_f1ap_id() const;
    const uint64_t&                                gnb_du_ue_f1ap_id() const;
    const unbounded_octstring<true>&               res_coordination_transfer_container() const;
    const du_to_cu_rrc_info_s&                     du_to_cu_rrc_info() const;
    const drbs_setup_mod_list_l&                   drbs_setup_mod_list() const;
    const drbs_modified_list_l&                    drbs_modified_list() const;
    const srbs_failed_to_be_setup_mod_list_l&      srbs_failed_to_be_setup_mod_list() const;
    const drbs_failed_to_be_setup_mod_list_l&      drbs_failed_to_be_setup_mod_list() const;
    const scell_failedto_setup_mod_list_l&         scell_failedto_setup_mod_list() const;
    const drbs_failed_to_be_modified_list_l&       drbs_failed_to_be_modified_list() const;
    const inactivity_monitoring_resp_e&            inactivity_monitoring_resp() const;
    const crit_diagnostics_s&                      crit_diagnostics() const;
    const uint32_t&                                c_rnti() const;
    const associated_scell_list_l&                 associated_scell_list() const;
    const srbs_setup_mod_list_l&                   srbs_setup_mod_list() const;
    const srbs_modified_list_l&                    srbs_modified_list() const;
    const full_cfg_e&                              full_cfg() const;
    const bh_chs_setup_mod_list_l&                 bh_chs_setup_mod_list() const;
    const bh_chs_modified_list_l&                  bh_chs_modified_list() const;
    const bh_chs_failed_to_be_setup_mod_list_l&    bh_chs_failed_to_be_setup_mod_list() const;
    const bh_chs_failed_to_be_modified_list_l&     bh_chs_failed_to_be_modified_list() const;
    const sl_drbs_setup_mod_list_l&                sl_drbs_setup_mod_list() const;
    const sl_drbs_modified_list_l&                 sl_drbs_modified_list() const;
    const sl_drbs_failed_to_be_setup_mod_list_l&   sl_drbs_failed_to_be_setup_mod_list() const;
    const sl_drbs_failed_to_be_modified_list_l&    sl_drbs_failed_to_be_modified_list() const;
    const nr_cgi_s&                                requested_target_cell_global_id() const;
    const scg_activation_status_e&                 scg_activation_status() const;
    const uu_rlc_ch_setup_list_l&                  uu_rlc_ch_setup_list() const;
    const uu_rlc_ch_failed_to_be_setup_list_l&     uu_rlc_ch_failed_to_be_setup_list() const;
    const uu_rlc_ch_modified_list_l&               uu_rlc_ch_modified_list() const;
    const uu_rlc_ch_failed_to_be_modified_list_l&  uu_rlc_ch_failed_to_be_modified_list() const;
    const pc5_rlc_ch_setup_list_l&                 pc5_rlc_ch_setup_list() const;
    const pc5_rlc_ch_failed_to_be_setup_list_l&    pc5_rlc_ch_failed_to_be_setup_list() const;
    const pc5_rlc_ch_modified_list_l&              pc5_rlc_ch_modified_list() const;
    const pc5_rlc_ch_failed_to_be_modified_list_l& pc5_rlc_ch_failed_to_be_modified_list() const;
    const sdt_bearer_cfg_info_s&                   sdt_bearer_cfg_info() const;
    const ue_multicast_m_rbs_setup_list_l&         ue_multicast_m_rbs_setup_list() const;
    const serving_cell_mo_encoded_in_cgc_list_l&   serving_cell_mo_encoded_in_cgc_list() const;

  private:
    types type_;
    choice_buffer_t<associated_scell_list_l,
                    bh_chs_failed_to_be_modified_list_l,
                    bh_chs_failed_to_be_setup_mod_list_l,
                    bh_chs_modified_list_l,
                    bh_chs_setup_mod_list_l,
                    crit_diagnostics_s,
                    drbs_failed_to_be_modified_list_l,
                    drbs_failed_to_be_setup_mod_list_l,
                    drbs_modified_list_l,
                    drbs_setup_mod_list_l,
                    du_to_cu_rrc_info_s,
                    nr_cgi_s,
                    pc5_rlc_ch_failed_to_be_modified_list_l,
                    pc5_rlc_ch_failed_to_be_setup_list_l,
                    pc5_rlc_ch_modified_list_l,
                    pc5_rlc_ch_setup_list_l,
                    scell_failedto_setup_mod_list_l,
                    sdt_bearer_cfg_info_s,
                    serving_cell_mo_encoded_in_cgc_list_l,
                    sl_drbs_failed_to_be_modified_list_l,
                    sl_drbs_failed_to_be_setup_mod_list_l,
                    sl_drbs_modified_list_l,
                    sl_drbs_setup_mod_list_l,
                    srbs_failed_to_be_setup_mod_list_l,
                    srbs_modified_list_l,
                    srbs_setup_mod_list_l,
                    ue_multicast_m_rbs_setup_list_l,
                    unbounded_octstring<true>,
                    uu_rlc_ch_failed_to_be_modified_list_l,
                    uu_rlc_ch_failed_to_be_setup_list_l,
                    uu_rlc_ch_modified_list_l,
                    uu_rlc_ch_setup_list_l>
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

// UEContextReleaseCommandIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_release_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        cause,
        rrc_container,
        srb_id,
        old_gnb_du_ue_f1ap_id,
        execute_dupl,
        rrc_delivery_status_request,
        target_cells_to_cancel,
        pos_conext_rev_ind,
        cg_sdt_kept_ind,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                            gnb_cu_ue_f1ap_id();
    uint64_t&                            gnb_du_ue_f1ap_id();
    cause_c&                             cause();
    unbounded_octstring<true>&           rrc_container();
    uint8_t&                             srb_id();
    uint64_t&                            old_gnb_du_ue_f1ap_id();
    execute_dupl_e&                      execute_dupl();
    rrc_delivery_status_request_e&       rrc_delivery_status_request();
    target_cell_list_l&                  target_cells_to_cancel();
    pos_conext_rev_ind_e&                pos_conext_rev_ind();
    cg_sdt_kept_ind_e&                   cg_sdt_kept_ind();
    const uint64_t&                      gnb_cu_ue_f1ap_id() const;
    const uint64_t&                      gnb_du_ue_f1ap_id() const;
    const cause_c&                       cause() const;
    const unbounded_octstring<true>&     rrc_container() const;
    const uint8_t&                       srb_id() const;
    const uint64_t&                      old_gnb_du_ue_f1ap_id() const;
    const execute_dupl_e&                execute_dupl() const;
    const rrc_delivery_status_request_e& rrc_delivery_status_request() const;
    const target_cell_list_l&            target_cells_to_cancel() const;
    const pos_conext_rev_ind_e&          pos_conext_rev_ind() const;
    const cg_sdt_kept_ind_e&             cg_sdt_kept_ind() const;

  private:
    types                                                                   type_;
    choice_buffer_t<cause_c, target_cell_list_l, unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextReleaseCompleteIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_release_complete_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    crit_diagnostics_s&       crit_diagnostics();
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                               type_;
    choice_buffer_t<crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextReleaseRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_release_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, cause, target_cells_to_cancel, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                 gnb_cu_ue_f1ap_id();
    uint64_t&                 gnb_du_ue_f1ap_id();
    cause_c&                  cause();
    target_cell_list_l&       target_cells_to_cancel();
    const uint64_t&           gnb_cu_ue_f1ap_id() const;
    const uint64_t&           gnb_du_ue_f1ap_id() const;
    const cause_c&            cause() const;
    const target_cell_list_l& target_cells_to_cancel() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, target_cell_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextSetupFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        cause,
        crit_diagnostics,
        potential_sp_cell_list,
        requested_target_cell_global_id,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                       gnb_cu_ue_f1ap_id();
    uint64_t&                       gnb_du_ue_f1ap_id();
    cause_c&                        cause();
    crit_diagnostics_s&             crit_diagnostics();
    potential_sp_cell_list_l&       potential_sp_cell_list();
    nr_cgi_s&                       requested_target_cell_global_id();
    const uint64_t&                 gnb_cu_ue_f1ap_id() const;
    const uint64_t&                 gnb_du_ue_f1ap_id() const;
    const cause_c&                  cause() const;
    const crit_diagnostics_s&       crit_diagnostics() const;
    const potential_sp_cell_list_l& potential_sp_cell_list() const;
    const nr_cgi_s&                 requested_target_cell_global_id() const;

  private:
    types                                                                            type_;
    choice_buffer_t<cause_c, crit_diagnostics_s, nr_cgi_s, potential_sp_cell_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEContextSetupRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        sp_cell_id,
        serv_cell_idx,
        sp_cell_ul_cfg,
        cu_to_du_rrc_info,
        candidate_sp_cell_list,
        drx_cycle,
        res_coordination_transfer_container,
        scell_to_be_setup_list,
        srbs_to_be_setup_list,
        drbs_to_be_setup_list,
        inactivity_monitoring_request,
        rat_freq_prio_info,
        rrc_container,
        masked_imeisv,
        serving_plmn,
        gnb_du_ue_ambr_ul,
        rrc_delivery_status_request,
        res_coordination_transfer_info,
        serving_cell_mo,
        new_gnb_cu_ue_f1ap_id,
        ran_ue_id,
        trace_activation,
        add_rrm_prio_idx,
        bh_chs_to_be_setup_list,
        cfg_bap_address,
        nr_v2x_services_authorized,
        ltev2x_services_authorized,
        nr_ue_sidelink_aggr_max_bitrate,
        lte_ue_sidelink_aggr_max_bitrate,
        pc5_link_ambr,
        sl_drbs_to_be_setup_list,
        conditional_inter_du_mob_info,
        management_based_mdt_plmn_list,
        serving_n_id,
        f1_c_transfer_path,
        f1_c_transfer_path_nr_dc,
        mdt_polluted_meas_ind,
        scg_activation_request,
        cg_sdt_session_info_old,
        five_g_pro_se_authorized,
        five_g_pro_se_ue_pc5_aggr_max_bitrate,
        five_g_pro_se_pc5_link_ambr,
        uu_rlc_ch_to_be_setup_list,
        pc5_rlc_ch_to_be_setup_list,
        path_switch_cfg,
        gnb_du_ue_slice_max_bit_rate_list,
        multicast_mbs_session_setup_list,
        ue_multicast_m_rbs_to_be_setup_list,
        serving_cell_mo_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                    gnb_cu_ue_f1ap_id();
    uint64_t&                                    gnb_du_ue_f1ap_id();
    nr_cgi_s&                                    sp_cell_id();
    uint8_t&                                     serv_cell_idx();
    cell_ul_cfg_e&                               sp_cell_ul_cfg();
    cu_to_du_rrc_info_s&                         cu_to_du_rrc_info();
    candidate_sp_cell_list_l&                    candidate_sp_cell_list();
    drx_cycle_s&                                 drx_cycle();
    unbounded_octstring<true>&                   res_coordination_transfer_container();
    scell_to_be_setup_list_l&                    scell_to_be_setup_list();
    srbs_to_be_setup_list_l&                     srbs_to_be_setup_list();
    drbs_to_be_setup_list_l&                     drbs_to_be_setup_list();
    inactivity_monitoring_request_e&             inactivity_monitoring_request();
    rat_freq_prio_info_c&                        rat_freq_prio_info();
    unbounded_octstring<true>&                   rrc_container();
    fixed_bitstring<64, false, true>&            masked_imeisv();
    fixed_octstring<3, true>&                    serving_plmn();
    uint64_t&                                    gnb_du_ue_ambr_ul();
    rrc_delivery_status_request_e&               rrc_delivery_status_request();
    res_coordination_transfer_info_s&            res_coordination_transfer_info();
    uint8_t&                                     serving_cell_mo();
    uint64_t&                                    new_gnb_cu_ue_f1ap_id();
    fixed_octstring<8, true>&                    ran_ue_id();
    trace_activation_s&                          trace_activation();
    fixed_bitstring<32, false, true>&            add_rrm_prio_idx();
    bh_chs_to_be_setup_list_l&                   bh_chs_to_be_setup_list();
    fixed_bitstring<10, false, true>&            cfg_bap_address();
    nr_v2x_services_authorized_s&                nr_v2x_services_authorized();
    ltev2x_services_authorized_s&                ltev2x_services_authorized();
    nr_ue_sidelink_aggr_max_bitrate_s&           nr_ue_sidelink_aggr_max_bitrate();
    lte_ue_sidelink_aggr_max_bitrate_s&          lte_ue_sidelink_aggr_max_bitrate();
    uint64_t&                                    pc5_link_ambr();
    sl_drbs_to_be_setup_list_l&                  sl_drbs_to_be_setup_list();
    conditional_inter_du_mob_info_s&             conditional_inter_du_mob_info();
    mdt_plmn_list_l&                             management_based_mdt_plmn_list();
    fixed_bitstring<44, false, true>&            serving_n_id();
    f1_c_transfer_path_s&                        f1_c_transfer_path();
    f1_c_transfer_path_nr_dc_s&                  f1_c_transfer_path_nr_dc();
    mdt_polluted_meas_ind_e&                     mdt_polluted_meas_ind();
    scg_activation_request_e&                    scg_activation_request();
    cg_sdt_session_info_s&                       cg_sdt_session_info_old();
    five_g_pro_se_authorized_s&                  five_g_pro_se_authorized();
    nr_ue_sidelink_aggr_max_bitrate_s&           five_g_pro_se_ue_pc5_aggr_max_bitrate();
    uint64_t&                                    five_g_pro_se_pc5_link_ambr();
    uu_rlc_ch_to_be_setup_list_l&                uu_rlc_ch_to_be_setup_list();
    pc5_rlc_ch_to_be_setup_list_l&               pc5_rlc_ch_to_be_setup_list();
    path_switch_cfg_s&                           path_switch_cfg();
    gnb_du_ue_slice_max_bit_rate_list_l&         gnb_du_ue_slice_max_bit_rate_list();
    multicast_mbs_session_list_l&                multicast_mbs_session_setup_list();
    ue_multicast_m_rbs_to_be_setup_list_l&       ue_multicast_m_rbs_to_be_setup_list();
    serving_cell_mo_list_l&                      serving_cell_mo_list();
    const uint64_t&                              gnb_cu_ue_f1ap_id() const;
    const uint64_t&                              gnb_du_ue_f1ap_id() const;
    const nr_cgi_s&                              sp_cell_id() const;
    const uint8_t&                               serv_cell_idx() const;
    const cell_ul_cfg_e&                         sp_cell_ul_cfg() const;
    const cu_to_du_rrc_info_s&                   cu_to_du_rrc_info() const;
    const candidate_sp_cell_list_l&              candidate_sp_cell_list() const;
    const drx_cycle_s&                           drx_cycle() const;
    const unbounded_octstring<true>&             res_coordination_transfer_container() const;
    const scell_to_be_setup_list_l&              scell_to_be_setup_list() const;
    const srbs_to_be_setup_list_l&               srbs_to_be_setup_list() const;
    const drbs_to_be_setup_list_l&               drbs_to_be_setup_list() const;
    const inactivity_monitoring_request_e&       inactivity_monitoring_request() const;
    const rat_freq_prio_info_c&                  rat_freq_prio_info() const;
    const unbounded_octstring<true>&             rrc_container() const;
    const fixed_bitstring<64, false, true>&      masked_imeisv() const;
    const fixed_octstring<3, true>&              serving_plmn() const;
    const uint64_t&                              gnb_du_ue_ambr_ul() const;
    const rrc_delivery_status_request_e&         rrc_delivery_status_request() const;
    const res_coordination_transfer_info_s&      res_coordination_transfer_info() const;
    const uint8_t&                               serving_cell_mo() const;
    const uint64_t&                              new_gnb_cu_ue_f1ap_id() const;
    const fixed_octstring<8, true>&              ran_ue_id() const;
    const trace_activation_s&                    trace_activation() const;
    const fixed_bitstring<32, false, true>&      add_rrm_prio_idx() const;
    const bh_chs_to_be_setup_list_l&             bh_chs_to_be_setup_list() const;
    const fixed_bitstring<10, false, true>&      cfg_bap_address() const;
    const nr_v2x_services_authorized_s&          nr_v2x_services_authorized() const;
    const ltev2x_services_authorized_s&          ltev2x_services_authorized() const;
    const nr_ue_sidelink_aggr_max_bitrate_s&     nr_ue_sidelink_aggr_max_bitrate() const;
    const lte_ue_sidelink_aggr_max_bitrate_s&    lte_ue_sidelink_aggr_max_bitrate() const;
    const uint64_t&                              pc5_link_ambr() const;
    const sl_drbs_to_be_setup_list_l&            sl_drbs_to_be_setup_list() const;
    const conditional_inter_du_mob_info_s&       conditional_inter_du_mob_info() const;
    const mdt_plmn_list_l&                       management_based_mdt_plmn_list() const;
    const fixed_bitstring<44, false, true>&      serving_n_id() const;
    const f1_c_transfer_path_s&                  f1_c_transfer_path() const;
    const f1_c_transfer_path_nr_dc_s&            f1_c_transfer_path_nr_dc() const;
    const mdt_polluted_meas_ind_e&               mdt_polluted_meas_ind() const;
    const scg_activation_request_e&              scg_activation_request() const;
    const cg_sdt_session_info_s&                 cg_sdt_session_info_old() const;
    const five_g_pro_se_authorized_s&            five_g_pro_se_authorized() const;
    const nr_ue_sidelink_aggr_max_bitrate_s&     five_g_pro_se_ue_pc5_aggr_max_bitrate() const;
    const uint64_t&                              five_g_pro_se_pc5_link_ambr() const;
    const uu_rlc_ch_to_be_setup_list_l&          uu_rlc_ch_to_be_setup_list() const;
    const pc5_rlc_ch_to_be_setup_list_l&         pc5_rlc_ch_to_be_setup_list() const;
    const path_switch_cfg_s&                     path_switch_cfg() const;
    const gnb_du_ue_slice_max_bit_rate_list_l&   gnb_du_ue_slice_max_bit_rate_list() const;
    const multicast_mbs_session_list_l&          multicast_mbs_session_setup_list() const;
    const ue_multicast_m_rbs_to_be_setup_list_l& ue_multicast_m_rbs_to_be_setup_list() const;
    const serving_cell_mo_list_l&                serving_cell_mo_list() const;

  private:
    types type_;
    choice_buffer_t<bh_chs_to_be_setup_list_l,
                    candidate_sp_cell_list_l,
                    cg_sdt_session_info_s,
                    conditional_inter_du_mob_info_s,
                    cu_to_du_rrc_info_s,
                    drbs_to_be_setup_list_l,
                    drx_cycle_s,
                    f1_c_transfer_path_nr_dc_s,
                    f1_c_transfer_path_s,
                    five_g_pro_se_authorized_s,
                    fixed_bitstring<64, false, true>,
                    fixed_octstring<3, true>,
                    fixed_octstring<8, true>,
                    gnb_du_ue_slice_max_bit_rate_list_l,
                    lte_ue_sidelink_aggr_max_bitrate_s,
                    ltev2x_services_authorized_s,
                    mdt_plmn_list_l,
                    multicast_mbs_session_list_l,
                    nr_cgi_s,
                    nr_ue_sidelink_aggr_max_bitrate_s,
                    nr_v2x_services_authorized_s,
                    path_switch_cfg_s,
                    pc5_rlc_ch_to_be_setup_list_l,
                    rat_freq_prio_info_c,
                    res_coordination_transfer_info_s,
                    scell_to_be_setup_list_l,
                    serving_cell_mo_list_l,
                    sl_drbs_to_be_setup_list_l,
                    srbs_to_be_setup_list_l,
                    trace_activation_s,
                    ue_multicast_m_rbs_to_be_setup_list_l,
                    unbounded_octstring<true>,
                    uu_rlc_ch_to_be_setup_list_l>
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

// UEContextSetupResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_context_setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        du_to_cu_rrc_info,
        c_rnti,
        res_coordination_transfer_container,
        full_cfg,
        drbs_setup_list,
        srbs_failed_to_be_setup_list,
        drbs_failed_to_be_setup_list,
        scell_failedto_setup_list,
        inactivity_monitoring_resp,
        crit_diagnostics,
        srbs_setup_list,
        bh_chs_setup_list,
        bh_chs_failed_to_be_setup_list,
        sl_drbs_setup_list,
        sl_drbs_failed_to_be_setup_list,
        requested_target_cell_global_id,
        scg_activation_status,
        uu_rlc_ch_setup_list,
        uu_rlc_ch_failed_to_be_setup_list,
        pc5_rlc_ch_setup_list,
        pc5_rlc_ch_failed_to_be_setup_list,
        serving_cell_mo_encoded_in_cgc_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                                    gnb_cu_ue_f1ap_id();
    uint64_t&                                    gnb_du_ue_f1ap_id();
    du_to_cu_rrc_info_s&                         du_to_cu_rrc_info();
    uint32_t&                                    c_rnti();
    unbounded_octstring<true>&                   res_coordination_transfer_container();
    full_cfg_e&                                  full_cfg();
    drbs_setup_list_l&                           drbs_setup_list();
    srbs_failed_to_be_setup_list_l&              srbs_failed_to_be_setup_list();
    drbs_failed_to_be_setup_list_l&              drbs_failed_to_be_setup_list();
    scell_failedto_setup_list_l&                 scell_failedto_setup_list();
    inactivity_monitoring_resp_e&                inactivity_monitoring_resp();
    crit_diagnostics_s&                          crit_diagnostics();
    srbs_setup_list_l&                           srbs_setup_list();
    bh_chs_setup_list_l&                         bh_chs_setup_list();
    bh_chs_failed_to_be_setup_list_l&            bh_chs_failed_to_be_setup_list();
    sl_drbs_setup_list_l&                        sl_drbs_setup_list();
    sl_drbs_failed_to_be_setup_list_l&           sl_drbs_failed_to_be_setup_list();
    nr_cgi_s&                                    requested_target_cell_global_id();
    scg_activation_status_e&                     scg_activation_status();
    uu_rlc_ch_setup_list_l&                      uu_rlc_ch_setup_list();
    uu_rlc_ch_failed_to_be_setup_list_l&         uu_rlc_ch_failed_to_be_setup_list();
    pc5_rlc_ch_setup_list_l&                     pc5_rlc_ch_setup_list();
    pc5_rlc_ch_failed_to_be_setup_list_l&        pc5_rlc_ch_failed_to_be_setup_list();
    serving_cell_mo_encoded_in_cgc_list_l&       serving_cell_mo_encoded_in_cgc_list();
    const uint64_t&                              gnb_cu_ue_f1ap_id() const;
    const uint64_t&                              gnb_du_ue_f1ap_id() const;
    const du_to_cu_rrc_info_s&                   du_to_cu_rrc_info() const;
    const uint32_t&                              c_rnti() const;
    const unbounded_octstring<true>&             res_coordination_transfer_container() const;
    const full_cfg_e&                            full_cfg() const;
    const drbs_setup_list_l&                     drbs_setup_list() const;
    const srbs_failed_to_be_setup_list_l&        srbs_failed_to_be_setup_list() const;
    const drbs_failed_to_be_setup_list_l&        drbs_failed_to_be_setup_list() const;
    const scell_failedto_setup_list_l&           scell_failedto_setup_list() const;
    const inactivity_monitoring_resp_e&          inactivity_monitoring_resp() const;
    const crit_diagnostics_s&                    crit_diagnostics() const;
    const srbs_setup_list_l&                     srbs_setup_list() const;
    const bh_chs_setup_list_l&                   bh_chs_setup_list() const;
    const bh_chs_failed_to_be_setup_list_l&      bh_chs_failed_to_be_setup_list() const;
    const sl_drbs_setup_list_l&                  sl_drbs_setup_list() const;
    const sl_drbs_failed_to_be_setup_list_l&     sl_drbs_failed_to_be_setup_list() const;
    const nr_cgi_s&                              requested_target_cell_global_id() const;
    const scg_activation_status_e&               scg_activation_status() const;
    const uu_rlc_ch_setup_list_l&                uu_rlc_ch_setup_list() const;
    const uu_rlc_ch_failed_to_be_setup_list_l&   uu_rlc_ch_failed_to_be_setup_list() const;
    const pc5_rlc_ch_setup_list_l&               pc5_rlc_ch_setup_list() const;
    const pc5_rlc_ch_failed_to_be_setup_list_l&  pc5_rlc_ch_failed_to_be_setup_list() const;
    const serving_cell_mo_encoded_in_cgc_list_l& serving_cell_mo_encoded_in_cgc_list() const;

  private:
    types type_;
    choice_buffer_t<bh_chs_failed_to_be_setup_list_l,
                    bh_chs_setup_list_l,
                    crit_diagnostics_s,
                    drbs_failed_to_be_setup_list_l,
                    drbs_setup_list_l,
                    du_to_cu_rrc_info_s,
                    nr_cgi_s,
                    pc5_rlc_ch_failed_to_be_setup_list_l,
                    pc5_rlc_ch_setup_list_l,
                    scell_failedto_setup_list_l,
                    serving_cell_mo_encoded_in_cgc_list_l,
                    sl_drbs_failed_to_be_setup_list_l,
                    sl_drbs_setup_list_l,
                    srbs_failed_to_be_setup_list_l,
                    srbs_setup_list_l,
                    unbounded_octstring<true>,
                    uu_rlc_ch_failed_to_be_setup_list_l,
                    uu_rlc_ch_setup_list_l>
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

// UEInactivityNotificationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_inactivity_notif_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, drb_activity_list, sdt_termination_request, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                        gnb_cu_ue_f1ap_id();
    uint64_t&                        gnb_du_ue_f1ap_id();
    drb_activity_list_l&             drb_activity_list();
    sdt_termination_request_e&       sdt_termination_request();
    const uint64_t&                  gnb_cu_ue_f1ap_id() const;
    const uint64_t&                  gnb_du_ue_f1ap_id() const;
    const drb_activity_list_l&       drb_activity_list() const;
    const sdt_termination_request_e& sdt_termination_request() const;

  private:
    types                                type_;
    choice_buffer_t<drb_activity_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ULRRCMessageTransferIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ul_rrc_msg_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        gnb_cu_ue_f1ap_id,
        gnb_du_ue_f1ap_id,
        srb_id,
        rrc_container,
        sel_plmn_id,
        new_gnb_du_ue_f1ap_id,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint64_t&                        gnb_cu_ue_f1ap_id();
    uint64_t&                        gnb_du_ue_f1ap_id();
    uint8_t&                         srb_id();
    unbounded_octstring<true>&       rrc_container();
    fixed_octstring<3, true>&        sel_plmn_id();
    uint64_t&                        new_gnb_du_ue_f1ap_id();
    const uint64_t&                  gnb_cu_ue_f1ap_id() const;
    const uint64_t&                  gnb_du_ue_f1ap_id() const;
    const uint8_t&                   srb_id() const;
    const unbounded_octstring<true>& rrc_container() const;
    const fixed_octstring<3, true>&  sel_plmn_id() const;
    const uint64_t&                  new_gnb_du_ue_f1ap_id() const;

  private:
    types                                                                type_;
    choice_buffer_t<fixed_octstring<3, true>, unbounded_octstring<true>> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// WriteReplaceWarningRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct write_replace_warning_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        pws_sys_info,
        repeat_period,
        numof_broadcast_request,
        cells_to_be_broadcast_list,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                           transaction_id();
    pws_sys_info_s&                     pws_sys_info();
    uint32_t&                           repeat_period();
    uint32_t&                           numof_broadcast_request();
    cells_to_be_broadcast_list_l&       cells_to_be_broadcast_list();
    const uint16_t&                     transaction_id() const;
    const pws_sys_info_s&               pws_sys_info() const;
    const uint32_t&                     repeat_period() const;
    const uint32_t&                     numof_broadcast_request() const;
    const cells_to_be_broadcast_list_l& cells_to_be_broadcast_list() const;

  private:
    types                                                         type_;
    choice_buffer_t<cells_to_be_broadcast_list_l, pws_sys_info_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// WriteReplaceWarningResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct write_replace_warning_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        cells_broadcast_completed_list,
        crit_diagnostics,
        ded_si_delivery_needed_ue_list,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

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
    uint16_t&                               transaction_id();
    cells_broadcast_completed_list_l&       cells_broadcast_completed_list();
    crit_diagnostics_s&                     crit_diagnostics();
    ded_si_delivery_needed_ue_list_l&       ded_si_delivery_needed_ue_list();
    const uint16_t&                         transaction_id() const;
    const cells_broadcast_completed_list_l& cells_broadcast_completed_list() const;
    const crit_diagnostics_s&               crit_diagnostics() const;
    const ded_si_delivery_needed_ue_list_l& ded_si_delivery_needed_ue_list() const;

  private:
    types                                                                                                   type_;
    choice_buffer_t<cells_broadcast_completed_list_l, crit_diagnostics_s, ded_si_delivery_needed_ue_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct f1_removal_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cause_c>                               cause;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  f1_removal_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1RemovalFailure ::= SEQUENCE
using f1_removal_fail_s = elementary_procedure_option<f1_removal_fail_ies_container>;

// F1RemovalRequest ::= SEQUENCE
using f1_removal_request_s = elementary_procedure_option<protocol_ie_container_l<f1_removal_request_ies_o>>;

struct f1_removal_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  f1_removal_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1RemovalResponse ::= SEQUENCE
using f1_removal_resp_s = elementary_procedure_option<f1_removal_resp_ies_container>;

struct f1_setup_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              time_to_wait_present     = false;
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cause_c>                               cause;
  ie_field_s<time_to_wait_e>                        time_to_wait;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  f1_setup_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1SetupFailure ::= SEQUENCE
using f1_setup_fail_s = elementary_procedure_option<f1_setup_fail_ies_container>;

struct f1_setup_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                       gnb_du_name_present                  = false;
  bool                                                       gnb_du_served_cells_list_present     = false;
  bool                                                       transport_layer_address_info_present = false;
  bool                                                       bap_address_present                  = false;
  bool                                                       extended_gnb_du_name_present         = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>          transaction_id;
  ie_field_s<integer<uint64_t, 0, 68719476735, false, true>> gnb_du_id;
  ie_field_s<printable_string<1, 150, true, true>>           gnb_du_name;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<gnb_du_served_cells_item_ies_o>, 1, 512, true>>
                                               gnb_du_served_cells_list;
  ie_field_s<rrc_version_s>                    gnb_du_rrc_version;
  ie_field_s<transport_layer_address_info_s>   transport_layer_address_info;
  ie_field_s<fixed_bitstring<10, false, true>> bap_address;
  ie_field_s<extended_gnb_du_name_s>           extended_gnb_du_name;

  // sequence methods
  f1_setup_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1SetupRequest ::= SEQUENCE
using f1_setup_request_s = elementary_procedure_option<f1_setup_request_ies_container>;

struct f1_setup_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              gnb_cu_name_present                  = false;
  bool                                              cells_to_be_activ_list_present       = false;
  bool                                              transport_layer_address_info_present = false;
  bool                                              ul_bh_non_up_traffic_map_present     = false;
  bool                                              bap_address_present                  = false;
  bool                                              extended_gnb_cu_name_present         = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<printable_string<1, 150, true, true>>  gnb_cu_name;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_to_be_activ_list_item_ies_o>, 1, 512, true>>
                                               cells_to_be_activ_list;
  ie_field_s<rrc_version_s>                    gnb_cu_rrc_version;
  ie_field_s<transport_layer_address_info_s>   transport_layer_address_info;
  ie_field_s<ul_bh_non_up_traffic_map_s>       ul_bh_non_up_traffic_map;
  ie_field_s<fixed_bitstring<10, false, true>> bap_address;
  ie_field_s<extended_gnb_cu_name_s>           extended_gnb_cu_name;

  // sequence methods
  f1_setup_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1SetupResponse ::= SEQUENCE
using f1_setup_resp_s = elementary_procedure_option<f1_setup_resp_ies_container>;

struct gnb_cu_cfg_upd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              cells_to_be_activ_list_present       = false;
  bool                                              cells_to_be_deactiv_list_present     = false;
  bool                                              gnb_cu_tnl_assoc_to_add_list_present = false;
  bool                                              gnb_cu_tnl_assoc_to_rem_list_present = false;
  bool                                              gnb_cu_tnl_assoc_to_upd_list_present = false;
  bool                                              cells_to_be_barred_list_present      = false;
  bool                                              protected_eutra_res_list_present     = false;
  bool                                              neighbour_cell_info_list_present     = false;
  bool                                              transport_layer_address_info_present = false;
  bool                                              ul_bh_non_up_traffic_map_present     = false;
  bool                                              bap_address_present                  = false;
  bool                                              cco_assist_info_present              = false;
  bool                                              cells_for_son_list_present           = false;
  bool                                              gnb_cu_name_present                  = false;
  bool                                              extended_gnb_cu_name_present         = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_to_be_activ_list_item_ies_o>, 1, 512, true>>
      cells_to_be_activ_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_to_be_deactiv_list_item_ies_o>, 1, 512, true>>
      cells_to_be_deactiv_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_add_item_ies_o>, 1, 32, true>>
      gnb_cu_tnl_assoc_to_add_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_rem_item_ies_o>, 1, 32, true>>
      gnb_cu_tnl_assoc_to_rem_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_upd_item_ies_o>, 1, 32, true>>
      gnb_cu_tnl_assoc_to_upd_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_to_be_barred_item_ies_o>, 1, 512, true>>
      cells_to_be_barred_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<protected_eutra_res_item_ies_o>, 1, 256, true>>
      protected_eutra_res_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<neighbour_cell_info_item_ies_o>, 1, 512, true>>
                                                             neighbour_cell_info_list;
  ie_field_s<transport_layer_address_info_s>                 transport_layer_address_info;
  ie_field_s<ul_bh_non_up_traffic_map_s>                     ul_bh_non_up_traffic_map;
  ie_field_s<fixed_bitstring<10, false, true>>               bap_address;
  ie_field_s<cco_assist_info_s>                              cco_assist_info;
  ie_field_s<dyn_seq_of<cells_for_son_item_s, 1, 256, true>> cells_for_son_list;
  ie_field_s<printable_string<1, 150, true, true>>           gnb_cu_name;
  ie_field_s<extended_gnb_cu_name_s>                         extended_gnb_cu_name;

  // sequence methods
  gnb_cu_cfg_upd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBCUConfigurationUpdate ::= SEQUENCE
using gnb_cu_cfg_upd_s = elementary_procedure_option<gnb_cu_cfg_upd_ies_container>;

struct gnb_cu_cfg_upd_ack_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              cells_failed_to_be_activ_list_present         = false;
  bool                                              crit_diagnostics_present                      = false;
  bool                                              gnb_cu_tnl_assoc_setup_list_present           = false;
  bool                                              gnb_cu_tnl_assoc_failed_to_setup_list_present = false;
  bool                                              ded_si_delivery_needed_ue_list_present        = false;
  bool                                              transport_layer_address_info_present          = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_failed_to_be_activ_list_item_ies_o>, 1, 512, true>>
                                 cells_failed_to_be_activ_list;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<gnb_cu_tnl_assoc_setup_item_ies_o>, 1, 32, true>>
      gnb_cu_tnl_assoc_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<gnb_cu_tnl_assoc_failed_to_setup_item_ies_o>, 1, 32, true>>
      gnb_cu_tnl_assoc_failed_to_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ded_si_delivery_needed_ue_item_ies_o>, 1, 65536, true>>
                                             ded_si_delivery_needed_ue_list;
  ie_field_s<transport_layer_address_info_s> transport_layer_address_info;

  // sequence methods
  gnb_cu_cfg_upd_ack_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBCUConfigurationUpdateAcknowledge ::= SEQUENCE
using gnb_cu_cfg_upd_ack_s = elementary_procedure_option<gnb_cu_cfg_upd_ack_ies_container>;

struct gnb_cu_cfg_upd_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              time_to_wait_present     = false;
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cause_c>                               cause;
  ie_field_s<time_to_wait_e>                        time_to_wait;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  gnb_cu_cfg_upd_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBCUConfigurationUpdateFailure ::= SEQUENCE
using gnb_cu_cfg_upd_fail_s = elementary_procedure_option<gnb_cu_cfg_upd_fail_ies_container>;

struct gnb_du_cfg_upd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              served_cells_to_add_list_present       = false;
  bool                                              served_cells_to_modify_list_present    = false;
  bool                                              served_cells_to_delete_list_present    = false;
  bool                                              cells_status_list_present              = false;
  bool                                              ded_si_delivery_needed_ue_list_present = false;
  bool                                              gnb_du_id_present                      = false;
  bool                                              gnb_du_tnl_assoc_to_rem_list_present   = false;
  bool                                              transport_layer_address_info_present   = false;
  bool                                              coverage_mod_notif_present             = false;
  bool                                              gnb_du_name_present                    = false;
  bool                                              extended_gnb_du_name_present           = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<served_cells_to_add_item_ies_o>, 1, 512, true>>
      served_cells_to_add_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<served_cells_to_modify_item_ies_o>, 1, 512, true>>
      served_cells_to_modify_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<served_cells_to_delete_item_ies_o>, 1, 512, true>>
      served_cells_to_delete_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_status_item_ies_o>, 0, 512, true>> cells_status_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ded_si_delivery_needed_ue_item_ies_o>, 1, 65536, true>>
                                                             ded_si_delivery_needed_ue_list;
  ie_field_s<integer<uint64_t, 0, 68719476735, false, true>> gnb_du_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<gnb_du_tnl_assoc_to_rem_item_ies_o>, 1, 32, true>>
                                                   gnb_du_tnl_assoc_to_rem_list;
  ie_field_s<transport_layer_address_info_s>       transport_layer_address_info;
  ie_field_s<coverage_mod_notif_s>                 coverage_mod_notif;
  ie_field_s<printable_string<1, 150, true, true>> gnb_du_name;
  ie_field_s<extended_gnb_du_name_s>               extended_gnb_du_name;

  // sequence methods
  gnb_du_cfg_upd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUConfigurationUpdate ::= SEQUENCE
using gnb_du_cfg_upd_s = elementary_procedure_option<gnb_du_cfg_upd_ies_container>;

struct gnb_du_cfg_upd_ack_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              cells_to_be_activ_list_present       = false;
  bool                                              crit_diagnostics_present             = false;
  bool                                              cells_to_be_deactiv_list_present     = false;
  bool                                              transport_layer_address_info_present = false;
  bool                                              ul_bh_non_up_traffic_map_present     = false;
  bool                                              bap_address_present                  = false;
  bool                                              cells_for_son_list_present           = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_to_be_activ_list_item_ies_o>, 1, 512, true>>
                                 cells_to_be_activ_list;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_to_be_deactiv_list_item_ies_o>, 1, 512, true>>
                                                             cells_to_be_deactiv_list;
  ie_field_s<transport_layer_address_info_s>                 transport_layer_address_info;
  ie_field_s<ul_bh_non_up_traffic_map_s>                     ul_bh_non_up_traffic_map;
  ie_field_s<fixed_bitstring<10, false, true>>               bap_address;
  ie_field_s<dyn_seq_of<cells_for_son_item_s, 1, 256, true>> cells_for_son_list;

  // sequence methods
  gnb_du_cfg_upd_ack_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUConfigurationUpdateAcknowledge ::= SEQUENCE
using gnb_du_cfg_upd_ack_s = elementary_procedure_option<gnb_du_cfg_upd_ack_ies_container>;

struct gnb_du_cfg_upd_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              time_to_wait_present     = false;
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cause_c>                               cause;
  ie_field_s<time_to_wait_e>                        time_to_wait;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  gnb_du_cfg_upd_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUConfigurationUpdateFailure ::= SEQUENCE
using gnb_du_cfg_upd_fail_s = elementary_procedure_option<gnb_du_cfg_upd_fail_ies_container>;

struct gnb_du_res_cfg_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                                    activ_cells_to_be_upd_list_present = false;
  bool                                                                    child_nodes_list_present           = false;
  bool                                                                    neighbour_node_cells_list_present  = false;
  bool                                                                    serving_cells_list_present         = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>                       transaction_id;
  ie_field_s<dyn_seq_of<activ_cells_to_be_upd_list_item_s, 1, 512, true>> activ_cells_to_be_upd_list;
  ie_field_s<dyn_seq_of<child_nodes_list_item_s, 1, 1024, true>>          child_nodes_list;
  ie_field_s<dyn_seq_of<neighbour_node_cells_list_item_s, 1, 1024, true>> neighbour_node_cells_list;
  ie_field_s<dyn_seq_of<serving_cells_list_item_s, 1, 32, true>>          serving_cells_list;

  // sequence methods
  gnb_du_res_cfg_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUResourceConfiguration ::= SEQUENCE
using gnb_du_res_cfg_s = elementary_procedure_option<gnb_du_res_cfg_ies_container>;

struct gnb_du_res_cfg_ack_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  gnb_du_res_cfg_ack_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUResourceConfigurationAcknowledge ::= SEQUENCE
using gnb_du_res_cfg_ack_s = elementary_procedure_option<gnb_du_res_cfg_ack_ies_container>;

struct gnb_du_res_cfg_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              time_to_wait_present     = false;
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cause_c>                               cause;
  ie_field_s<time_to_wait_e>                        time_to_wait;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  gnb_du_res_cfg_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUResourceConfigurationFailure ::= SEQUENCE
using gnb_du_res_cfg_fail_s = elementary_procedure_option<gnb_du_res_cfg_fail_ies_container>;

struct gnb_du_res_coordination_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              ignore_res_coordination_container_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<request_type_e>                        request_type;
  ie_field_s<unbounded_octstring<true>>             eutra_nr_cell_res_coordination_req_container;
  ie_field_s<ignore_res_coordination_container_e>   ignore_res_coordination_container;

  // sequence methods
  gnb_du_res_coordination_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUResourceCoordinationRequest ::= SEQUENCE
using gnb_du_res_coordination_request_s = elementary_procedure_option<gnb_du_res_coordination_request_ies_container>;

struct gnb_du_res_coordination_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<unbounded_octstring<true>>             eutra_nr_cell_res_coordination_req_ack_container;

  // sequence methods
  gnb_du_res_coordination_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUResourceCoordinationResponse ::= SEQUENCE
using gnb_du_res_coordination_resp_s = elementary_procedure_option<gnb_du_res_coordination_resp_ies_container>;

struct gnb_du_status_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              iab_congestion_ind_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<gnb_du_overload_info_e>                gnb_du_overload_info;
  ie_field_s<iab_congestion_ind_s>                  iab_congestion_ind;

  // sequence methods
  gnb_du_status_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUStatusIndication ::= SEQUENCE
using gnb_du_status_ind_s = elementary_procedure_option<gnb_du_status_ind_ies_container>;

struct iab_tnl_address_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              time_to_wait_present     = false;
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cause_c>                               cause;
  ie_field_s<time_to_wait_e>                        time_to_wait;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  iab_tnl_address_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IABTNLAddressFailure ::= SEQUENCE
using iab_tnl_address_fail_s = elementary_procedure_option<iab_tnl_address_fail_ies_container>;

struct iab_tnl_address_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              ia_bv4_addresses_requested_present    = false;
  bool                                              iab_ip_v6_request_type_present        = false;
  bool                                              iab_tnl_addresses_to_rem_list_present = false;
  bool                                              iab_tnl_addresses_exception_present   = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<ia_bv4_addresses_requested_s>          ia_bv4_addresses_requested;
  ie_field_s<iab_ip_v6_request_type_c>              iab_ip_v6_request_type;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<iab_tnl_addresses_to_rem_item_ies_o>, 1, 1024, true>>
                                            iab_tnl_addresses_to_rem_list;
  ie_field_s<iab_tnl_addresses_exception_s> iab_tnl_addresses_exception;

  // sequence methods
  iab_tnl_address_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IABTNLAddressRequest ::= SEQUENCE
using iab_tnl_address_request_s = elementary_procedure_option<iab_tnl_address_request_ies_container>;

struct iab_tnl_address_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<iab_allocated_tnl_address_list_item_ies_o>, 1, 1024, true>>
      iab_allocated_tnl_address_list;

  // sequence methods
  iab_tnl_address_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IABTNLAddressResponse ::= SEQUENCE
using iab_tnl_address_resp_s = elementary_procedure_option<iab_tnl_address_resp_ies_container>;

struct iab_up_cfg_upd_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              time_to_wait_present     = false;
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cause_c>                               cause;
  ie_field_s<time_to_wait_e>                        time_to_wait;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  iab_up_cfg_upd_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IABUPConfigurationUpdateFailure ::= SEQUENCE
using iab_up_cfg_upd_fail_s = elementary_procedure_option<iab_up_cfg_upd_fail_ies_container>;

struct iab_up_cfg_upd_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              ul_up_tnl_info_to_upd_list_present    = false;
  bool                                              ul_up_tnl_address_to_upd_list_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ul_up_tnl_info_to_upd_list_item_ies_o>, 1, 32678, true>>
      ul_up_tnl_info_to_upd_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ul_up_tnl_address_to_upd_list_item_ies_o>, 1, 8, true>>
      ul_up_tnl_address_to_upd_list;

  // sequence methods
  iab_up_cfg_upd_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IABUPConfigurationUpdateRequest ::= SEQUENCE
using iab_up_cfg_upd_request_s = elementary_procedure_option<iab_up_cfg_upd_request_ies_container>;

struct iab_up_cfg_upd_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              crit_diagnostics_present              = false;
  bool                                              dl_up_tnl_address_to_upd_list_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<dl_up_tnl_address_to_upd_list_item_ies_o>, 1, 8, true>>
      dl_up_tnl_address_to_upd_list;

  // sequence methods
  iab_up_cfg_upd_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IABUPConfigurationUpdateResponse ::= SEQUENCE
using iab_up_cfg_upd_resp_s = elementary_procedure_option<iab_up_cfg_upd_resp_ies_container>;

struct init_ul_rrc_msg_transfer_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      du_to_cu_rrc_container_present           = false;
  bool                                                      sul_access_ind_present                   = false;
  bool                                                      ran_ue_id_present                        = false;
  bool                                                      rrc_container_rrc_setup_complete_present = false;
  bool                                                      nr_red_cap_ue_ind_present                = false;
  bool                                                      sdt_info_present                         = false;
  bool                                                      sidelink_relay_cfg_present               = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<nr_cgi_s>                                      nr_cgi;
  ie_field_s<integer<uint32_t, 0, 65535, true, true>>       c_rnti;
  ie_field_s<unbounded_octstring<true>>                     rrc_container;
  ie_field_s<unbounded_octstring<true>>                     du_to_cu_rrc_container;
  ie_field_s<sul_access_ind_e>                              sul_access_ind;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>         transaction_id;
  ie_field_s<fixed_octstring<8, true>>                      ran_ue_id;
  ie_field_s<unbounded_octstring<true>>                     rrc_container_rrc_setup_complete;
  ie_field_s<nr_red_cap_ue_ind_e>                           nr_red_cap_ue_ind;
  ie_field_s<sdt_info_s>                                    sdt_info;
  ie_field_s<sidelink_relay_cfg_s>                          sidelink_relay_cfg;

  // sequence methods
  init_ul_rrc_msg_transfer_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InitialULRRCMessageTransfer ::= SEQUENCE
using init_ul_rrc_msg_transfer_s = elementary_procedure_option<init_ul_rrc_msg_transfer_ies_container>;

struct meas_activation_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                          prs_meas_info_list_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>     gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>     gnb_du_ue_f1ap_id;
  ie_field_s<activation_request_type_e>                         activation_request_type;
  ie_field_s<dyn_seq_of<prs_meas_info_list_item_s, 1, 4, true>> prs_meas_info_list;

  // sequence methods
  meas_activation_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementActivation ::= SEQUENCE
using meas_activation_s = elementary_procedure_option<meas_activation_ies_container>;

struct meas_precfg_confirm_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      pos_meas_gap_pre_cfg_list_present = false;
  bool                                                      crit_diagnostics_present          = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<pos_meas_gap_pre_cfg_list_s>                   pos_meas_gap_pre_cfg_list;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  meas_precfg_confirm_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementPreconfigurationConfirm ::= SEQUENCE
using meas_precfg_confirm_s = elementary_procedure_option<meas_precfg_confirm_ies_container>;

struct meas_precfg_refuse_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  meas_precfg_refuse_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementPreconfigurationRefuse ::= SEQUENCE
using meas_precfg_refuse_s = elementary_procedure_option<meas_precfg_refuse_ies_container>;

struct meas_precfg_required_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>      gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>      gnb_du_ue_f1ap_id;
  ie_field_s<dyn_seq_of<trp_prs_info_list_item_s, 1, 256, true>> trp_prs_info_list;

  // sequence methods
  meas_precfg_required_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementPreconfigurationRequired ::= SEQUENCE
using meas_precfg_required_s = elementary_procedure_option<meas_precfg_required_ies_container>;

struct multicast_context_mod_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  multicast_context_mod_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextModificationFailure ::= SEQUENCE
using multicast_context_mod_fail_s = elementary_procedure_option<multicast_context_mod_fail_ies_container>;

struct multicast_context_mod_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      mbs_service_area_present                     = false;
  bool                                                      multicast_m_rbs_to_be_setup_mod_list_present = false;
  bool                                                      multicast_m_rbs_to_be_modified_list_present  = false;
  bool                                                      multicast_m_rbs_to_be_released_list_present  = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<mbs_service_area_c>                            mbs_service_area;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<multicast_m_rbs_to_be_setup_mod_item_ies_o>, 1, 32, true>>
      multicast_m_rbs_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<multicast_m_rbs_to_be_modified_item_ies_o>, 1, 32, true>>
      multicast_m_rbs_to_be_modified_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<multicast_m_rbs_to_be_released_item_ies_o>, 1, 32, true>>
      multicast_m_rbs_to_be_released_list;

  // sequence methods
  multicast_context_mod_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextModificationRequest ::= SEQUENCE
using multicast_context_mod_request_s = elementary_procedure_option<multicast_context_mod_request_ies_container>;

struct multicast_context_mod_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      multicast_m_rbs_setup_mod_list_present              = false;
  bool                                                      multicast_m_rbs_failed_to_be_setup_mod_list_present = false;
  bool                                                      multicast_m_rbs_modified_list_present               = false;
  bool                                                      multicast_m_rbs_failed_to_be_modified_list_present  = false;
  bool                                                      crit_diagnostics_present                            = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<multicast_m_rbs_setup_mod_item_ies_o>, 1, 32, true>>
      multicast_m_rbs_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<multicast_m_rbs_failed_to_be_setup_mod_item_ies_o>, 1, 32, true>>
      multicast_m_rbs_failed_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<multicast_m_rbs_modified_item_ies_o>, 1, 32, true>>
      multicast_m_rbs_modified_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<multicast_m_rbs_failed_to_be_modified_item_ies_o>, 1, 32, true>>
                                 multicast_m_rbs_failed_to_be_modified_list;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;

  // sequence methods
  multicast_context_mod_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextModificationResponse ::= SEQUENCE
using multicast_context_mod_resp_s = elementary_procedure_option<multicast_context_mod_resp_ies_container>;

struct multicast_context_release_cmd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<cause_c>                                       cause;

  // sequence methods
  multicast_context_release_cmd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextReleaseCommand ::= SEQUENCE
using multicast_context_release_cmd_s = elementary_procedure_option<multicast_context_release_cmd_ies_container>;

struct multicast_context_release_complete_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  multicast_context_release_complete_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextReleaseComplete ::= SEQUENCE
using multicast_context_release_complete_s =
    elementary_procedure_option<multicast_context_release_complete_ies_container>;

struct multicast_context_release_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<cause_c>                                       cause;

  // sequence methods
  multicast_context_release_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextReleaseRequest ::= SEQUENCE
using multicast_context_release_request_s =
    elementary_procedure_option<multicast_context_release_request_ies_container>;

struct multicast_context_setup_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      gnb_du_mbs_f1ap_id_present = false;
  bool                                                      crit_diagnostics_present   = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  multicast_context_setup_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextSetupFailure ::= SEQUENCE
using multicast_context_setup_fail_s = elementary_procedure_option<multicast_context_setup_fail_ies_container>;

struct multicast_context_setup_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      mbs_service_area_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<mbs_session_id_s>                              mbs_session_id;
  ie_field_s<mbs_service_area_c>                            mbs_service_area;
  ie_field_s<snssai_s>                                      snssai;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<multicast_m_rbs_to_be_setup_item_ies_o>, 1, 32, true>>
      multicast_m_rbs_to_be_setup_list;

  // sequence methods
  multicast_context_setup_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextSetupRequest ::= SEQUENCE
using multicast_context_setup_request_s = elementary_procedure_option<multicast_context_setup_request_ies_container>;

struct multicast_context_setup_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      multicast_m_rbs_failed_to_be_setup_list_present = false;
  bool                                                      crit_diagnostics_present                        = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<multicast_m_rbs_setup_item_ies_o>, 1, 32, true>>
      multicast_m_rbs_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<multicast_m_rbs_failed_to_be_setup_item_ies_o>, 1, 32, true>>
                                 multicast_m_rbs_failed_to_be_setup_list;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;

  // sequence methods
  multicast_context_setup_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextSetupResponse ::= SEQUENCE
using multicast_context_setup_resp_s = elementary_procedure_option<multicast_context_setup_resp_ies_container>;

struct multicast_distribution_release_cmd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<mbs_multicast_f1_u_context_descriptor_s>       mbs_multicast_f1_u_context_descriptor;
  ie_field_s<cause_c>                                       cause;

  // sequence methods
  multicast_distribution_release_cmd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastDistributionReleaseCommand ::= SEQUENCE
using multicast_distribution_release_cmd_s =
    elementary_procedure_option<multicast_distribution_release_cmd_ies_container>;

struct multicast_distribution_release_complete_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<mbs_multicast_f1_u_context_descriptor_s>       mbs_multicast_f1_u_context_descriptor;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  multicast_distribution_release_complete_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastDistributionReleaseComplete ::= SEQUENCE
using multicast_distribution_release_complete_s =
    elementary_procedure_option<multicast_distribution_release_complete_ies_container>;

struct multicast_distribution_setup_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      gnb_du_mbs_f1ap_id_present = false;
  bool                                                      crit_diagnostics_present   = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<mbs_multicast_f1_u_context_descriptor_s>       mbs_multicast_f1_u_context_descriptor;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  multicast_distribution_setup_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastDistributionSetupFailure ::= SEQUENCE
using multicast_distribution_setup_fail_s =
    elementary_procedure_option<multicast_distribution_setup_fail_ies_container>;

struct multicast_distribution_setup_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<mbs_multicast_f1_u_context_descriptor_s>       mbs_multicast_f1_u_context_descriptor;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<multicast_f1_u_context_to_be_setup_item_ies_o>, 1, 32, true>>
      multicast_f1_u_context_to_be_setup_list;

  // sequence methods
  multicast_distribution_setup_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastDistributionSetupRequest ::= SEQUENCE
using multicast_distribution_setup_request_s =
    elementary_procedure_option<multicast_distribution_setup_request_ies_container>;

struct multicast_distribution_setup_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool multicast_f1_u_context_failed_to_be_setup_list_present = false;
  bool crit_diagnostics_present                               = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_mbs_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_mbs_f1ap_id;
  ie_field_s<mbs_multicast_f1_u_context_descriptor_s>       mbs_multicast_f1_u_context_descriptor;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<multicast_f1_u_context_setup_item_ies_o>, 1, 32, true>>
      multicast_f1_u_context_setup_list;
  ie_field_s<
      dyn_seq_of<protocol_ie_single_container_s<multicast_f1_u_context_failed_to_be_setup_item_ies_o>, 1, 32, true>>
                                       multicast_f1_u_context_failed_to_be_setup_list;
  ie_field_s<crit_diagnostics_s>       crit_diagnostics;
  ie_field_s<fixed_octstring<4, true>> multicast_f1_u_context_ref_cu;

  // sequence methods
  multicast_distribution_setup_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastDistributionSetupResponse ::= SEQUENCE
using multicast_distribution_setup_resp_s =
    elementary_procedure_option<multicast_distribution_setup_resp_ies_container>;

struct multicast_group_paging_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                         ue_id_list_for_paging_list_present = false;
  bool                         mc_paging_cell_list_present        = false;
  ie_field_s<mbs_session_id_s> mbs_session_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ue_id_list_for_paging_item_ies_o>, 1, 4096, true>>
      ue_id_list_for_paging_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<mc_paging_cell_item_ies_o>, 1, 512, true>> mc_paging_cell_list;

  // sequence methods
  multicast_group_paging_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastGroupPaging ::= SEQUENCE
using multicast_group_paging_s = elementary_procedure_option<multicast_group_paging_ies_container>;

struct network_access_rate_reduction_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<uac_assist_info_s>                     uac_assist_info;

  // sequence methods
  network_access_rate_reduction_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NetworkAccessRateReduction ::= SEQUENCE
using network_access_rate_reduction_s = elementary_procedure_option<network_access_rate_reduction_ies_container>;

struct notify_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>                                  gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>                                  gnb_du_ue_f1ap_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drb_notify_item_ies_o>, 1, 64, true>> drb_notify_list;

  // sequence methods
  notify_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Notify ::= SEQUENCE
using notify_s = elementary_procedure_option<notify_ies_container>;

struct pdc_meas_fail_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint8_t, 1, 16, true, true>>           ran_ue_pdc_meas_id;
  ie_field_s<cause_c>                                       cause;

  // sequence methods
  pdc_meas_fail_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCMeasurementFailureIndication ::= SEQUENCE
using pdc_meas_fail_ind_s = elementary_procedure_option<pdc_meas_fail_ind_ies_container>;

struct pdc_meas_initiation_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint8_t, 1, 16, true, true>>           ran_ue_pdc_meas_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  pdc_meas_initiation_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCMeasurementInitiationFailure ::= SEQUENCE
using pdc_meas_initiation_fail_s = elementary_procedure_option<pdc_meas_initiation_fail_ies_container>;

struct pdc_meas_initiation_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      pdc_meas_periodicity_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint8_t, 1, 16, true, true>>           ran_ue_pdc_meas_id;
  ie_field_s<pdc_report_type_e>                             pdc_report_type;
  ie_field_s<pdc_meas_periodicity_e>                        pdc_meas_periodicity;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<pdc_meas_quantities_item_ies_o>, 1, 16, true>>
      pdc_meas_quantities;

  // sequence methods
  pdc_meas_initiation_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCMeasurementInitiationRequest ::= SEQUENCE
using pdc_meas_initiation_request_s = elementary_procedure_option<pdc_meas_initiation_request_ies_container>;

struct pdc_meas_initiation_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      pdc_meas_result_present  = false;
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint8_t, 1, 16, true, true>>           ran_ue_pdc_meas_id;
  ie_field_s<pdc_meas_result_s>                             pdc_meas_result;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  pdc_meas_initiation_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCMeasurementInitiationResponse ::= SEQUENCE
using pdc_meas_initiation_resp_s = elementary_procedure_option<pdc_meas_initiation_resp_ies_container>;

struct pdc_meas_report_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint8_t, 1, 16, true, true>>           ran_ue_pdc_meas_id;
  ie_field_s<pdc_meas_result_s>                             pdc_meas_result;

  // sequence methods
  pdc_meas_report_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCMeasurementReport ::= SEQUENCE
using pdc_meas_report_s = elementary_procedure_option<pdc_meas_report_ies_container>;

struct pdc_meas_termination_cmd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint8_t, 1, 16, true, true>>           ran_ue_pdc_meas_id;

  // sequence methods
  pdc_meas_termination_cmd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCMeasurementTerminationCommand ::= SEQUENCE
using pdc_meas_termination_cmd_s = elementary_procedure_option<pdc_meas_termination_cmd_ies_container>;

struct prs_cfg_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cause_c>                               cause;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  prs_cfg_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSConfigurationFailure ::= SEQUENCE
using prs_cfg_fail_s = elementary_procedure_option<prs_cfg_fail_ies_container>;

struct prs_cfg_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<prs_cfg_request_type_e>                    prs_cfg_request_type;
  ie_field_s<dyn_seq_of<prstrp_item_s, 1, 65535, true>> prstrp_list;

  // sequence methods
  prs_cfg_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSConfigurationRequest ::= SEQUENCE
using prs_cfg_request_s = elementary_procedure_option<prs_cfg_request_ies_container>;

struct prs_cfg_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      prs_tx_trp_list_present  = false;
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>         transaction_id;
  ie_field_s<dyn_seq_of<prs_tx_trp_item_s, 1, 65535, true>> prs_tx_trp_list;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  prs_cfg_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSConfigurationResponse ::= SEQUENCE
using prs_cfg_resp_s = elementary_procedure_option<prs_cfg_resp_ies_container>;

struct pws_cancel_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                 broadcast_to_be_cancelled_list_present = false;
  bool                                                 cancel_all_warning_msgs_ind_present    = false;
  bool                                                 notif_info_present                     = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>    transaction_id;
  ie_field_s<integer<uint32_t, 0, 65535, false, true>> numof_broadcast_request;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<broadcast_to_be_cancelled_list_item_ies_o>, 1, 512, true>>
                                            broadcast_to_be_cancelled_list;
  ie_field_s<cancel_all_warning_msgs_ind_e> cancel_all_warning_msgs_ind;
  ie_field_s<notif_info_s>                  notif_info;

  // sequence methods
  pws_cancel_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSCancelRequest ::= SEQUENCE
using pws_cancel_request_s = elementary_procedure_option<pws_cancel_request_ies_container>;

struct pws_cancel_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              cells_broadcast_cancelled_list_present = false;
  bool                                              crit_diagnostics_present               = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_broadcast_cancelled_list_item_ies_o>, 1, 512, true>>
                                 cells_broadcast_cancelled_list;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;

  // sequence methods
  pws_cancel_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSCancelResponse ::= SEQUENCE
using pws_cancel_resp_s = elementary_procedure_option<pws_cancel_resp_ies_container>;

struct pws_fail_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              pws_failed_nr_cgi_list_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<pws_failed_nr_cgi_list_item_ies_o>, 1, 512, true>>
      pws_failed_nr_cgi_list;

  // sequence methods
  pws_fail_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSFailureIndication ::= SEQUENCE
using pws_fail_ind_s = elementary_procedure_option<pws_fail_ind_ies_container>;

struct pws_restart_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<nr_cgi_list_for_restart_list_item_ies_o>, 1, 512, true>>
      nr_cgi_list_for_restart_list;

  // sequence methods
  pws_restart_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSRestartIndication ::= SEQUENCE
using pws_restart_ind_s = elementary_procedure_option<pws_restart_ind_ies_container>;

struct paging_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                          paging_drx_present                          = false;
  bool                          paging_prio_present                         = false;
  bool                          paging_origin_present                       = false;
  bool                          ran_ue_paging_drx_present                   = false;
  bool                          cn_ue_paging_drx_present                    = false;
  bool                          nr_paginge_drx_info_present                 = false;
  bool                          nr_paginge_drx_infofor_rrc_inactive_present = false;
  bool                          paging_cause_present                        = false;
  bool                          pe_ip_s_assist_info_present                 = false;
  bool                          ue_paging_cap_present                       = false;
  bool                          extended_ue_id_idx_value_present            = false;
  ie_field_s<ue_id_idx_value_c> ue_id_idx_value;
  ie_field_s<paging_id_c>       paging_id;
  ie_field_s<paging_drx_e>      paging_drx;
  ie_field_s<paging_prio_e>     paging_prio;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<paging_cell_item_ies_o>, 1, 512, true>> paging_cell_list;
  ie_field_s<paging_origin_e>                                                                  paging_origin;
  ie_field_s<paging_drx_e>                                                                     ran_ue_paging_drx;
  ie_field_s<paging_drx_e>                                                                     cn_ue_paging_drx;
  ie_field_s<nr_paginge_drx_info_s>                                                            nr_paginge_drx_info;
  ie_field_s<nr_paginge_drx_infofor_rrc_inactive_s> nr_paginge_drx_infofor_rrc_inactive;
  ie_field_s<paging_cause_e>                        paging_cause;
  ie_field_s<pe_ip_s_assist_info_s>                 pe_ip_s_assist_info;
  ie_field_s<ue_paging_cap_s>                       ue_paging_cap;
  ie_field_s<fixed_bitstring<16, false, true>>      extended_ue_id_idx_value;

  // sequence methods
  paging_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Paging ::= SEQUENCE
using paging_s = elementary_procedure_option<paging_ies_container>;

struct pos_sys_info_delivery_cmd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>>         transaction_id;
  ie_field_s<nr_cgi_s>                                      nr_cgi;
  ie_field_s<dyn_seq_of<pos_sitype_item_s, 1, 32, true>>    pos_sitype_list;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> confirmed_ue_id;

  // sequence methods
  pos_sys_info_delivery_cmd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PosSystemInformationDeliveryCommand ::= SEQUENCE
using pos_sys_info_delivery_cmd_s = elementary_procedure_option<pos_sys_info_delivery_cmd_ies_container>;

struct positioning_activation_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  positioning_activation_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningActivationFailure ::= SEQUENCE
using positioning_activation_fail_s = elementary_procedure_option<positioning_activation_fail_ies_container>;

struct positioning_activation_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      activation_time_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<srs_type_c>                                    srs_type;
  ie_field_s<fixed_bitstring<64, false, true>>              activation_time;

  // sequence methods
  positioning_activation_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningActivationRequest ::= SEQUENCE
using positioning_activation_request_s = elementary_procedure_option<positioning_activation_request_ies_container>;

struct positioning_activation_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      sys_frame_num_present    = false;
  bool                                                      slot_num_present         = false;
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint16_t, 0, 1023, false, true>>       sys_frame_num;
  ie_field_s<integer<uint8_t, 0, 79, false, true>>          slot_num;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  positioning_activation_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningActivationResponse ::= SEQUENCE
using positioning_activation_resp_s = elementary_procedure_option<positioning_activation_resp_ies_container>;

struct positioning_assist_info_ctrl_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              pos_assist_info_present             = false;
  bool                                              pos_broadcast_present               = false;
  bool                                              positioning_broadcast_cells_present = false;
  bool                                              routing_id_present                  = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<unbounded_octstring<true>>             pos_assist_info;
  ie_field_s<pos_broadcast_e>                       pos_broadcast;
  ie_field_s<dyn_seq_of<nr_cgi_s, 1, 16384, true>>  positioning_broadcast_cells;
  ie_field_s<unbounded_octstring<true>>             routing_id;

  // sequence methods
  positioning_assist_info_ctrl_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningAssistanceInformationControl ::= SEQUENCE
using positioning_assist_info_ctrl_s = elementary_procedure_option<positioning_assist_info_ctrl_ies_container>;

struct positioning_assist_info_feedback_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              pos_assist_info_fail_list_present   = false;
  bool                                              positioning_broadcast_cells_present = false;
  bool                                              routing_id_present                  = false;
  bool                                              crit_diagnostics_present            = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<unbounded_octstring<true>>             pos_assist_info_fail_list;
  ie_field_s<dyn_seq_of<nr_cgi_s, 1, 16384, true>>  positioning_broadcast_cells;
  ie_field_s<unbounded_octstring<true>>             routing_id;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  positioning_assist_info_feedback_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningAssistanceInformationFeedback ::= SEQUENCE
using positioning_assist_info_feedback_s = elementary_procedure_option<positioning_assist_info_feedback_ies_container>;

struct positioning_deactivation_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<abort_tx_c>                                    abort_tx;

  // sequence methods
  positioning_deactivation_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningDeactivation ::= SEQUENCE
using positioning_deactivation_s = elementary_procedure_option<positioning_deactivation_ies_container>;

struct positioning_info_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  positioning_info_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningInformationFailure ::= SEQUENCE
using positioning_info_fail_s = elementary_procedure_option<positioning_info_fail_ies_container>;

struct positioning_info_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      requested_srs_tx_characteristics_present = false;
  bool                                                      ue_report_info_present                   = false;
  bool                                                      srs_pos_rrc_inactive_query_ind_present   = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<requested_srs_tx_characteristics_s>            requested_srs_tx_characteristics;
  ie_field_s<ue_report_info_s>                              ue_report_info;
  ie_field_s<srs_pos_rrc_inactive_query_ind_e>              srs_pos_rrc_inactive_query_ind;

  // sequence methods
  positioning_info_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningInformationRequest ::= SEQUENCE
using positioning_info_request_s = elementary_procedure_option<positioning_info_request_ies_container>;

struct positioning_info_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      srs_cfg_present                  = false;
  bool                                                      sfn_initisation_time_present     = false;
  bool                                                      crit_diagnostics_present         = false;
  bool                                                      srs_pos_rrc_inactive_cfg_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<srs_cfg_s>                                     srs_cfg;
  ie_field_s<fixed_bitstring<64, false, true>>              sfn_initisation_time;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;
  ie_field_s<unbounded_octstring<true>>                     srs_pos_rrc_inactive_cfg;

  // sequence methods
  positioning_info_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningInformationResponse ::= SEQUENCE
using positioning_info_resp_s = elementary_procedure_option<positioning_info_resp_ies_container>;

struct positioning_info_upd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      srs_cfg_present              = false;
  bool                                                      sfn_initisation_time_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<srs_cfg_s>                                     srs_cfg;
  ie_field_s<fixed_bitstring<64, false, true>>              sfn_initisation_time;

  // sequence methods
  positioning_info_upd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningInformationUpdate ::= SEQUENCE
using positioning_info_upd_s = elementary_procedure_option<positioning_info_upd_ies_container>;

struct positioning_meas_abort_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>>   transaction_id;
  ie_field_s<integer<uint32_t, 1, 65536, true, true>> lmf_meas_id;
  ie_field_s<integer<uint32_t, 1, 65536, true, true>> ran_meas_id;

  // sequence methods
  positioning_meas_abort_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningMeasurementAbort ::= SEQUENCE
using positioning_meas_abort_s = elementary_procedure_option<positioning_meas_abort_ies_container>;

struct positioning_meas_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>   transaction_id;
  ie_field_s<integer<uint32_t, 1, 65536, true, true>> lmf_meas_id;
  ie_field_s<integer<uint32_t, 1, 65536, true, true>> ran_meas_id;
  ie_field_s<cause_c>                                 cause;
  ie_field_s<crit_diagnostics_s>                      crit_diagnostics;

  // sequence methods
  positioning_meas_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningMeasurementFailure ::= SEQUENCE
using positioning_meas_fail_s = elementary_procedure_option<positioning_meas_fail_ies_container>;

struct positioning_meas_fail_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>>   transaction_id;
  ie_field_s<integer<uint32_t, 1, 65536, true, true>> lmf_meas_id;
  ie_field_s<integer<uint32_t, 1, 65536, true, true>> ran_meas_id;
  ie_field_s<cause_c>                                 cause;

  // sequence methods
  positioning_meas_fail_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningMeasurementFailureIndication ::= SEQUENCE
using positioning_meas_fail_ind_s = elementary_procedure_option<positioning_meas_fail_ind_ies_container>;

struct positioning_meas_report_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>>                transaction_id;
  ie_field_s<integer<uint32_t, 1, 65536, true, true>>              lmf_meas_id;
  ie_field_s<integer<uint32_t, 1, 65536, true, true>>              ran_meas_id;
  ie_field_s<dyn_seq_of<pos_meas_result_list_item_s, 1, 64, true>> pos_meas_result_list;

  // sequence methods
  positioning_meas_report_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningMeasurementReport ::= SEQUENCE
using positioning_meas_report_s = elementary_procedure_option<positioning_meas_report_ies_container>;

struct positioning_meas_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                               pos_meas_periodicity_present             = false;
  bool                                                               sfn_initisation_time_present             = false;
  bool                                                               srs_cfg_present                          = false;
  bool                                                               meas_beam_info_request_present           = false;
  bool                                                               sys_frame_num_present                    = false;
  bool                                                               slot_num_present                         = false;
  bool                                                               pos_meas_periodicity_extended_present    = false;
  bool                                                               resp_time_present                        = false;
  bool                                                               meas_characteristics_request_ind_present = false;
  bool                                                               meas_time_occasion_present               = false;
  bool                                                               pos_meas_amount_present                  = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>                  transaction_id;
  ie_field_s<integer<uint32_t, 1, 65536, true, true>>                lmf_meas_id;
  ie_field_s<integer<uint32_t, 1, 65536, true, true>>                ran_meas_id;
  ie_field_s<dyn_seq_of<trp_meas_request_item_s, 1, 64, true>>       trp_meas_request_list;
  ie_field_s<pos_report_characteristics_e>                           pos_report_characteristics;
  ie_field_s<meas_periodicity_e>                                     pos_meas_periodicity;
  ie_field_s<dyn_seq_of<pos_meas_quantities_item_s, 1, 16384, true>> pos_meas_quantities;
  ie_field_s<fixed_bitstring<64, false, true>>                       sfn_initisation_time;
  ie_field_s<srs_cfg_s>                                              srs_cfg;
  ie_field_s<meas_beam_info_request_e>                               meas_beam_info_request;
  ie_field_s<integer<uint16_t, 0, 1023, false, true>>                sys_frame_num;
  ie_field_s<integer<uint8_t, 0, 79, false, true>>                   slot_num;
  ie_field_s<meas_periodicity_extended_e>                            pos_meas_periodicity_extended;
  ie_field_s<resp_time_s>                                            resp_time;
  ie_field_s<fixed_bitstring<16, false, true>>                       meas_characteristics_request_ind;
  ie_field_s<meas_time_occasion_e>                                   meas_time_occasion;
  ie_field_s<pos_meas_amount_e>                                      pos_meas_amount;

  // sequence methods
  positioning_meas_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningMeasurementRequest ::= SEQUENCE
using positioning_meas_request_s = elementary_procedure_option<positioning_meas_request_ies_container>;

struct positioning_meas_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                             pos_meas_result_list_present = false;
  bool                                                             crit_diagnostics_present     = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>                transaction_id;
  ie_field_s<integer<uint32_t, 1, 65536, true, true>>              lmf_meas_id;
  ie_field_s<integer<uint32_t, 1, 65536, true, true>>              ran_meas_id;
  ie_field_s<dyn_seq_of<pos_meas_result_list_item_s, 1, 64, true>> pos_meas_result_list;
  ie_field_s<crit_diagnostics_s>                                   crit_diagnostics;

  // sequence methods
  positioning_meas_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningMeasurementResponse ::= SEQUENCE
using positioning_meas_resp_s = elementary_procedure_option<positioning_meas_resp_ies_container>;

struct positioning_meas_upd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                     srs_cfg_present                          = false;
  bool                                                     trp_meas_upd_list_present                = false;
  bool                                                     meas_characteristics_request_ind_present = false;
  bool                                                     meas_time_occasion_present               = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>        transaction_id;
  ie_field_s<integer<uint32_t, 1, 65536, true, true>>      lmf_meas_id;
  ie_field_s<integer<uint32_t, 1, 65536, true, true>>      ran_meas_id;
  ie_field_s<srs_cfg_s>                                    srs_cfg;
  ie_field_s<dyn_seq_of<trp_meas_upd_item_s, 1, 64, true>> trp_meas_upd_list;
  ie_field_s<fixed_bitstring<16, false, true>>             meas_characteristics_request_ind;
  ie_field_s<meas_time_occasion_e>                         meas_time_occasion;

  // sequence methods
  positioning_meas_upd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningMeasurementUpdate ::= SEQUENCE
using positioning_meas_upd_s = elementary_procedure_option<positioning_meas_upd_ies_container>;

struct private_ie_container_empty_l {
  template <class valueT_>
  using ie_field_s = private_ie_container_item_s<valueT_>;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};
using private_msg_ies_container = private_ie_container_empty_l;

// PrivateMessage ::= SEQUENCE
struct private_msg_s {
  bool                      ext = false;
  private_msg_ies_container private_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct qo_e_info_transfer_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      qo_e_info_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<qo_e_info_s>                                   qo_e_info;

  // sequence methods
  qo_e_info_transfer_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoEInformationTransfer ::= SEQUENCE
using qo_e_info_transfer_s = elementary_procedure_option<qo_e_info_transfer_ies_container>;

struct rrc_delivery_report_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<rrc_delivery_status_s>                         rrc_delivery_status;
  ie_field_s<integer<uint8_t, 0, 3, true, true>>            srb_id;

  // sequence methods
  rrc_delivery_report_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCDeliveryReport ::= SEQUENCE
using rrc_delivery_report_s = elementary_procedure_option<rrc_delivery_report_ies_container>;

struct ref_time_info_report_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<time_ref_info_s>                       time_ref_info;

  // sequence methods
  ref_time_info_report_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReferenceTimeInformationReport ::= SEQUENCE
using ref_time_info_report_s = elementary_procedure_option<ref_time_info_report_ies_container>;

struct ref_time_info_report_ctrl_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<report_request_type_s>                 report_request_type;

  // sequence methods
  ref_time_info_report_ctrl_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReferenceTimeInformationReportingControl ::= SEQUENCE
using ref_time_info_report_ctrl_s = elementary_procedure_option<ref_time_info_report_ctrl_ies_container>;

struct reset_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cause_c>                               cause;
  ie_field_s<reset_type_c>                          reset_type;

  // sequence methods
  reset_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Reset ::= SEQUENCE
using reset_s = elementary_procedure_option<reset_ies_container>;

struct reset_ack_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              ue_associated_lc_f1_conn_list_res_ack_present = false;
  bool                                              crit_diagnostics_present                      = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ue_associated_lc_f1_conn_item_res_ack_o>, 1, 65536, true>>
                                 ue_associated_lc_f1_conn_list_res_ack;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;

  // sequence methods
  reset_ack_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResetAcknowledge ::= SEQUENCE
using reset_ack_s = elementary_procedure_option<reset_ack_ies_container>;

struct res_status_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>  transaction_id;
  ie_field_s<integer<uint16_t, 0, 4095, true, true>> gnb_cu_meas_id;
  ie_field_s<integer<uint16_t, 0, 4095, true, true>> gnb_du_meas_id;
  ie_field_s<cause_c>                                cause;
  ie_field_s<crit_diagnostics_s>                     crit_diagnostics;

  // sequence methods
  res_status_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceStatusFailure ::= SEQUENCE
using res_status_fail_s = elementary_procedure_option<res_status_fail_ies_container>;

struct res_status_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                        gnb_du_meas_id_present         = false;
  bool                                                        report_characteristics_present = false;
  bool                                                        cell_to_report_list_present    = false;
  bool                                                        report_periodicity_present     = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>           transaction_id;
  ie_field_s<integer<uint16_t, 0, 4095, true, true>>          gnb_cu_meas_id;
  ie_field_s<integer<uint16_t, 0, 4095, true, true>>          gnb_du_meas_id;
  ie_field_s<regist_request_e>                                regist_request;
  ie_field_s<fixed_bitstring<32, false, true>>                report_characteristics;
  ie_field_s<dyn_seq_of<cell_to_report_item_s, 1, 512, true>> cell_to_report_list;
  ie_field_s<report_periodicity_e>                            report_periodicity;

  // sequence methods
  res_status_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceStatusRequest ::= SEQUENCE
using res_status_request_s = elementary_procedure_option<res_status_request_ies_container>;

struct res_status_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>  transaction_id;
  ie_field_s<integer<uint16_t, 0, 4095, true, true>> gnb_cu_meas_id;
  ie_field_s<integer<uint16_t, 0, 4095, true, true>> gnb_du_meas_id;
  ie_field_s<crit_diagnostics_s>                     crit_diagnostics;

  // sequence methods
  res_status_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceStatusResponse ::= SEQUENCE
using res_status_resp_s = elementary_procedure_option<res_status_resp_ies_container>;

struct res_status_upd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                          hardware_load_ind_present     = false;
  bool                                                          tnl_capacity_ind_present      = false;
  bool                                                          cell_meas_result_list_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>             transaction_id;
  ie_field_s<integer<uint16_t, 0, 4095, true, true>>            gnb_cu_meas_id;
  ie_field_s<integer<uint16_t, 0, 4095, true, true>>            gnb_du_meas_id;
  ie_field_s<hardware_load_ind_s>                               hardware_load_ind;
  ie_field_s<tnl_capacity_ind_s>                                tnl_capacity_ind;
  ie_field_s<dyn_seq_of<cell_meas_result_item_s, 1, 512, true>> cell_meas_result_list;

  // sequence methods
  res_status_upd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceStatusUpdate ::= SEQUENCE
using res_status_upd_s = elementary_procedure_option<res_status_upd_ies_container>;

struct sys_info_delivery_cmd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true>>         transaction_id;
  ie_field_s<nr_cgi_s>                                      nr_cgi;
  ie_field_s<dyn_seq_of<sitype_item_s, 1, 32, true>>        sitype_list;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> confirmed_ue_id;

  // sequence methods
  sys_info_delivery_cmd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationDeliveryCommand ::= SEQUENCE
using sys_info_delivery_cmd_s = elementary_procedure_option<sys_info_delivery_cmd_ies_container>;

struct trp_info_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<cause_c>                               cause;
  ie_field_s<crit_diagnostics_s>                    crit_diagnostics;

  // sequence methods
  trp_info_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRPInformationFailure ::= SEQUENCE
using trp_info_fail_s = elementary_procedure_option<trp_info_fail_ies_container>;

struct trp_info_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                    trp_list_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>       transaction_id;
  ie_field_s<dyn_seq_of<trp_list_item_s, 1, 65535, true>> trp_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<trp_info_type_item_trp_req_o>, 1, 64, true>>
      trp_info_type_list_trp_req;

  // sequence methods
  trp_info_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRPInformationRequest ::= SEQUENCE
using trp_info_request_s = elementary_procedure_option<trp_info_request_ies_container>;

struct trp_info_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<trp_info_item_trp_resp_o>, 1, 65535, true>>
                                 trp_info_list_trp_resp;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;

  // sequence methods
  trp_info_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRPInformationResponse ::= SEQUENCE
using trp_info_resp_s = elementary_procedure_option<trp_info_resp_ies_container>;

struct trace_start_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<trace_activation_s>                            trace_activation;

  // sequence methods
  trace_start_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TraceStart ::= SEQUENCE
using trace_start_s = elementary_procedure_option<trace_start_ies_container>;

struct ue_context_mod_confirm_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool res_coordination_transfer_container_present              = false;
  bool drbs_modified_conf_list_present                          = false;
  bool rrc_container_present                                    = false;
  bool crit_diagnostics_present                                 = false;
  bool execute_dupl_present                                     = false;
  bool res_coordination_transfer_info_present                   = false;
  bool sl_drbs_modified_conf_list_present                       = false;
  bool uu_rlc_ch_modified_list_present                          = false;
  bool pc5_rlc_ch_modified_list_present                         = false;
  bool ue_multicast_m_rbs_confirmed_to_be_modified_list_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<unbounded_octstring<true>>                     res_coordination_transfer_container;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_modified_conf_item_ies_o>, 1, 64, true>>
                                               drbs_modified_conf_list;
  ie_field_s<unbounded_octstring<true>>        rrc_container;
  ie_field_s<crit_diagnostics_s>               crit_diagnostics;
  ie_field_s<execute_dupl_e>                   execute_dupl;
  ie_field_s<res_coordination_transfer_info_s> res_coordination_transfer_info;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<sl_drbs_modified_conf_item_ies_o>, 1, 512, true>>
                                                                   sl_drbs_modified_conf_list;
  ie_field_s<dyn_seq_of<uu_rlc_ch_modified_item_s, 1, 32, true>>   uu_rlc_ch_modified_list;
  ie_field_s<dyn_seq_of<pc5_rlc_ch_modified_item_s, 1, 512, true>> pc5_rlc_ch_modified_list;
  ie_field_s<
      dyn_seq_of<protocol_ie_single_container_s<ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o>, 1, 64, true>>
      ue_multicast_m_rbs_confirmed_to_be_modified_list;

  // sequence methods
  ue_context_mod_confirm_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationConfirm ::= SEQUENCE
using ue_context_mod_confirm_s = elementary_procedure_option<ue_context_mod_confirm_ies_container>;

struct ue_context_mod_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      crit_diagnostics_present                = false;
  bool                                                      requested_target_cell_global_id_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;
  ie_field_s<nr_cgi_s>                                      requested_target_cell_global_id;

  // sequence methods
  ue_context_mod_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationFailure ::= SEQUENCE
using ue_context_mod_fail_s = elementary_procedure_option<ue_context_mod_fail_ies_container>;

struct ue_context_mod_refuse_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  ue_context_mod_refuse_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationRefuse ::= SEQUENCE
using ue_context_mod_refuse_s = elementary_procedure_option<ue_context_mod_refuse_ies_container>;

struct ue_context_mod_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool sp_cell_id_present                                    = false;
  bool serv_cell_idx_present                                 = false;
  bool sp_cell_ul_cfg_present                                = false;
  bool drx_cycle_present                                     = false;
  bool cu_to_du_rrc_info_present                             = false;
  bool tx_action_ind_present                                 = false;
  bool res_coordination_transfer_container_present           = false;
  bool rrc_recfg_complete_ind_present                        = false;
  bool rrc_container_present                                 = false;
  bool scell_to_be_setup_mod_list_present                    = false;
  bool scell_to_be_remd_list_present                         = false;
  bool srbs_to_be_setup_mod_list_present                     = false;
  bool drbs_to_be_setup_mod_list_present                     = false;
  bool drbs_to_be_modified_list_present                      = false;
  bool srbs_to_be_released_list_present                      = false;
  bool drbs_to_be_released_list_present                      = false;
  bool inactivity_monitoring_request_present                 = false;
  bool rat_freq_prio_info_present                            = false;
  bool drx_cfg_ind_present                                   = false;
  bool rlc_fail_ind_present                                  = false;
  bool ul_tx_direct_current_list_info_present                = false;
  bool gnb_du_cfg_query_present                              = false;
  bool gnb_du_ue_ambr_ul_present                             = false;
  bool execute_dupl_present                                  = false;
  bool rrc_delivery_status_request_present                   = false;
  bool res_coordination_transfer_info_present                = false;
  bool serving_cell_mo_present                               = false;
  bool needfor_gap_present                                   = false;
  bool full_cfg_present                                      = false;
  bool add_rrm_prio_idx_present                              = false;
  bool lower_layer_presence_status_change_present            = false;
  bool bh_chs_to_be_setup_mod_list_present                   = false;
  bool bh_chs_to_be_modified_list_present                    = false;
  bool bh_chs_to_be_released_list_present                    = false;
  bool nr_v2x_services_authorized_present                    = false;
  bool ltev2x_services_authorized_present                    = false;
  bool nr_ue_sidelink_aggr_max_bitrate_present               = false;
  bool lte_ue_sidelink_aggr_max_bitrate_present              = false;
  bool pc5_link_ambr_present                                 = false;
  bool sl_drbs_to_be_setup_mod_list_present                  = false;
  bool sl_drbs_to_be_modified_list_present                   = false;
  bool sl_drbs_to_be_released_list_present                   = false;
  bool conditional_intra_du_mob_info_present                 = false;
  bool f1_c_transfer_path_present                            = false;
  bool scg_ind_present                                       = false;
  bool ul_tx_direct_current_two_carrier_list_info_present    = false;
  bool iab_conditional_rrc_msg_delivery_ind_present          = false;
  bool f1_c_transfer_path_nr_dc_present                      = false;
  bool mdt_polluted_meas_ind_present                         = false;
  bool scg_activation_request_present                        = false;
  bool cg_sdt_query_ind_present                              = false;
  bool five_g_pro_se_authorized_present                      = false;
  bool five_g_pro_se_ue_pc5_aggr_max_bitrate_present         = false;
  bool five_g_pro_se_pc5_link_ambr_present                   = false;
  bool upd_remote_ue_local_id_present                        = false;
  bool uu_rlc_ch_to_be_setup_list_present                    = false;
  bool uu_rlc_ch_to_be_modified_list_present                 = false;
  bool uu_rlc_ch_to_be_released_list_present                 = false;
  bool pc5_rlc_ch_to_be_setup_list_present                   = false;
  bool pc5_rlc_ch_to_be_modified_list_present                = false;
  bool pc5_rlc_ch_to_be_released_list_present                = false;
  bool path_switch_cfg_present                               = false;
  bool gnb_du_ue_slice_max_bit_rate_list_present             = false;
  bool multicast_mbs_session_setup_list_present              = false;
  bool multicast_mbs_session_rem_list_present                = false;
  bool ue_multicast_m_rbs_to_be_setup_at_modify_list_present = false;
  bool ue_multicast_m_rbs_to_be_released_list_present        = false;
  bool sldrx_cycle_list_present                              = false;
  bool management_based_mdt_plmn_mod_list_present            = false;
  bool sdt_bearer_cfg_query_ind_present                      = false;
  bool daps_ho_status_present                                = false;
  bool serving_cell_mo_list_present                          = false;
  bool ul_tx_direct_current_more_carrier_info_present        = false;
  bool cp_acmcg_info_present                                 = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<nr_cgi_s>                                      sp_cell_id;
  ie_field_s<integer<uint8_t, 0, 31, true, true>>           serv_cell_idx;
  ie_field_s<cell_ul_cfg_e>                                 sp_cell_ul_cfg;
  ie_field_s<drx_cycle_s>                                   drx_cycle;
  ie_field_s<cu_to_du_rrc_info_s>                           cu_to_du_rrc_info;
  ie_field_s<tx_action_ind_e>                               tx_action_ind;
  ie_field_s<unbounded_octstring<true>>                     res_coordination_transfer_container;
  ie_field_s<rrc_recfg_complete_ind_e>                      rrc_recfg_complete_ind;
  ie_field_s<unbounded_octstring<true>>                     rrc_container;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<scell_to_be_setup_mod_item_ies_o>, 1, 32, true>>
      scell_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<scell_to_be_remd_item_ies_o>, 1, 32, true>>
      scell_to_be_remd_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_to_be_setup_mod_item_ies_o>, 1, 8, true>>
      srbs_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_to_be_setup_mod_item_ies_o>, 1, 64, true>>
      drbs_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_to_be_modified_item_ies_o>, 1, 64, true>>
      drbs_to_be_modified_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_to_be_released_item_ies_o>, 1, 8, true>>
      srbs_to_be_released_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_to_be_released_item_ies_o>, 1, 64, true>>
                                                              drbs_to_be_released_list;
  ie_field_s<inactivity_monitoring_request_e>                 inactivity_monitoring_request;
  ie_field_s<rat_freq_prio_info_c>                            rat_freq_prio_info;
  ie_field_s<drx_cfg_ind_e>                                   drx_cfg_ind;
  ie_field_s<rlc_fail_ind_s>                                  rlc_fail_ind;
  ie_field_s<unbounded_octstring<true>>                       ul_tx_direct_current_list_info;
  ie_field_s<gnb_du_cfg_query_e>                              gnb_du_cfg_query;
  ie_field_s<integer<uint64_t, 0, 4000000000000, true, true>> gnb_du_ue_ambr_ul;
  ie_field_s<execute_dupl_e>                                  execute_dupl;
  ie_field_s<rrc_delivery_status_request_e>                   rrc_delivery_status_request;
  ie_field_s<res_coordination_transfer_info_s>                res_coordination_transfer_info;
  ie_field_s<integer<uint8_t, 1, 64, true, true>>             serving_cell_mo;
  ie_field_s<needfor_gap_e>                                   needfor_gap;
  ie_field_s<full_cfg_e>                                      full_cfg;
  ie_field_s<fixed_bitstring<32, false, true>>                add_rrm_prio_idx;
  ie_field_s<lower_layer_presence_status_change_e>            lower_layer_presence_status_change;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<bh_chs_to_be_setup_mod_item_ies_o>, 1, 65536, true>>
      bh_chs_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<bh_chs_to_be_modified_item_ies_o>, 1, 65536, true>>
      bh_chs_to_be_modified_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<bh_chs_to_be_released_item_ies_o>, 1, 65536, true>>
                                                              bh_chs_to_be_released_list;
  ie_field_s<nr_v2x_services_authorized_s>                    nr_v2x_services_authorized;
  ie_field_s<ltev2x_services_authorized_s>                    ltev2x_services_authorized;
  ie_field_s<nr_ue_sidelink_aggr_max_bitrate_s>               nr_ue_sidelink_aggr_max_bitrate;
  ie_field_s<lte_ue_sidelink_aggr_max_bitrate_s>              lte_ue_sidelink_aggr_max_bitrate;
  ie_field_s<integer<uint64_t, 0, 4000000000000, true, true>> pc5_link_ambr;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<sl_drbs_to_be_setup_mod_item_ies_o>, 1, 512, true>>
      sl_drbs_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<sl_drbs_to_be_modified_item_ies_o>, 1, 512, true>>
      sl_drbs_to_be_modified_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<sl_drbs_to_be_released_item_ies_o>, 1, 512, true>>
                                                                          sl_drbs_to_be_released_list;
  ie_field_s<conditional_intra_du_mob_info_s>                             conditional_intra_du_mob_info;
  ie_field_s<f1_c_transfer_path_s>                                        f1_c_transfer_path;
  ie_field_s<scg_ind_e>                                                   scg_ind;
  ie_field_s<unbounded_octstring<true>>                                   ul_tx_direct_current_two_carrier_list_info;
  ie_field_s<iab_conditional_rrc_msg_delivery_ind_e>                      iab_conditional_rrc_msg_delivery_ind;
  ie_field_s<f1_c_transfer_path_nr_dc_s>                                  f1_c_transfer_path_nr_dc;
  ie_field_s<mdt_polluted_meas_ind_e>                                     mdt_polluted_meas_ind;
  ie_field_s<scg_activation_request_e>                                    scg_activation_request;
  ie_field_s<cg_sdt_query_ind_e>                                          cg_sdt_query_ind;
  ie_field_s<five_g_pro_se_authorized_s>                                  five_g_pro_se_authorized;
  ie_field_s<nr_ue_sidelink_aggr_max_bitrate_s>                           five_g_pro_se_ue_pc5_aggr_max_bitrate;
  ie_field_s<integer<uint64_t, 0, 4000000000000, true, true>>             five_g_pro_se_pc5_link_ambr;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>                       upd_remote_ue_local_id;
  ie_field_s<dyn_seq_of<uu_rlc_ch_to_be_setup_item_s, 1, 32, true>>       uu_rlc_ch_to_be_setup_list;
  ie_field_s<dyn_seq_of<uu_rlc_ch_to_be_modified_item_s, 1, 32, true>>    uu_rlc_ch_to_be_modified_list;
  ie_field_s<dyn_seq_of<uu_rlc_ch_to_be_released_item_s, 1, 32, true>>    uu_rlc_ch_to_be_released_list;
  ie_field_s<dyn_seq_of<pc5_rlc_ch_to_be_setup_item_s, 1, 512, true>>     pc5_rlc_ch_to_be_setup_list;
  ie_field_s<dyn_seq_of<pc5_rlc_ch_to_be_modified_item_s, 1, 512, true>>  pc5_rlc_ch_to_be_modified_list;
  ie_field_s<dyn_seq_of<pc5_rlc_ch_to_be_released_item_s, 1, 512, true>>  pc5_rlc_ch_to_be_released_list;
  ie_field_s<path_switch_cfg_s>                                           path_switch_cfg;
  ie_field_s<dyn_seq_of<gnb_du_ue_slice_max_bit_rate_item_s, 1, 8, true>> gnb_du_ue_slice_max_bit_rate_list;
  ie_field_s<dyn_seq_of<multicast_mbs_session_list_item_s, 1, 256, true>> multicast_mbs_session_setup_list;
  ie_field_s<dyn_seq_of<multicast_mbs_session_list_item_s, 1, 256, true>> multicast_mbs_session_rem_list;
  ie_field_s<
      dyn_seq_of<protocol_ie_single_container_s<ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o>, 1, 64, true>>
      ue_multicast_m_rbs_to_be_setup_at_modify_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ue_multicast_m_rbs_to_be_released_item_ies_o>, 1, 64, true>>
                                                                ue_multicast_m_rbs_to_be_released_list;
  ie_field_s<dyn_seq_of<sldrx_cycle_item_s, 1, 32, true>>       sldrx_cycle_list;
  ie_field_s<dyn_seq_of<fixed_octstring<3, true>, 0, 16, true>> management_based_mdt_plmn_mod_list;
  ie_field_s<sdt_bearer_cfg_query_ind_e>                        sdt_bearer_cfg_query_ind;
  ie_field_s<daps_ho_status_e>                                  daps_ho_status;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<serving_cell_mo_list_item_ies_o>, 1, 16, true>>
                                        serving_cell_mo_list;
  ie_field_s<unbounded_octstring<true>> ul_tx_direct_current_more_carrier_info;
  ie_field_s<cp_acmcg_info_s>           cp_acmcg_info;

  // sequence methods
  ue_context_mod_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationRequest ::= SEQUENCE
using ue_context_mod_request_s = elementary_procedure_option<ue_context_mod_request_ies_container>;

struct ue_context_mod_required_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool res_coordination_transfer_container_present             = false;
  bool du_to_cu_rrc_info_present                               = false;
  bool drbs_required_to_be_modified_list_present               = false;
  bool srbs_required_to_be_released_list_present               = false;
  bool drbs_required_to_be_released_list_present               = false;
  bool bh_chs_required_to_be_released_list_present             = false;
  bool sl_drbs_required_to_be_modified_list_present            = false;
  bool sl_drbs_required_to_be_released_list_present            = false;
  bool target_cells_to_cancel_present                          = false;
  bool uu_rlc_ch_required_to_be_modified_list_present          = false;
  bool uu_rlc_ch_required_to_be_released_list_present          = false;
  bool pc5_rlc_ch_required_to_be_modified_list_present         = false;
  bool pc5_rlc_ch_required_to_be_released_list_present         = false;
  bool ue_multicast_m_rbs_required_to_be_modified_list_present = false;
  bool ue_multicast_m_rbs_required_to_be_released_list_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<unbounded_octstring<true>>                     res_coordination_transfer_container;
  ie_field_s<du_to_cu_rrc_info_s>                           du_to_cu_rrc_info;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_required_to_be_modified_item_ies_o>, 1, 64, true>>
      drbs_required_to_be_modified_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_required_to_be_released_item_ies_o>, 1, 8, true>>
      srbs_required_to_be_released_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_required_to_be_released_item_ies_o>, 1, 64, true>>
                      drbs_required_to_be_released_list;
  ie_field_s<cause_c> cause;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<bh_chs_required_to_be_released_item_ies_o>, 1, 65536, true>>
      bh_chs_required_to_be_released_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<sl_drbs_required_to_be_modified_item_ies_o>, 1, 512, true>>
      sl_drbs_required_to_be_modified_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<sl_drbs_required_to_be_released_item_ies_o>, 1, 512, true>>
                                                                                sl_drbs_required_to_be_released_list;
  ie_field_s<dyn_seq_of<target_cell_list_item_s, 1, 8, true>>                   target_cells_to_cancel;
  ie_field_s<dyn_seq_of<uu_rlc_ch_required_to_be_modified_item_s, 1, 32, true>> uu_rlc_ch_required_to_be_modified_list;
  ie_field_s<dyn_seq_of<uu_rlc_ch_required_to_be_released_item_s, 1, 32, true>> uu_rlc_ch_required_to_be_released_list;
  ie_field_s<dyn_seq_of<pc5_rlc_ch_required_to_be_modified_item_s, 1, 512, true>>
      pc5_rlc_ch_required_to_be_modified_list;
  ie_field_s<dyn_seq_of<pc5_rlc_ch_required_to_be_released_item_s, 1, 512, true>>
      pc5_rlc_ch_required_to_be_released_list;
  ie_field_s<
      dyn_seq_of<protocol_ie_single_container_s<ue_multicast_m_rbs_required_to_be_modified_item_ies_o>, 1, 64, true>>
      ue_multicast_m_rbs_required_to_be_modified_list;
  ie_field_s<
      dyn_seq_of<protocol_ie_single_container_s<ue_multicast_m_rbs_required_to_be_released_item_ies_o>, 1, 64, true>>
      ue_multicast_m_rbs_required_to_be_released_list;

  // sequence methods
  ue_context_mod_required_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationRequired ::= SEQUENCE
using ue_context_mod_required_s = elementary_procedure_option<ue_context_mod_required_ies_container>;

struct ue_context_mod_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      res_coordination_transfer_container_present   = false;
  bool                                                      du_to_cu_rrc_info_present                     = false;
  bool                                                      drbs_setup_mod_list_present                   = false;
  bool                                                      drbs_modified_list_present                    = false;
  bool                                                      srbs_failed_to_be_setup_mod_list_present      = false;
  bool                                                      drbs_failed_to_be_setup_mod_list_present      = false;
  bool                                                      scell_failedto_setup_mod_list_present         = false;
  bool                                                      drbs_failed_to_be_modified_list_present       = false;
  bool                                                      inactivity_monitoring_resp_present            = false;
  bool                                                      crit_diagnostics_present                      = false;
  bool                                                      c_rnti_present                                = false;
  bool                                                      associated_scell_list_present                 = false;
  bool                                                      srbs_setup_mod_list_present                   = false;
  bool                                                      srbs_modified_list_present                    = false;
  bool                                                      full_cfg_present                              = false;
  bool                                                      bh_chs_setup_mod_list_present                 = false;
  bool                                                      bh_chs_modified_list_present                  = false;
  bool                                                      bh_chs_failed_to_be_setup_mod_list_present    = false;
  bool                                                      bh_chs_failed_to_be_modified_list_present     = false;
  bool                                                      sl_drbs_setup_mod_list_present                = false;
  bool                                                      sl_drbs_modified_list_present                 = false;
  bool                                                      sl_drbs_failed_to_be_setup_mod_list_present   = false;
  bool                                                      sl_drbs_failed_to_be_modified_list_present    = false;
  bool                                                      requested_target_cell_global_id_present       = false;
  bool                                                      scg_activation_status_present                 = false;
  bool                                                      uu_rlc_ch_setup_list_present                  = false;
  bool                                                      uu_rlc_ch_failed_to_be_setup_list_present     = false;
  bool                                                      uu_rlc_ch_modified_list_present               = false;
  bool                                                      uu_rlc_ch_failed_to_be_modified_list_present  = false;
  bool                                                      pc5_rlc_ch_setup_list_present                 = false;
  bool                                                      pc5_rlc_ch_failed_to_be_setup_list_present    = false;
  bool                                                      pc5_rlc_ch_modified_list_present              = false;
  bool                                                      pc5_rlc_ch_failed_to_be_modified_list_present = false;
  bool                                                      sdt_bearer_cfg_info_present                   = false;
  bool                                                      ue_multicast_m_rbs_setup_list_present         = false;
  bool                                                      serving_cell_mo_encoded_in_cgc_list_present   = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<unbounded_octstring<true>>                     res_coordination_transfer_container;
  ie_field_s<du_to_cu_rrc_info_s>                           du_to_cu_rrc_info;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_setup_mod_item_ies_o>, 1, 64, true>> drbs_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_modified_item_ies_o>, 1, 64, true>>  drbs_modified_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_failed_to_be_setup_mod_item_ies_o>, 1, 8, true>>
      srbs_failed_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_failed_to_be_setup_mod_item_ies_o>, 1, 64, true>>
      drbs_failed_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<scell_failedto_setup_mod_item_ies_o>, 1, 32, true>>
      scell_failedto_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_failed_to_be_modified_item_ies_o>, 1, 64, true>>
                                                      drbs_failed_to_be_modified_list;
  ie_field_s<inactivity_monitoring_resp_e>            inactivity_monitoring_resp;
  ie_field_s<crit_diagnostics_s>                      crit_diagnostics;
  ie_field_s<integer<uint32_t, 0, 65535, true, true>> c_rnti;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<associated_scell_item_ies_o>, 1, 32, true>>
                                                                                                associated_scell_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_setup_mod_item_ies_o>, 1, 8, true>> srbs_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_modified_item_ies_o>, 1, 8, true>>  srbs_modified_list;
  ie_field_s<full_cfg_e>                                                                        full_cfg;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<bh_chs_setup_mod_item_ies_o>, 1, 65536, true>>
      bh_chs_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<bh_chs_modified_item_ies_o>, 1, 65536, true>>
      bh_chs_modified_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<bh_chs_failed_to_be_setup_mod_item_ies_o>, 1, 65536, true>>
      bh_chs_failed_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<bh_chs_failed_to_be_modified_item_ies_o>, 1, 65536, true>>
      bh_chs_failed_to_be_modified_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<sl_drbs_setup_mod_item_ies_o>, 1, 512, true>>
      sl_drbs_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<sl_drbs_modified_item_ies_o>, 1, 512, true>>
      sl_drbs_modified_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<sl_drbs_failed_to_be_setup_mod_item_ies_o>, 1, 512, true>>
      sl_drbs_failed_to_be_setup_mod_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<sl_drbs_failed_to_be_modified_item_ies_o>, 1, 512, true>>
                                                                                sl_drbs_failed_to_be_modified_list;
  ie_field_s<nr_cgi_s>                                                          requested_target_cell_global_id;
  ie_field_s<scg_activation_status_e>                                           scg_activation_status;
  ie_field_s<dyn_seq_of<uu_rlc_ch_setup_item_s, 1, 32, true>>                   uu_rlc_ch_setup_list;
  ie_field_s<dyn_seq_of<uu_rlc_ch_failed_to_be_setup_item_s, 1, 32, true>>      uu_rlc_ch_failed_to_be_setup_list;
  ie_field_s<dyn_seq_of<uu_rlc_ch_modified_item_s, 1, 32, true>>                uu_rlc_ch_modified_list;
  ie_field_s<dyn_seq_of<uu_rlc_ch_failed_to_be_modified_item_s, 1, 32, true>>   uu_rlc_ch_failed_to_be_modified_list;
  ie_field_s<dyn_seq_of<pc5_rlc_ch_setup_item_s, 1, 512, true>>                 pc5_rlc_ch_setup_list;
  ie_field_s<dyn_seq_of<pc5_rlc_ch_failed_to_be_setup_item_s, 1, 512, true>>    pc5_rlc_ch_failed_to_be_setup_list;
  ie_field_s<dyn_seq_of<pc5_rlc_ch_modified_item_s, 1, 512, true>>              pc5_rlc_ch_modified_list;
  ie_field_s<dyn_seq_of<pc5_rlc_ch_failed_to_be_modified_item_s, 1, 512, true>> pc5_rlc_ch_failed_to_be_modified_list;
  ie_field_s<sdt_bearer_cfg_info_s>                                             sdt_bearer_cfg_info;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ue_multicast_m_rbs_setup_item_ies_o>, 1, 64, true>>
                                                                            ue_multicast_m_rbs_setup_list;
  ie_field_s<dyn_seq_of<serving_cell_mo_encoded_in_cgc_item_s, 1, 8, true>> serving_cell_mo_encoded_in_cgc_list;

  // sequence methods
  ue_context_mod_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextModificationResponse ::= SEQUENCE
using ue_context_mod_resp_s = elementary_procedure_option<ue_context_mod_resp_ies_container>;

struct ue_context_release_cmd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                        rrc_container_present               = false;
  bool                                                        srb_id_present                      = false;
  bool                                                        old_gnb_du_ue_f1ap_id_present       = false;
  bool                                                        execute_dupl_present                = false;
  bool                                                        rrc_delivery_status_request_present = false;
  bool                                                        target_cells_to_cancel_present      = false;
  bool                                                        pos_conext_rev_ind_present          = false;
  bool                                                        cg_sdt_kept_ind_present             = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>   gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>   gnb_du_ue_f1ap_id;
  ie_field_s<cause_c>                                         cause;
  ie_field_s<unbounded_octstring<true>>                       rrc_container;
  ie_field_s<integer<uint8_t, 0, 3, true, true>>              srb_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>   old_gnb_du_ue_f1ap_id;
  ie_field_s<execute_dupl_e>                                  execute_dupl;
  ie_field_s<rrc_delivery_status_request_e>                   rrc_delivery_status_request;
  ie_field_s<dyn_seq_of<target_cell_list_item_s, 1, 8, true>> target_cells_to_cancel;
  ie_field_s<pos_conext_rev_ind_e>                            pos_conext_rev_ind;
  ie_field_s<cg_sdt_kept_ind_e>                               cg_sdt_kept_ind;

  // sequence methods
  ue_context_release_cmd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextReleaseCommand ::= SEQUENCE
using ue_context_release_cmd_s = elementary_procedure_option<ue_context_release_cmd_ies_container>;

struct ue_context_release_complete_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      crit_diagnostics_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;

  // sequence methods
  ue_context_release_complete_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextReleaseComplete ::= SEQUENCE
using ue_context_release_complete_s = elementary_procedure_option<ue_context_release_complete_ies_container>;

struct ue_context_release_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                        target_cells_to_cancel_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>   gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>   gnb_du_ue_f1ap_id;
  ie_field_s<cause_c>                                         cause;
  ie_field_s<dyn_seq_of<target_cell_list_item_s, 1, 8, true>> target_cells_to_cancel;

  // sequence methods
  ue_context_release_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextReleaseRequest ::= SEQUENCE
using ue_context_release_request_s = elementary_procedure_option<ue_context_release_request_ies_container>;

struct ue_context_setup_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      gnb_du_ue_f1ap_id_present               = false;
  bool                                                      crit_diagnostics_present                = false;
  bool                                                      potential_sp_cell_list_present          = false;
  bool                                                      requested_target_cell_global_id_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<cause_c>                                       cause;
  ie_field_s<crit_diagnostics_s>                            crit_diagnostics;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<potential_sp_cell_item_ies_o>, 0, 64, true>>
                       potential_sp_cell_list;
  ie_field_s<nr_cgi_s> requested_target_cell_global_id;

  // sequence methods
  ue_context_setup_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextSetupFailure ::= SEQUENCE
using ue_context_setup_fail_s = elementary_procedure_option<ue_context_setup_fail_ies_container>;

struct ue_context_setup_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      gnb_du_ue_f1ap_id_present                     = false;
  bool                                                      sp_cell_ul_cfg_present                        = false;
  bool                                                      candidate_sp_cell_list_present                = false;
  bool                                                      drx_cycle_present                             = false;
  bool                                                      res_coordination_transfer_container_present   = false;
  bool                                                      scell_to_be_setup_list_present                = false;
  bool                                                      srbs_to_be_setup_list_present                 = false;
  bool                                                      drbs_to_be_setup_list_present                 = false;
  bool                                                      inactivity_monitoring_request_present         = false;
  bool                                                      rat_freq_prio_info_present                    = false;
  bool                                                      rrc_container_present                         = false;
  bool                                                      masked_imeisv_present                         = false;
  bool                                                      serving_plmn_present                          = false;
  bool                                                      gnb_du_ue_ambr_ul_present                     = false;
  bool                                                      rrc_delivery_status_request_present           = false;
  bool                                                      res_coordination_transfer_info_present        = false;
  bool                                                      serving_cell_mo_present                       = false;
  bool                                                      new_gnb_cu_ue_f1ap_id_present                 = false;
  bool                                                      ran_ue_id_present                             = false;
  bool                                                      trace_activation_present                      = false;
  bool                                                      add_rrm_prio_idx_present                      = false;
  bool                                                      bh_chs_to_be_setup_list_present               = false;
  bool                                                      cfg_bap_address_present                       = false;
  bool                                                      nr_v2x_services_authorized_present            = false;
  bool                                                      ltev2x_services_authorized_present            = false;
  bool                                                      nr_ue_sidelink_aggr_max_bitrate_present       = false;
  bool                                                      lte_ue_sidelink_aggr_max_bitrate_present      = false;
  bool                                                      pc5_link_ambr_present                         = false;
  bool                                                      sl_drbs_to_be_setup_list_present              = false;
  bool                                                      conditional_inter_du_mob_info_present         = false;
  bool                                                      management_based_mdt_plmn_list_present        = false;
  bool                                                      serving_n_id_present                          = false;
  bool                                                      f1_c_transfer_path_present                    = false;
  bool                                                      f1_c_transfer_path_nr_dc_present              = false;
  bool                                                      mdt_polluted_meas_ind_present                 = false;
  bool                                                      scg_activation_request_present                = false;
  bool                                                      cg_sdt_session_info_old_present               = false;
  bool                                                      five_g_pro_se_authorized_present              = false;
  bool                                                      five_g_pro_se_ue_pc5_aggr_max_bitrate_present = false;
  bool                                                      five_g_pro_se_pc5_link_ambr_present           = false;
  bool                                                      uu_rlc_ch_to_be_setup_list_present            = false;
  bool                                                      pc5_rlc_ch_to_be_setup_list_present           = false;
  bool                                                      path_switch_cfg_present                       = false;
  bool                                                      gnb_du_ue_slice_max_bit_rate_list_present     = false;
  bool                                                      multicast_mbs_session_setup_list_present      = false;
  bool                                                      ue_multicast_m_rbs_to_be_setup_list_present   = false;
  bool                                                      serving_cell_mo_list_present                  = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<nr_cgi_s>                                      sp_cell_id;
  ie_field_s<integer<uint8_t, 0, 31, true, true>>           serv_cell_idx;
  ie_field_s<cell_ul_cfg_e>                                 sp_cell_ul_cfg;
  ie_field_s<cu_to_du_rrc_info_s>                           cu_to_du_rrc_info;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<candidate_sp_cell_item_ies_o>, 1, 64, true>>
                                        candidate_sp_cell_list;
  ie_field_s<drx_cycle_s>               drx_cycle;
  ie_field_s<unbounded_octstring<true>> res_coordination_transfer_container;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<scell_to_be_setup_item_ies_o>, 1, 32, true>>
      scell_to_be_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_to_be_setup_item_ies_o>, 1, 8, true>> srbs_to_be_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_to_be_setup_item_ies_o>, 1, 64, true>>
                                                              drbs_to_be_setup_list;
  ie_field_s<inactivity_monitoring_request_e>                 inactivity_monitoring_request;
  ie_field_s<rat_freq_prio_info_c>                            rat_freq_prio_info;
  ie_field_s<unbounded_octstring<true>>                       rrc_container;
  ie_field_s<fixed_bitstring<64, false, true>>                masked_imeisv;
  ie_field_s<fixed_octstring<3, true>>                        serving_plmn;
  ie_field_s<integer<uint64_t, 0, 4000000000000, true, true>> gnb_du_ue_ambr_ul;
  ie_field_s<rrc_delivery_status_request_e>                   rrc_delivery_status_request;
  ie_field_s<res_coordination_transfer_info_s>                res_coordination_transfer_info;
  ie_field_s<integer<uint8_t, 1, 64, true, true>>             serving_cell_mo;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>>   new_gnb_cu_ue_f1ap_id;
  ie_field_s<fixed_octstring<8, true>>                        ran_ue_id;
  ie_field_s<trace_activation_s>                              trace_activation;
  ie_field_s<fixed_bitstring<32, false, true>>                add_rrm_prio_idx;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<bh_chs_to_be_setup_item_ies_o>, 1, 65536, true>>
                                                              bh_chs_to_be_setup_list;
  ie_field_s<fixed_bitstring<10, false, true>>                cfg_bap_address;
  ie_field_s<nr_v2x_services_authorized_s>                    nr_v2x_services_authorized;
  ie_field_s<ltev2x_services_authorized_s>                    ltev2x_services_authorized;
  ie_field_s<nr_ue_sidelink_aggr_max_bitrate_s>               nr_ue_sidelink_aggr_max_bitrate;
  ie_field_s<lte_ue_sidelink_aggr_max_bitrate_s>              lte_ue_sidelink_aggr_max_bitrate;
  ie_field_s<integer<uint64_t, 0, 4000000000000, true, true>> pc5_link_ambr;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<sl_drbs_to_be_setup_item_ies_o>, 1, 512, true>>
                                                                          sl_drbs_to_be_setup_list;
  ie_field_s<conditional_inter_du_mob_info_s>                             conditional_inter_du_mob_info;
  ie_field_s<dyn_seq_of<fixed_octstring<3, true>, 1, 16, true>>           management_based_mdt_plmn_list;
  ie_field_s<fixed_bitstring<44, false, true>>                            serving_n_id;
  ie_field_s<f1_c_transfer_path_s>                                        f1_c_transfer_path;
  ie_field_s<f1_c_transfer_path_nr_dc_s>                                  f1_c_transfer_path_nr_dc;
  ie_field_s<mdt_polluted_meas_ind_e>                                     mdt_polluted_meas_ind;
  ie_field_s<scg_activation_request_e>                                    scg_activation_request;
  ie_field_s<cg_sdt_session_info_s>                                       cg_sdt_session_info_old;
  ie_field_s<five_g_pro_se_authorized_s>                                  five_g_pro_se_authorized;
  ie_field_s<nr_ue_sidelink_aggr_max_bitrate_s>                           five_g_pro_se_ue_pc5_aggr_max_bitrate;
  ie_field_s<integer<uint64_t, 0, 4000000000000, true, true>>             five_g_pro_se_pc5_link_ambr;
  ie_field_s<dyn_seq_of<uu_rlc_ch_to_be_setup_item_s, 1, 32, true>>       uu_rlc_ch_to_be_setup_list;
  ie_field_s<dyn_seq_of<pc5_rlc_ch_to_be_setup_item_s, 1, 512, true>>     pc5_rlc_ch_to_be_setup_list;
  ie_field_s<path_switch_cfg_s>                                           path_switch_cfg;
  ie_field_s<dyn_seq_of<gnb_du_ue_slice_max_bit_rate_item_s, 1, 8, true>> gnb_du_ue_slice_max_bit_rate_list;
  ie_field_s<dyn_seq_of<multicast_mbs_session_list_item_s, 1, 256, true>> multicast_mbs_session_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ue_multicast_m_rbs_to_be_setup_item_ies_o>, 1, 64, true>>
      ue_multicast_m_rbs_to_be_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<serving_cell_mo_list_item_ies_o>, 1, 16, true>>
      serving_cell_mo_list;

  // sequence methods
  ue_context_setup_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextSetupRequest ::= SEQUENCE
using ue_context_setup_request_s = elementary_procedure_option<ue_context_setup_request_ies_container>;

struct ue_context_setup_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      c_rnti_present                              = false;
  bool                                                      res_coordination_transfer_container_present = false;
  bool                                                      full_cfg_present                            = false;
  bool                                                      drbs_setup_list_present                     = false;
  bool                                                      srbs_failed_to_be_setup_list_present        = false;
  bool                                                      drbs_failed_to_be_setup_list_present        = false;
  bool                                                      scell_failedto_setup_list_present           = false;
  bool                                                      inactivity_monitoring_resp_present          = false;
  bool                                                      crit_diagnostics_present                    = false;
  bool                                                      srbs_setup_list_present                     = false;
  bool                                                      bh_chs_setup_list_present                   = false;
  bool                                                      bh_chs_failed_to_be_setup_list_present      = false;
  bool                                                      sl_drbs_setup_list_present                  = false;
  bool                                                      sl_drbs_failed_to_be_setup_list_present     = false;
  bool                                                      requested_target_cell_global_id_present     = false;
  bool                                                      scg_activation_status_present               = false;
  bool                                                      uu_rlc_ch_setup_list_present                = false;
  bool                                                      uu_rlc_ch_failed_to_be_setup_list_present   = false;
  bool                                                      pc5_rlc_ch_setup_list_present               = false;
  bool                                                      pc5_rlc_ch_failed_to_be_setup_list_present  = false;
  bool                                                      serving_cell_mo_encoded_in_cgc_list_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<du_to_cu_rrc_info_s>                           du_to_cu_rrc_info;
  ie_field_s<integer<uint32_t, 0, 65535, true, true>>       c_rnti;
  ie_field_s<unbounded_octstring<true>>                     res_coordination_transfer_container;
  ie_field_s<full_cfg_e>                                    full_cfg;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_setup_item_ies_o>, 1, 64, true>> drbs_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_failed_to_be_setup_item_ies_o>, 1, 8, true>>
      srbs_failed_to_be_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drbs_failed_to_be_setup_item_ies_o>, 1, 64, true>>
      drbs_failed_to_be_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<scell_failedto_setup_item_ies_o>, 1, 32, true>>
                                                                                            scell_failedto_setup_list;
  ie_field_s<inactivity_monitoring_resp_e>                                                  inactivity_monitoring_resp;
  ie_field_s<crit_diagnostics_s>                                                            crit_diagnostics;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<srbs_setup_item_ies_o>, 1, 8, true>> srbs_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<bh_chs_setup_item_ies_o>, 1, 65536, true>> bh_chs_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<bh_chs_failed_to_be_setup_item_ies_o>, 1, 65536, true>>
      bh_chs_failed_to_be_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<sl_drbs_setup_item_ies_o>, 1, 512, true>> sl_drbs_setup_list;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<sl_drbs_failed_to_be_setup_item_ies_o>, 1, 512, true>>
                                                                             sl_drbs_failed_to_be_setup_list;
  ie_field_s<nr_cgi_s>                                                       requested_target_cell_global_id;
  ie_field_s<scg_activation_status_e>                                        scg_activation_status;
  ie_field_s<dyn_seq_of<uu_rlc_ch_setup_item_s, 1, 32, true>>                uu_rlc_ch_setup_list;
  ie_field_s<dyn_seq_of<uu_rlc_ch_failed_to_be_setup_item_s, 1, 32, true>>   uu_rlc_ch_failed_to_be_setup_list;
  ie_field_s<dyn_seq_of<pc5_rlc_ch_setup_item_s, 1, 512, true>>              pc5_rlc_ch_setup_list;
  ie_field_s<dyn_seq_of<pc5_rlc_ch_failed_to_be_setup_item_s, 1, 512, true>> pc5_rlc_ch_failed_to_be_setup_list;
  ie_field_s<dyn_seq_of<serving_cell_mo_encoded_in_cgc_item_s, 1, 8, true>>  serving_cell_mo_encoded_in_cgc_list;

  // sequence methods
  ue_context_setup_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEContextSetupResponse ::= SEQUENCE
using ue_context_setup_resp_s = elementary_procedure_option<ue_context_setup_resp_ies_container>;

struct ue_inactivity_notif_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      sdt_termination_request_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<drb_activity_item_ies_o>, 1, 64, true>> drb_activity_list;
  ie_field_s<sdt_termination_request_e>                                                        sdt_termination_request;

  // sequence methods
  ue_inactivity_notif_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInactivityNotification ::= SEQUENCE
using ue_inactivity_notif_s = elementary_procedure_option<ue_inactivity_notif_ies_container>;

struct ul_rrc_msg_transfer_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                      sel_plmn_id_present           = false;
  bool                                                      new_gnb_du_ue_f1ap_id_present = false;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_cu_ue_f1ap_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> gnb_du_ue_f1ap_id;
  ie_field_s<integer<uint8_t, 0, 3, true, true>>            srb_id;
  ie_field_s<unbounded_octstring<true>>                     rrc_container;
  ie_field_s<fixed_octstring<3, true>>                      sel_plmn_id;
  ie_field_s<integer<uint64_t, 0, 4294967295, false, true>> new_gnb_du_ue_f1ap_id;

  // sequence methods
  ul_rrc_msg_transfer_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULRRCMessageTransfer ::= SEQUENCE
using ul_rrc_msg_transfer_s = elementary_procedure_option<ul_rrc_msg_transfer_ies_container>;

struct write_replace_warning_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                 cells_to_be_broadcast_list_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>>    transaction_id;
  ie_field_s<pws_sys_info_s>                           pws_sys_info;
  ie_field_s<integer<uint32_t, 0, 131071, true, true>> repeat_period;
  ie_field_s<integer<uint32_t, 0, 65535, false, true>> numof_broadcast_request;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_to_be_broadcast_list_item_ies_o>, 1, 512, true>>
      cells_to_be_broadcast_list;

  // sequence methods
  write_replace_warning_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WriteReplaceWarningRequest ::= SEQUENCE
using write_replace_warning_request_s = elementary_procedure_option<write_replace_warning_request_ies_container>;

struct write_replace_warning_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                              cells_broadcast_completed_list_present = false;
  bool                                              crit_diagnostics_present               = false;
  bool                                              ded_si_delivery_needed_ue_list_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true>> transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<cells_broadcast_completed_list_item_ies_o>, 1, 512, true>>
                                 cells_broadcast_completed_list;
  ie_field_s<crit_diagnostics_s> crit_diagnostics;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ded_si_delivery_needed_ue_item_ies_o>, 1, 65536, true>>
      ded_si_delivery_needed_ue_list;

  // sequence methods
  write_replace_warning_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WriteReplaceWarningResponse ::= SEQUENCE
using write_replace_warning_resp_s = elementary_procedure_option<write_replace_warning_resp_ies_container>;

} // namespace f1ap
} // namespace asn1

extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::associated_scell_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bap_hdr_rewriting_added_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bap_hdr_rewriting_remd_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_routing_info_added_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_routing_info_remd_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_failed_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_failed_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_failed_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_required_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_to_be_cancelled_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_m_rbs_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_m_rbs_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_m_rbs_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::broadcast_m_rbs_failed_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_m_rbs_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_m_rbs_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_m_rbs_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_m_rbs_failed_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_m_rbs_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::candidate_sp_cell_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_broadcast_cancelled_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_broadcast_completed_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_failed_to_be_activ_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_status_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_broadcast_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_activ_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_barred_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_deactiv_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::dl_up_tnl_address_to_upd_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drb_activity_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drb_notify_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_failed_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_failed_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_failed_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_modified_conf_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_required_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_required_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ded_si_delivery_needed_ue_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ue_associated_lc_f1_conn_item_res_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_cu_tnl_assoc_failed_to_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_cu_tnl_assoc_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_cu_tnl_assoc_to_add_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_cu_tnl_assoc_to_rem_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_cu_tnl_assoc_to_upd_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_du_served_cells_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_du_tnl_assoc_to_rem_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::iab_allocated_tnl_address_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::iab_tnl_addresses_to_rem_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::mc_paging_cell_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::multicast_f1_u_context_failed_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_f1_u_context_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_f1_u_context_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::multicast_m_rbs_failed_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_m_rbs_failed_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::multicast_m_rbs_failed_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_m_rbs_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_m_rbs_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_m_rbs_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_m_rbs_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_m_rbs_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_m_rbs_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_m_rbs_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::nr_cgi_list_for_restart_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::neighbour_cell_info_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::pws_failed_nr_cgi_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::paging_cell_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::potential_sp_cell_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::protected_eutra_res_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::scell_failedto_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::scell_failedto_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::scell_to_be_remd_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::scell_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::scell_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_failed_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_failed_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_failed_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_modified_conf_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_required_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_required_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_failed_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_failed_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_required_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::served_cells_to_add_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::served_cells_to_delete_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::served_cells_to_modify_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::serving_cell_mo_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::trp_info_item_trp_resp_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::trp_info_type_item_trp_req_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::ue_multicast_m_rbs_required_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::ue_multicast_m_rbs_required_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ue_multicast_m_rbs_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ue_multicast_m_rbs_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ue_multicast_m_rbs_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ue_associated_lc_f1_conn_item_res_ack_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ue_id_list_for_paging_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ul_up_tnl_address_to_upd_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ul_up_tnl_info_to_upd_list_item_ies_o>;

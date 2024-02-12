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
 *                     3GPP TS ASN1 F1AP v17.4.1 (2023-04)
 *
 ******************************************************************************/

#pragma once

#include "f1ap_pdu_contents_ue.h"

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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct access_and_mob_ind_ies_container {
  bool                             rach_report_info_list_present          = false;
  bool                             rlf_report_info_list_present           = false;
  bool                             successful_ho_report_info_list_present = false;
  uint16_t                         transaction_id;
  rach_report_info_list_l          rach_report_info_list;
  rlf_report_info_list_l           rlf_report_info_list;
  successful_ho_report_info_list_l successful_ho_report_info_list;

  // sequence methods
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct access_success_ies_container {
  uint64_t gnb_cu_ue_f1ap_id;
  uint64_t gnb_du_ue_f1ap_id;
  nr_cgi_s nr_cgi;

  // sequence methods
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
  using aperiodic_e_ = enumerated<aperiodic_opts, true>;

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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct bap_map_cfg_ies_container {
  bool                           bh_routing_info_added_list_present   = false;
  bool                           bh_routing_info_remd_list_present    = false;
  bool                           traffic_map_info_present             = false;
  bool                           buffer_size_thresh_present           = false;
  bool                           bap_hdr_rewriting_added_list_present = false;
  bool                           re_routing_enable_ind_present        = false;
  bool                           bap_hdr_rewriting_remd_list_present  = false;
  uint16_t                       transaction_id;
  bh_routing_info_added_list_l   bh_routing_info_added_list;
  bh_routing_info_remd_list_l    bh_routing_info_remd_list;
  traffic_map_info_c             traffic_map_info;
  uint32_t                       buffer_size_thresh;
  bap_hdr_rewriting_added_list_l bap_hdr_rewriting_added_list;
  re_routing_enable_ind_e        re_routing_enable_ind;
  bap_hdr_rewriting_remd_list_l  bap_hdr_rewriting_remd_list;

  // sequence methods
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct bap_map_cfg_ack_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct bap_map_cfg_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BAPMappingConfigurationFailure ::= SEQUENCE
using bap_map_cfg_fail_s = elementary_procedure_option<bap_map_cfg_fail_ies_container>;

// BroadcastContextModificationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_mod_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_mbs_f1ap_id, gnb_du_mbs_f1ap_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_context_mod_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_mbs_f1ap_id;
  uint64_t           gnb_du_mbs_f1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastContextModificationFailure ::= SEQUENCE
using broadcast_context_mod_fail_s = elementary_procedure_option<broadcast_context_mod_fail_ies_container>;

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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_context_mod_request_ies_container {
  bool                                   mbs_service_area_present                     = false;
  bool                                   broadcast_m_rbs_to_be_setup_mod_list_present = false;
  bool                                   broadcast_m_rbs_to_be_modified_list_present  = false;
  bool                                   broadcast_m_rbs_to_be_released_list_present  = false;
  uint64_t                               gnb_cu_mbs_f1ap_id;
  uint64_t                               gnb_du_mbs_f1ap_id;
  mbs_service_area_c                     mbs_service_area;
  mbs_cu_to_du_rrc_info_s                mbs_cu_to_du_rrc_info;
  broadcast_m_rbs_to_be_setup_mod_list_l broadcast_m_rbs_to_be_setup_mod_list;
  broadcast_m_rbs_to_be_modified_list_l  broadcast_m_rbs_to_be_modified_list;
  broadcast_m_rbs_to_be_released_list_l  broadcast_m_rbs_to_be_released_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastContextModificationRequest ::= SEQUENCE
using broadcast_context_mod_request_s = elementary_procedure_option<broadcast_context_mod_request_ies_container>;

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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_context_mod_resp_ies_container {
  bool                                          broadcast_m_rbs_setup_mod_list_present              = false;
  bool                                          broadcast_m_rbs_failed_to_be_setup_mod_list_present = false;
  bool                                          broadcast_m_rbs_modified_list_present               = false;
  bool                                          broadcast_m_rbs_failed_to_be_modified_list_present  = false;
  bool                                          crit_diagnostics_present                            = false;
  uint64_t                                      gnb_cu_mbs_f1ap_id;
  uint64_t                                      gnb_du_mbs_f1ap_id;
  broadcast_m_rbs_setup_mod_list_l              broadcast_m_rbs_setup_mod_list;
  broadcast_m_rbs_failed_to_be_setup_mod_list_l broadcast_m_rbs_failed_to_be_setup_mod_list;
  broadcast_m_rbs_modified_list_l               broadcast_m_rbs_modified_list;
  broadcast_m_rbs_failed_to_be_modified_list_l  broadcast_m_rbs_failed_to_be_modified_list;
  crit_diagnostics_s                            crit_diagnostics;

  // sequence methods
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_context_release_cmd_ies_container {
  uint64_t gnb_cu_mbs_f1ap_id;
  uint64_t gnb_du_mbs_f1ap_id;
  cause_c  cause;

  // sequence methods
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_context_release_complete_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_mbs_f1ap_id;
  uint64_t           gnb_du_mbs_f1ap_id;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_context_release_request_ies_container {
  uint64_t gnb_cu_mbs_f1ap_id;
  uint64_t gnb_du_mbs_f1ap_id;
  cause_c  cause;

  // sequence methods
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_context_setup_fail_ies_container {
  bool               gnb_du_mbs_f1ap_id_present = false;
  bool               crit_diagnostics_present   = false;
  uint64_t           gnb_cu_mbs_f1ap_id;
  uint64_t           gnb_du_mbs_f1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastContextSetupFailure ::= SEQUENCE
using broadcast_context_setup_fail_s = elementary_procedure_option<broadcast_context_setup_fail_ies_container>;

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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_context_setup_request_ies_container {
  bool                               mbs_service_area_present = false;
  uint64_t                           gnb_cu_mbs_f1ap_id;
  mbs_session_id_s                   mbs_session_id;
  mbs_service_area_c                 mbs_service_area;
  mbs_cu_to_du_rrc_info_s            mbs_cu_to_du_rrc_info;
  snssai_s                           snssai;
  broadcast_m_rbs_to_be_setup_list_l broadcast_m_rbs_to_be_setup_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BroadcastContextSetupRequest ::= SEQUENCE
using broadcast_context_setup_request_s = elementary_procedure_option<broadcast_context_setup_request_ies_container>;

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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct broadcast_context_setup_resp_ies_container {
  bool                                      broadcast_m_rbs_failed_to_be_setup_list_present = false;
  bool                                      broadcast_area_scope_present                    = false;
  bool                                      crit_diagnostics_present                        = false;
  uint64_t                                  gnb_cu_mbs_f1ap_id;
  uint64_t                                  gnb_du_mbs_f1ap_id;
  broadcast_m_rbs_setup_list_l              broadcast_m_rbs_setup_list;
  broadcast_m_rbs_failed_to_be_setup_list_l broadcast_m_rbs_failed_to_be_setup_list;
  broadcast_area_scope_c                    broadcast_area_scope;
  crit_diagnostics_s                        crit_diagnostics;

  // sequence methods
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct cu_du_radio_info_transfer_ies_container {
  uint16_t                transaction_id;
  cu_du_radio_info_type_c cu_du_radio_info_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CUDURadioInformationTransfer ::= SEQUENCE
using cu_du_radio_info_transfer_s = elementary_procedure_option<cu_du_radio_info_transfer_ies_container>;

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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct cell_traffic_trace_ies_container {
  bool                                                  privacy_ind_present                 = false;
  bool                                                  trace_collection_entity_uri_present = false;
  uint64_t                                              gnb_cu_ue_f1ap_id;
  uint64_t                                              gnb_du_ue_f1ap_id;
  fixed_octstring<8, true>                              trace_id;
  bounded_bitstring<1, 160, true, true>                 trace_collection_entity_ip_address;
  privacy_ind_e                                         privacy_ind;
  visible_string<0, MAX_ASN_STRING_LENGTH, false, true> trace_collection_entity_uri;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellTrafficTrace ::= SEQUENCE
using cell_traffic_trace_s = elementary_procedure_option<cell_traffic_trace_ies_container>;

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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct du_cu_radio_info_transfer_ies_container {
  uint16_t                transaction_id;
  du_cu_radio_info_type_c du_cu_radio_info_type;

  // sequence methods
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct deactiv_trace_ies_container {
  uint64_t                 gnb_cu_ue_f1ap_id;
  uint64_t                 gnb_du_ue_f1ap_id;
  fixed_octstring<8, true> trace_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DeactivateTrace ::= SEQUENCE
using deactiv_trace_s = elementary_procedure_option<deactiv_trace_ies_container>;

// E-CIDMeasurementFailureIndication-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct e_c_id_meas_fail_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, lmf_ue_meas_id, ran_ue_meas_id, cause, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct e_c_id_meas_fail_ind_ies_container {
  uint64_t gnb_cu_ue_f1ap_id;
  uint64_t gnb_du_ue_f1ap_id;
  uint16_t lmf_ue_meas_id;
  uint16_t ran_ue_meas_id;
  cause_c  cause;

  // sequence methods
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct e_c_id_meas_initiation_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  uint16_t           lmf_ue_meas_id;
  uint16_t           ran_ue_meas_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct e_c_id_meas_initiation_request_ies_container {
  bool                            e_c_id_meas_periodicity_present      = false;
  bool                            pos_meas_periodicity_nr_ao_a_present = false;
  uint64_t                        gnb_cu_ue_f1ap_id;
  uint64_t                        gnb_du_ue_f1ap_id;
  uint16_t                        lmf_ue_meas_id;
  uint16_t                        ran_ue_meas_id;
  e_c_id_report_characteristics_e e_c_id_report_characteristics;
  meas_periodicity_e              e_c_id_meas_periodicity;
  e_c_id_meas_quantities_l        e_c_id_meas_quantities;
  pos_meas_periodicity_nr_ao_a_e  pos_meas_periodicity_nr_ao_a;

  // sequence methods
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct e_c_id_meas_initiation_resp_ies_container {
  bool                 e_c_id_meas_result_present = false;
  bool                 cell_portion_id_present    = false;
  bool                 crit_diagnostics_present   = false;
  uint64_t             gnb_cu_ue_f1ap_id;
  uint64_t             gnb_du_ue_f1ap_id;
  uint16_t             lmf_ue_meas_id;
  uint16_t             ran_ue_meas_id;
  e_c_id_meas_result_s e_c_id_meas_result;
  uint16_t             cell_portion_id;
  crit_diagnostics_s   crit_diagnostics;

  // sequence methods
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct e_c_id_meas_report_ies_container {
  bool                 cell_portion_id_present = false;
  uint64_t             gnb_cu_ue_f1ap_id;
  uint64_t             gnb_du_ue_f1ap_id;
  uint16_t             lmf_ue_meas_id;
  uint16_t             ran_ue_meas_id;
  e_c_id_meas_result_s e_c_id_meas_result;
  uint16_t             cell_portion_id;

  // sequence methods
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct e_c_id_meas_termination_cmd_ies_container {
  uint64_t gnb_cu_ue_f1ap_id;
  uint64_t gnb_du_ue_f1ap_id;
  uint16_t lmf_ue_meas_id;
  uint16_t ran_ue_meas_id;

  // sequence methods
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct error_ind_ies_container {
  bool               gnb_cu_ue_f1ap_id_present = false;
  bool               gnb_du_ue_f1ap_id_present = false;
  bool               cause_present             = false;
  bool               crit_diagnostics_present  = false;
  uint16_t           transaction_id;
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
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
    using types = enumerated<types_opts>;

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
    using types = enumerated<types_opts>;

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

// ResetAll ::= ENUMERATED
struct reset_all_opts {
  enum options { reset_all, /*...*/ nulltype } value;

  const char* to_string() const;
};
using reset_all_e = enumerated<reset_all_opts, true>;

// ResetType-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using reset_type_ext_ies_o = protocol_ies_empty_o;

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

// TRPInformationItemTRPResp ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct trp_info_item_trp_resp_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { trp_info_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

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
    using types = enumerated<types_opts>;

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
    using types = enumerated<types_opts>;

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

// MC-PagingCell-list ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using mc_paging_cell_list_l = dyn_array<protocol_ie_single_container_s<mc_paging_cell_item_ies_o>>;

// PagingCell-list ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using paging_cell_list_l = dyn_array<protocol_ie_single_container_s<paging_cell_item_ies_o>>;

// ResetType ::= CHOICE
struct reset_type_c {
  struct types_opts {
    enum options { f1_interface, part_of_f1_interface, choice_ext, nulltype } value;

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

// SRSType ::= CHOICE
struct srs_type_c {
  struct types_opts {
    enum options { semipersistent_srs, aperiodic_srs, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

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

// TRPInformationListTRPResp ::= SEQUENCE (SIZE (1..65535)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using trp_info_list_trp_resp_l = dyn_array<protocol_ie_single_container_s<trp_info_item_trp_resp_o>>;

// TRPInformationTypeListTRPReq ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using trp_info_type_list_trp_req_l = dyn_array<protocol_ie_single_container_s<trp_info_type_item_trp_req_o>>;

// UE-associatedLogicalF1-ConnectionListResAck ::= SEQUENCE (SIZE (1..65536)) OF
// ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using ue_associated_lc_f1_conn_list_res_ack_l =
    dyn_array<protocol_ie_single_container_s<ue_associated_lc_f1_conn_item_res_ack_o>>;

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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct f1ap_private_ies_empty_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::nulltype; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };
};
// PrivateMessage-IEs ::= OBJECT SET OF F1AP-PRIVATE-IES
using private_msg_ies_o = f1ap_private_ies_empty_o;

// QoEInformationTransfer-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct qo_e_info_transfer_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_ue_f1ap_id, gnb_du_ue_f1ap_id, qo_e_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
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
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
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
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct f1_removal_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1RemovalFailure ::= SEQUENCE
using f1_removal_fail_s = elementary_procedure_option<f1_removal_fail_ies_container>;

// F1RemovalRequest ::= SEQUENCE
using f1_removal_request_s = elementary_procedure_option<protocol_ie_container_l<f1_removal_request_ies_o>>;

struct f1_removal_resp_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1RemovalResponse ::= SEQUENCE
using f1_removal_resp_s = elementary_procedure_option<f1_removal_resp_ies_container>;

struct f1_setup_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1SetupFailure ::= SEQUENCE
using f1_setup_fail_s = elementary_procedure_option<f1_setup_fail_ies_container>;

struct f1_setup_request_ies_container {
  bool                                 gnb_du_name_present                  = false;
  bool                                 gnb_du_served_cells_list_present     = false;
  bool                                 transport_layer_address_info_present = false;
  bool                                 bap_address_present                  = false;
  bool                                 extended_gnb_du_name_present         = false;
  uint16_t                             transaction_id;
  uint64_t                             gnb_du_id;
  printable_string<1, 150, true, true> gnb_du_name;
  gnb_du_served_cells_list_l           gnb_du_served_cells_list;
  rrc_version_s                        gnb_du_rrc_version;
  transport_layer_address_info_s       transport_layer_address_info;
  fixed_bitstring<10, false, true>     bap_address;
  extended_gnb_du_name_s               extended_gnb_du_name;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1SetupRequest ::= SEQUENCE
using f1_setup_request_s = elementary_procedure_option<f1_setup_request_ies_container>;

struct f1_setup_resp_ies_container {
  bool                                 gnb_cu_name_present                  = false;
  bool                                 cells_to_be_activ_list_present       = false;
  bool                                 transport_layer_address_info_present = false;
  bool                                 ul_bh_non_up_traffic_map_present     = false;
  bool                                 bap_address_present                  = false;
  bool                                 extended_gnb_cu_name_present         = false;
  uint16_t                             transaction_id;
  printable_string<1, 150, true, true> gnb_cu_name;
  cells_to_be_activ_list_l             cells_to_be_activ_list;
  rrc_version_s                        gnb_cu_rrc_version;
  transport_layer_address_info_s       transport_layer_address_info;
  ul_bh_non_up_traffic_map_s           ul_bh_non_up_traffic_map;
  fixed_bitstring<10, false, true>     bap_address;
  extended_gnb_cu_name_s               extended_gnb_cu_name;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// F1SetupResponse ::= SEQUENCE
using f1_setup_resp_s = elementary_procedure_option<f1_setup_resp_ies_container>;

struct gnb_cu_cfg_upd_ies_container {
  bool                                 cells_to_be_activ_list_present       = false;
  bool                                 cells_to_be_deactiv_list_present     = false;
  bool                                 gnb_cu_tnl_assoc_to_add_list_present = false;
  bool                                 gnb_cu_tnl_assoc_to_rem_list_present = false;
  bool                                 gnb_cu_tnl_assoc_to_upd_list_present = false;
  bool                                 cells_to_be_barred_list_present      = false;
  bool                                 protected_eutra_res_list_present     = false;
  bool                                 neighbour_cell_info_list_present     = false;
  bool                                 transport_layer_address_info_present = false;
  bool                                 ul_bh_non_up_traffic_map_present     = false;
  bool                                 bap_address_present                  = false;
  bool                                 cco_assist_info_present              = false;
  bool                                 cells_for_son_list_present           = false;
  bool                                 gnb_cu_name_present                  = false;
  bool                                 extended_gnb_cu_name_present         = false;
  uint16_t                             transaction_id;
  cells_to_be_activ_list_l             cells_to_be_activ_list;
  cells_to_be_deactiv_list_l           cells_to_be_deactiv_list;
  gnb_cu_tnl_assoc_to_add_list_l       gnb_cu_tnl_assoc_to_add_list;
  gnb_cu_tnl_assoc_to_rem_list_l       gnb_cu_tnl_assoc_to_rem_list;
  gnb_cu_tnl_assoc_to_upd_list_l       gnb_cu_tnl_assoc_to_upd_list;
  cells_to_be_barred_list_l            cells_to_be_barred_list;
  protected_eutra_res_list_l           protected_eutra_res_list;
  neighbour_cell_info_list_l           neighbour_cell_info_list;
  transport_layer_address_info_s       transport_layer_address_info;
  ul_bh_non_up_traffic_map_s           ul_bh_non_up_traffic_map;
  fixed_bitstring<10, false, true>     bap_address;
  cco_assist_info_s                    cco_assist_info;
  cells_for_son_list_l                 cells_for_son_list;
  printable_string<1, 150, true, true> gnb_cu_name;
  extended_gnb_cu_name_s               extended_gnb_cu_name;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBCUConfigurationUpdate ::= SEQUENCE
using gnb_cu_cfg_upd_s = elementary_procedure_option<gnb_cu_cfg_upd_ies_container>;

struct gnb_cu_cfg_upd_ack_ies_container {
  bool                                    cells_failed_to_be_activ_list_present         = false;
  bool                                    crit_diagnostics_present                      = false;
  bool                                    gnb_cu_tnl_assoc_setup_list_present           = false;
  bool                                    gnb_cu_tnl_assoc_failed_to_setup_list_present = false;
  bool                                    ded_si_delivery_needed_ue_list_present        = false;
  bool                                    transport_layer_address_info_present          = false;
  uint16_t                                transaction_id;
  cells_failed_to_be_activ_list_l         cells_failed_to_be_activ_list;
  crit_diagnostics_s                      crit_diagnostics;
  gnb_cu_tnl_assoc_setup_list_l           gnb_cu_tnl_assoc_setup_list;
  gnb_cu_tnl_assoc_failed_to_setup_list_l gnb_cu_tnl_assoc_failed_to_setup_list;
  ded_si_delivery_needed_ue_list_l        ded_si_delivery_needed_ue_list;
  transport_layer_address_info_s          transport_layer_address_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBCUConfigurationUpdateAcknowledge ::= SEQUENCE
using gnb_cu_cfg_upd_ack_s = elementary_procedure_option<gnb_cu_cfg_upd_ack_ies_container>;

struct gnb_cu_cfg_upd_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBCUConfigurationUpdateFailure ::= SEQUENCE
using gnb_cu_cfg_upd_fail_s = elementary_procedure_option<gnb_cu_cfg_upd_fail_ies_container>;

struct gnb_du_cfg_upd_ies_container {
  bool                                 served_cells_to_add_list_present       = false;
  bool                                 served_cells_to_modify_list_present    = false;
  bool                                 served_cells_to_delete_list_present    = false;
  bool                                 cells_status_list_present              = false;
  bool                                 ded_si_delivery_needed_ue_list_present = false;
  bool                                 gnb_du_id_present                      = false;
  bool                                 gnb_du_tnl_assoc_to_rem_list_present   = false;
  bool                                 transport_layer_address_info_present   = false;
  bool                                 coverage_mod_notif_present             = false;
  bool                                 gnb_du_name_present                    = false;
  bool                                 extended_gnb_du_name_present           = false;
  uint16_t                             transaction_id;
  served_cells_to_add_list_l           served_cells_to_add_list;
  served_cells_to_modify_list_l        served_cells_to_modify_list;
  served_cells_to_delete_list_l        served_cells_to_delete_list;
  cells_status_list_l                  cells_status_list;
  ded_si_delivery_needed_ue_list_l     ded_si_delivery_needed_ue_list;
  uint64_t                             gnb_du_id;
  gnb_du_tnl_assoc_to_rem_list_l       gnb_du_tnl_assoc_to_rem_list;
  transport_layer_address_info_s       transport_layer_address_info;
  coverage_mod_notif_s                 coverage_mod_notif;
  printable_string<1, 150, true, true> gnb_du_name;
  extended_gnb_du_name_s               extended_gnb_du_name;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUConfigurationUpdate ::= SEQUENCE
using gnb_du_cfg_upd_s = elementary_procedure_option<gnb_du_cfg_upd_ies_container>;

struct gnb_du_cfg_upd_ack_ies_container {
  bool                             cells_to_be_activ_list_present       = false;
  bool                             crit_diagnostics_present             = false;
  bool                             cells_to_be_deactiv_list_present     = false;
  bool                             transport_layer_address_info_present = false;
  bool                             ul_bh_non_up_traffic_map_present     = false;
  bool                             bap_address_present                  = false;
  bool                             cells_for_son_list_present           = false;
  uint16_t                         transaction_id;
  cells_to_be_activ_list_l         cells_to_be_activ_list;
  crit_diagnostics_s               crit_diagnostics;
  cells_to_be_deactiv_list_l       cells_to_be_deactiv_list;
  transport_layer_address_info_s   transport_layer_address_info;
  ul_bh_non_up_traffic_map_s       ul_bh_non_up_traffic_map;
  fixed_bitstring<10, false, true> bap_address;
  cells_for_son_list_l             cells_for_son_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUConfigurationUpdateAcknowledge ::= SEQUENCE
using gnb_du_cfg_upd_ack_s = elementary_procedure_option<gnb_du_cfg_upd_ack_ies_container>;

struct gnb_du_cfg_upd_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUConfigurationUpdateFailure ::= SEQUENCE
using gnb_du_cfg_upd_fail_s = elementary_procedure_option<gnb_du_cfg_upd_fail_ies_container>;

struct gnb_du_res_cfg_ies_container {
  bool                         activ_cells_to_be_upd_list_present = false;
  bool                         child_nodes_list_present           = false;
  bool                         neighbour_node_cells_list_present  = false;
  bool                         serving_cells_list_present         = false;
  uint16_t                     transaction_id;
  activ_cells_to_be_upd_list_l activ_cells_to_be_upd_list;
  child_nodes_list_l           child_nodes_list;
  neighbour_node_cells_list_l  neighbour_node_cells_list;
  serving_cells_list_l         serving_cells_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUResourceConfiguration ::= SEQUENCE
using gnb_du_res_cfg_s = elementary_procedure_option<gnb_du_res_cfg_ies_container>;

struct gnb_du_res_cfg_ack_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUResourceConfigurationAcknowledge ::= SEQUENCE
using gnb_du_res_cfg_ack_s = elementary_procedure_option<gnb_du_res_cfg_ack_ies_container>;

struct gnb_du_res_cfg_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUResourceConfigurationFailure ::= SEQUENCE
using gnb_du_res_cfg_fail_s = elementary_procedure_option<gnb_du_res_cfg_fail_ies_container>;

struct gnb_du_res_coordination_request_ies_container {
  bool                                ignore_res_coordination_container_present = false;
  uint16_t                            transaction_id;
  request_type_e                      request_type;
  unbounded_octstring<true>           eutra_nr_cell_res_coordination_req_container;
  ignore_res_coordination_container_e ignore_res_coordination_container;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUResourceCoordinationRequest ::= SEQUENCE
using gnb_du_res_coordination_request_s = elementary_procedure_option<gnb_du_res_coordination_request_ies_container>;

struct gnb_du_res_coordination_resp_ies_container {
  uint16_t                  transaction_id;
  unbounded_octstring<true> eutra_nr_cell_res_coordination_req_ack_container;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUResourceCoordinationResponse ::= SEQUENCE
using gnb_du_res_coordination_resp_s = elementary_procedure_option<gnb_du_res_coordination_resp_ies_container>;

struct gnb_du_status_ind_ies_container {
  bool                   iab_congestion_ind_present = false;
  uint16_t               transaction_id;
  gnb_du_overload_info_e gnb_du_overload_info;
  iab_congestion_ind_s   iab_congestion_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBDUStatusIndication ::= SEQUENCE
using gnb_du_status_ind_s = elementary_procedure_option<gnb_du_status_ind_ies_container>;

struct iab_tnl_address_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IABTNLAddressFailure ::= SEQUENCE
using iab_tnl_address_fail_s = elementary_procedure_option<iab_tnl_address_fail_ies_container>;

struct iab_tnl_address_request_ies_container {
  bool                            ia_bv4_addresses_requested_present    = false;
  bool                            iab_ip_v6_request_type_present        = false;
  bool                            iab_tnl_addresses_to_rem_list_present = false;
  bool                            iab_tnl_addresses_exception_present   = false;
  uint16_t                        transaction_id;
  ia_bv4_addresses_requested_s    ia_bv4_addresses_requested;
  iab_ip_v6_request_type_c        iab_ip_v6_request_type;
  iab_tnl_addresses_to_rem_list_l iab_tnl_addresses_to_rem_list;
  iab_tnl_addresses_exception_s   iab_tnl_addresses_exception;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IABTNLAddressRequest ::= SEQUENCE
using iab_tnl_address_request_s = elementary_procedure_option<iab_tnl_address_request_ies_container>;

struct iab_tnl_address_resp_ies_container {
  uint16_t                         transaction_id;
  iab_allocated_tnl_address_list_l iab_allocated_tnl_address_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IABTNLAddressResponse ::= SEQUENCE
using iab_tnl_address_resp_s = elementary_procedure_option<iab_tnl_address_resp_ies_container>;

struct iab_up_cfg_upd_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IABUPConfigurationUpdateFailure ::= SEQUENCE
using iab_up_cfg_upd_fail_s = elementary_procedure_option<iab_up_cfg_upd_fail_ies_container>;

struct iab_up_cfg_upd_request_ies_container {
  bool                            ul_up_tnl_info_to_upd_list_present    = false;
  bool                            ul_up_tnl_address_to_upd_list_present = false;
  uint16_t                        transaction_id;
  ul_up_tnl_info_to_upd_list_l    ul_up_tnl_info_to_upd_list;
  ul_up_tnl_address_to_upd_list_l ul_up_tnl_address_to_upd_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IABUPConfigurationUpdateRequest ::= SEQUENCE
using iab_up_cfg_upd_request_s = elementary_procedure_option<iab_up_cfg_upd_request_ies_container>;

struct iab_up_cfg_upd_resp_ies_container {
  bool                            crit_diagnostics_present              = false;
  bool                            dl_up_tnl_address_to_upd_list_present = false;
  uint16_t                        transaction_id;
  crit_diagnostics_s              crit_diagnostics;
  dl_up_tnl_address_to_upd_list_l dl_up_tnl_address_to_upd_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IABUPConfigurationUpdateResponse ::= SEQUENCE
using iab_up_cfg_upd_resp_s = elementary_procedure_option<iab_up_cfg_upd_resp_ies_container>;

struct meas_activation_ies_container {
  bool                      prs_meas_info_list_present = false;
  uint64_t                  gnb_cu_ue_f1ap_id;
  uint64_t                  gnb_du_ue_f1ap_id;
  activation_request_type_e activation_request_type;
  prs_meas_info_list_l      prs_meas_info_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementActivation ::= SEQUENCE
using meas_activation_s = elementary_procedure_option<meas_activation_ies_container>;

struct meas_precfg_confirm_ies_container {
  bool                        pos_meas_gap_pre_cfg_list_present = false;
  bool                        crit_diagnostics_present          = false;
  uint64_t                    gnb_cu_ue_f1ap_id;
  uint64_t                    gnb_du_ue_f1ap_id;
  pos_meas_gap_pre_cfg_list_s pos_meas_gap_pre_cfg_list;
  crit_diagnostics_s          crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementPreconfigurationConfirm ::= SEQUENCE
using meas_precfg_confirm_s = elementary_procedure_option<meas_precfg_confirm_ies_container>;

struct meas_precfg_refuse_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementPreconfigurationRefuse ::= SEQUENCE
using meas_precfg_refuse_s = elementary_procedure_option<meas_precfg_refuse_ies_container>;

struct meas_precfg_required_ies_container {
  uint64_t            gnb_cu_ue_f1ap_id;
  uint64_t            gnb_du_ue_f1ap_id;
  trp_prs_info_list_l trp_prs_info_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementPreconfigurationRequired ::= SEQUENCE
using meas_precfg_required_s = elementary_procedure_option<meas_precfg_required_ies_container>;

struct multicast_context_mod_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_mbs_f1ap_id;
  uint64_t           gnb_du_mbs_f1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextModificationFailure ::= SEQUENCE
using multicast_context_mod_fail_s = elementary_procedure_option<multicast_context_mod_fail_ies_container>;

struct multicast_context_mod_request_ies_container {
  bool                                   mbs_service_area_present                     = false;
  bool                                   multicast_m_rbs_to_be_setup_mod_list_present = false;
  bool                                   multicast_m_rbs_to_be_modified_list_present  = false;
  bool                                   multicast_m_rbs_to_be_released_list_present  = false;
  uint64_t                               gnb_cu_mbs_f1ap_id;
  uint64_t                               gnb_du_mbs_f1ap_id;
  mbs_service_area_c                     mbs_service_area;
  multicast_m_rbs_to_be_setup_mod_list_l multicast_m_rbs_to_be_setup_mod_list;
  multicast_m_rbs_to_be_modified_list_l  multicast_m_rbs_to_be_modified_list;
  multicast_m_rbs_to_be_released_list_l  multicast_m_rbs_to_be_released_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextModificationRequest ::= SEQUENCE
using multicast_context_mod_request_s = elementary_procedure_option<multicast_context_mod_request_ies_container>;

struct multicast_context_mod_resp_ies_container {
  bool                                          multicast_m_rbs_setup_mod_list_present              = false;
  bool                                          multicast_m_rbs_failed_to_be_setup_mod_list_present = false;
  bool                                          multicast_m_rbs_modified_list_present               = false;
  bool                                          multicast_m_rbs_failed_to_be_modified_list_present  = false;
  bool                                          crit_diagnostics_present                            = false;
  uint64_t                                      gnb_cu_mbs_f1ap_id;
  uint64_t                                      gnb_du_mbs_f1ap_id;
  multicast_m_rbs_setup_mod_list_l              multicast_m_rbs_setup_mod_list;
  multicast_m_rbs_failed_to_be_setup_mod_list_l multicast_m_rbs_failed_to_be_setup_mod_list;
  multicast_m_rbs_modified_list_l               multicast_m_rbs_modified_list;
  multicast_m_rbs_failed_to_be_modified_list_l  multicast_m_rbs_failed_to_be_modified_list;
  crit_diagnostics_s                            crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextModificationResponse ::= SEQUENCE
using multicast_context_mod_resp_s = elementary_procedure_option<multicast_context_mod_resp_ies_container>;

struct multicast_context_release_cmd_ies_container {
  uint64_t gnb_cu_mbs_f1ap_id;
  uint64_t gnb_du_mbs_f1ap_id;
  cause_c  cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextReleaseCommand ::= SEQUENCE
using multicast_context_release_cmd_s = elementary_procedure_option<multicast_context_release_cmd_ies_container>;

struct multicast_context_release_complete_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_mbs_f1ap_id;
  uint64_t           gnb_du_mbs_f1ap_id;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextReleaseComplete ::= SEQUENCE
using multicast_context_release_complete_s =
    elementary_procedure_option<multicast_context_release_complete_ies_container>;

struct multicast_context_release_request_ies_container {
  uint64_t gnb_cu_mbs_f1ap_id;
  uint64_t gnb_du_mbs_f1ap_id;
  cause_c  cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextReleaseRequest ::= SEQUENCE
using multicast_context_release_request_s =
    elementary_procedure_option<multicast_context_release_request_ies_container>;

struct multicast_context_setup_fail_ies_container {
  bool               gnb_du_mbs_f1ap_id_present = false;
  bool               crit_diagnostics_present   = false;
  uint64_t           gnb_cu_mbs_f1ap_id;
  uint64_t           gnb_du_mbs_f1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextSetupFailure ::= SEQUENCE
using multicast_context_setup_fail_s = elementary_procedure_option<multicast_context_setup_fail_ies_container>;

struct multicast_context_setup_request_ies_container {
  bool                               mbs_service_area_present = false;
  uint64_t                           gnb_cu_mbs_f1ap_id;
  mbs_session_id_s                   mbs_session_id;
  mbs_service_area_c                 mbs_service_area;
  snssai_s                           snssai;
  multicast_m_rbs_to_be_setup_list_l multicast_m_rbs_to_be_setup_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextSetupRequest ::= SEQUENCE
using multicast_context_setup_request_s = elementary_procedure_option<multicast_context_setup_request_ies_container>;

struct multicast_context_setup_resp_ies_container {
  bool                                      multicast_m_rbs_failed_to_be_setup_list_present = false;
  bool                                      crit_diagnostics_present                        = false;
  uint64_t                                  gnb_cu_mbs_f1ap_id;
  uint64_t                                  gnb_du_mbs_f1ap_id;
  multicast_m_rbs_setup_list_l              multicast_m_rbs_setup_list;
  multicast_m_rbs_failed_to_be_setup_list_l multicast_m_rbs_failed_to_be_setup_list;
  crit_diagnostics_s                        crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastContextSetupResponse ::= SEQUENCE
using multicast_context_setup_resp_s = elementary_procedure_option<multicast_context_setup_resp_ies_container>;

struct multicast_distribution_release_cmd_ies_container {
  uint64_t                                gnb_cu_mbs_f1ap_id;
  uint64_t                                gnb_du_mbs_f1ap_id;
  mbs_multicast_f1_u_context_descriptor_s mbs_multicast_f1_u_context_descriptor;
  cause_c                                 cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastDistributionReleaseCommand ::= SEQUENCE
using multicast_distribution_release_cmd_s =
    elementary_procedure_option<multicast_distribution_release_cmd_ies_container>;

struct multicast_distribution_release_complete_ies_container {
  bool                                    crit_diagnostics_present = false;
  uint64_t                                gnb_cu_mbs_f1ap_id;
  uint64_t                                gnb_du_mbs_f1ap_id;
  mbs_multicast_f1_u_context_descriptor_s mbs_multicast_f1_u_context_descriptor;
  crit_diagnostics_s                      crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastDistributionReleaseComplete ::= SEQUENCE
using multicast_distribution_release_complete_s =
    elementary_procedure_option<multicast_distribution_release_complete_ies_container>;

struct multicast_distribution_setup_fail_ies_container {
  bool                                    gnb_du_mbs_f1ap_id_present = false;
  bool                                    crit_diagnostics_present   = false;
  uint64_t                                gnb_cu_mbs_f1ap_id;
  uint64_t                                gnb_du_mbs_f1ap_id;
  mbs_multicast_f1_u_context_descriptor_s mbs_multicast_f1_u_context_descriptor;
  cause_c                                 cause;
  crit_diagnostics_s                      crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastDistributionSetupFailure ::= SEQUENCE
using multicast_distribution_setup_fail_s =
    elementary_procedure_option<multicast_distribution_setup_fail_ies_container>;

struct multicast_distribution_setup_request_ies_container {
  uint64_t                                  gnb_cu_mbs_f1ap_id;
  uint64_t                                  gnb_du_mbs_f1ap_id;
  mbs_multicast_f1_u_context_descriptor_s   mbs_multicast_f1_u_context_descriptor;
  multicast_f1_u_context_to_be_setup_list_l multicast_f1_u_context_to_be_setup_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastDistributionSetupRequest ::= SEQUENCE
using multicast_distribution_setup_request_s =
    elementary_procedure_option<multicast_distribution_setup_request_ies_container>;

struct multicast_distribution_setup_resp_ies_container {
  bool                                             multicast_f1_u_context_failed_to_be_setup_list_present = false;
  bool                                             crit_diagnostics_present                               = false;
  uint64_t                                         gnb_cu_mbs_f1ap_id;
  uint64_t                                         gnb_du_mbs_f1ap_id;
  mbs_multicast_f1_u_context_descriptor_s          mbs_multicast_f1_u_context_descriptor;
  multicast_f1_u_context_setup_list_l              multicast_f1_u_context_setup_list;
  multicast_f1_u_context_failed_to_be_setup_list_l multicast_f1_u_context_failed_to_be_setup_list;
  crit_diagnostics_s                               crit_diagnostics;
  fixed_octstring<4, true>                         multicast_f1_u_context_ref_cu;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastDistributionSetupResponse ::= SEQUENCE
using multicast_distribution_setup_resp_s =
    elementary_procedure_option<multicast_distribution_setup_resp_ies_container>;

struct multicast_group_paging_ies_container {
  bool                         ue_id_list_for_paging_list_present = false;
  bool                         mc_paging_cell_list_present        = false;
  mbs_session_id_s             mbs_session_id;
  ue_id_list_for_paging_list_l ue_id_list_for_paging_list;
  mc_paging_cell_list_l        mc_paging_cell_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastGroupPaging ::= SEQUENCE
using multicast_group_paging_s = elementary_procedure_option<multicast_group_paging_ies_container>;

struct network_access_rate_reduction_ies_container {
  uint16_t          transaction_id;
  uac_assist_info_s uac_assist_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NetworkAccessRateReduction ::= SEQUENCE
using network_access_rate_reduction_s = elementary_procedure_option<network_access_rate_reduction_ies_container>;

struct notify_ies_container {
  uint64_t          gnb_cu_ue_f1ap_id;
  uint64_t          gnb_du_ue_f1ap_id;
  drb_notify_list_l drb_notify_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Notify ::= SEQUENCE
using notify_s = elementary_procedure_option<notify_ies_container>;

struct pdc_meas_fail_ind_ies_container {
  uint64_t gnb_cu_ue_f1ap_id;
  uint64_t gnb_du_ue_f1ap_id;
  uint8_t  ran_ue_pdc_meas_id;
  cause_c  cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCMeasurementFailureIndication ::= SEQUENCE
using pdc_meas_fail_ind_s = elementary_procedure_option<pdc_meas_fail_ind_ies_container>;

struct pdc_meas_initiation_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  uint8_t            ran_ue_pdc_meas_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCMeasurementInitiationFailure ::= SEQUENCE
using pdc_meas_initiation_fail_s = elementary_procedure_option<pdc_meas_initiation_fail_ies_container>;

struct pdc_meas_initiation_request_ies_container {
  bool                   pdc_meas_periodicity_present = false;
  uint64_t               gnb_cu_ue_f1ap_id;
  uint64_t               gnb_du_ue_f1ap_id;
  uint8_t                ran_ue_pdc_meas_id;
  pdc_report_type_e      pdc_report_type;
  pdc_meas_periodicity_e pdc_meas_periodicity;
  pdc_meas_quantities_l  pdc_meas_quantities;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCMeasurementInitiationRequest ::= SEQUENCE
using pdc_meas_initiation_request_s = elementary_procedure_option<pdc_meas_initiation_request_ies_container>;

struct pdc_meas_initiation_resp_ies_container {
  bool               pdc_meas_result_present  = false;
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  uint8_t            ran_ue_pdc_meas_id;
  pdc_meas_result_s  pdc_meas_result;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCMeasurementInitiationResponse ::= SEQUENCE
using pdc_meas_initiation_resp_s = elementary_procedure_option<pdc_meas_initiation_resp_ies_container>;

struct pdc_meas_report_ies_container {
  uint64_t          gnb_cu_ue_f1ap_id;
  uint64_t          gnb_du_ue_f1ap_id;
  uint8_t           ran_ue_pdc_meas_id;
  pdc_meas_result_s pdc_meas_result;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCMeasurementReport ::= SEQUENCE
using pdc_meas_report_s = elementary_procedure_option<pdc_meas_report_ies_container>;

struct pdc_meas_termination_cmd_ies_container {
  uint64_t gnb_cu_ue_f1ap_id;
  uint64_t gnb_du_ue_f1ap_id;
  uint8_t  ran_ue_pdc_meas_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCMeasurementTerminationCommand ::= SEQUENCE
using pdc_meas_termination_cmd_s = elementary_procedure_option<pdc_meas_termination_cmd_ies_container>;

struct prs_cfg_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSConfigurationFailure ::= SEQUENCE
using prs_cfg_fail_s = elementary_procedure_option<prs_cfg_fail_ies_container>;

struct prs_cfg_request_ies_container {
  prs_cfg_request_type_e prs_cfg_request_type;
  prstrp_list_l          prstrp_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSConfigurationRequest ::= SEQUENCE
using prs_cfg_request_s = elementary_procedure_option<prs_cfg_request_ies_container>;

struct prs_cfg_resp_ies_container {
  bool               prs_tx_trp_list_present  = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  prs_tx_trp_list_l  prs_tx_trp_list;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSConfigurationResponse ::= SEQUENCE
using prs_cfg_resp_s = elementary_procedure_option<prs_cfg_resp_ies_container>;

struct pws_cancel_request_ies_container {
  bool                             broadcast_to_be_cancelled_list_present = false;
  bool                             cancel_all_warning_msgs_ind_present    = false;
  bool                             notif_info_present                     = false;
  uint16_t                         transaction_id;
  uint32_t                         numof_broadcast_request;
  broadcast_to_be_cancelled_list_l broadcast_to_be_cancelled_list;
  cancel_all_warning_msgs_ind_e    cancel_all_warning_msgs_ind;
  notif_info_s                     notif_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSCancelRequest ::= SEQUENCE
using pws_cancel_request_s = elementary_procedure_option<pws_cancel_request_ies_container>;

struct pws_cancel_resp_ies_container {
  bool                             cells_broadcast_cancelled_list_present = false;
  bool                             crit_diagnostics_present               = false;
  uint16_t                         transaction_id;
  cells_broadcast_cancelled_list_l cells_broadcast_cancelled_list;
  crit_diagnostics_s               crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSCancelResponse ::= SEQUENCE
using pws_cancel_resp_s = elementary_procedure_option<pws_cancel_resp_ies_container>;

struct pws_fail_ind_ies_container {
  bool                     pws_failed_nr_cgi_list_present = false;
  uint16_t                 transaction_id;
  pws_failed_nr_cgi_list_l pws_failed_nr_cgi_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSFailureIndication ::= SEQUENCE
using pws_fail_ind_s = elementary_procedure_option<pws_fail_ind_ies_container>;

struct pws_restart_ind_ies_container {
  uint16_t                       transaction_id;
  nr_cgi_list_for_restart_list_l nr_cgi_list_for_restart_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PWSRestartIndication ::= SEQUENCE
using pws_restart_ind_s = elementary_procedure_option<pws_restart_ind_ies_container>;

struct paging_ies_container {
  bool                                  paging_drx_present                          = false;
  bool                                  paging_prio_present                         = false;
  bool                                  paging_origin_present                       = false;
  bool                                  ran_ue_paging_drx_present                   = false;
  bool                                  cn_ue_paging_drx_present                    = false;
  bool                                  nr_paginge_drx_info_present                 = false;
  bool                                  nr_paginge_drx_infofor_rrc_inactive_present = false;
  bool                                  paging_cause_present                        = false;
  bool                                  pe_ip_s_assist_info_present                 = false;
  bool                                  ue_paging_cap_present                       = false;
  bool                                  extended_ue_id_idx_value_present            = false;
  ue_id_idx_value_c                     ue_id_idx_value;
  paging_id_c                           paging_id;
  paging_drx_e                          paging_drx;
  paging_prio_e                         paging_prio;
  paging_cell_list_l                    paging_cell_list;
  paging_origin_e                       paging_origin;
  paging_drx_e                          ran_ue_paging_drx;
  paging_drx_e                          cn_ue_paging_drx;
  nr_paginge_drx_info_s                 nr_paginge_drx_info;
  nr_paginge_drx_infofor_rrc_inactive_s nr_paginge_drx_infofor_rrc_inactive;
  paging_cause_e                        paging_cause;
  pe_ip_s_assist_info_s                 pe_ip_s_assist_info;
  ue_paging_cap_s                       ue_paging_cap;
  fixed_bitstring<16, false, true>      extended_ue_id_idx_value;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Paging ::= SEQUENCE
using paging_s = elementary_procedure_option<paging_ies_container>;

struct pos_sys_info_delivery_cmd_ies_container {
  uint16_t          transaction_id;
  nr_cgi_s          nr_cgi;
  pos_sitype_list_l pos_sitype_list;
  uint64_t          confirmed_ue_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PosSystemInformationDeliveryCommand ::= SEQUENCE
using pos_sys_info_delivery_cmd_s = elementary_procedure_option<pos_sys_info_delivery_cmd_ies_container>;

struct positioning_activation_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningActivationFailure ::= SEQUENCE
using positioning_activation_fail_s = elementary_procedure_option<positioning_activation_fail_ies_container>;

struct positioning_activation_request_ies_container {
  bool                             activation_time_present = false;
  uint64_t                         gnb_cu_ue_f1ap_id;
  uint64_t                         gnb_du_ue_f1ap_id;
  srs_type_c                       srs_type;
  fixed_bitstring<64, false, true> activation_time;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningActivationRequest ::= SEQUENCE
using positioning_activation_request_s = elementary_procedure_option<positioning_activation_request_ies_container>;

struct positioning_activation_resp_ies_container {
  bool               sys_frame_num_present    = false;
  bool               slot_num_present         = false;
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  uint16_t           sys_frame_num;
  uint8_t            slot_num;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningActivationResponse ::= SEQUENCE
using positioning_activation_resp_s = elementary_procedure_option<positioning_activation_resp_ies_container>;

struct positioning_assist_info_ctrl_ies_container {
  bool                          pos_assist_info_present             = false;
  bool                          pos_broadcast_present               = false;
  bool                          positioning_broadcast_cells_present = false;
  bool                          routing_id_present                  = false;
  uint16_t                      transaction_id;
  unbounded_octstring<true>     pos_assist_info;
  pos_broadcast_e               pos_broadcast;
  positioning_broadcast_cells_l positioning_broadcast_cells;
  unbounded_octstring<true>     routing_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningAssistanceInformationControl ::= SEQUENCE
using positioning_assist_info_ctrl_s = elementary_procedure_option<positioning_assist_info_ctrl_ies_container>;

struct positioning_assist_info_feedback_ies_container {
  bool                          pos_assist_info_fail_list_present   = false;
  bool                          positioning_broadcast_cells_present = false;
  bool                          routing_id_present                  = false;
  bool                          crit_diagnostics_present            = false;
  uint16_t                      transaction_id;
  unbounded_octstring<true>     pos_assist_info_fail_list;
  positioning_broadcast_cells_l positioning_broadcast_cells;
  unbounded_octstring<true>     routing_id;
  crit_diagnostics_s            crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningAssistanceInformationFeedback ::= SEQUENCE
using positioning_assist_info_feedback_s = elementary_procedure_option<positioning_assist_info_feedback_ies_container>;

struct positioning_deactivation_ies_container {
  uint64_t   gnb_cu_ue_f1ap_id;
  uint64_t   gnb_du_ue_f1ap_id;
  abort_tx_c abort_tx;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningDeactivation ::= SEQUENCE
using positioning_deactivation_s = elementary_procedure_option<positioning_deactivation_ies_container>;

struct positioning_info_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningInformationFailure ::= SEQUENCE
using positioning_info_fail_s = elementary_procedure_option<positioning_info_fail_ies_container>;

struct positioning_info_request_ies_container {
  bool                               requested_srs_tx_characteristics_present = false;
  bool                               ue_report_info_present                   = false;
  bool                               srs_pos_rrc_inactive_query_ind_present   = false;
  uint64_t                           gnb_cu_ue_f1ap_id;
  uint64_t                           gnb_du_ue_f1ap_id;
  requested_srs_tx_characteristics_s requested_srs_tx_characteristics;
  ue_report_info_s                   ue_report_info;
  srs_pos_rrc_inactive_query_ind_e   srs_pos_rrc_inactive_query_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningInformationRequest ::= SEQUENCE
using positioning_info_request_s = elementary_procedure_option<positioning_info_request_ies_container>;

struct positioning_info_resp_ies_container {
  bool                             srs_cfg_present                  = false;
  bool                             sfn_initisation_time_present     = false;
  bool                             crit_diagnostics_present         = false;
  bool                             srs_pos_rrc_inactive_cfg_present = false;
  uint64_t                         gnb_cu_ue_f1ap_id;
  uint64_t                         gnb_du_ue_f1ap_id;
  srs_cfg_s                        srs_cfg;
  fixed_bitstring<64, false, true> sfn_initisation_time;
  crit_diagnostics_s               crit_diagnostics;
  unbounded_octstring<true>        srs_pos_rrc_inactive_cfg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningInformationResponse ::= SEQUENCE
using positioning_info_resp_s = elementary_procedure_option<positioning_info_resp_ies_container>;

struct positioning_info_upd_ies_container {
  bool                             srs_cfg_present              = false;
  bool                             sfn_initisation_time_present = false;
  uint64_t                         gnb_cu_ue_f1ap_id;
  uint64_t                         gnb_du_ue_f1ap_id;
  srs_cfg_s                        srs_cfg;
  fixed_bitstring<64, false, true> sfn_initisation_time;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningInformationUpdate ::= SEQUENCE
using positioning_info_upd_s = elementary_procedure_option<positioning_info_upd_ies_container>;

struct positioning_meas_abort_ies_container {
  uint16_t transaction_id;
  uint32_t lmf_meas_id;
  uint32_t ran_meas_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningMeasurementAbort ::= SEQUENCE
using positioning_meas_abort_s = elementary_procedure_option<positioning_meas_abort_ies_container>;

struct positioning_meas_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  uint32_t           lmf_meas_id;
  uint32_t           ran_meas_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningMeasurementFailure ::= SEQUENCE
using positioning_meas_fail_s = elementary_procedure_option<positioning_meas_fail_ies_container>;

struct positioning_meas_fail_ind_ies_container {
  uint16_t transaction_id;
  uint32_t lmf_meas_id;
  uint32_t ran_meas_id;
  cause_c  cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningMeasurementFailureIndication ::= SEQUENCE
using positioning_meas_fail_ind_s = elementary_procedure_option<positioning_meas_fail_ind_ies_container>;

struct positioning_meas_report_ies_container {
  uint16_t               transaction_id;
  uint32_t               lmf_meas_id;
  uint32_t               ran_meas_id;
  pos_meas_result_list_l pos_meas_result_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningMeasurementReport ::= SEQUENCE
using positioning_meas_report_s = elementary_procedure_option<positioning_meas_report_ies_container>;

struct positioning_meas_request_ies_container {
  bool                             pos_meas_periodicity_present             = false;
  bool                             sfn_initisation_time_present             = false;
  bool                             srs_cfg_present                          = false;
  bool                             meas_beam_info_request_present           = false;
  bool                             sys_frame_num_present                    = false;
  bool                             slot_num_present                         = false;
  bool                             pos_meas_periodicity_extended_present    = false;
  bool                             resp_time_present                        = false;
  bool                             meas_characteristics_request_ind_present = false;
  bool                             meas_time_occasion_present               = false;
  bool                             pos_meas_amount_present                  = false;
  uint16_t                         transaction_id;
  uint32_t                         lmf_meas_id;
  uint32_t                         ran_meas_id;
  trp_meas_request_list_l          trp_meas_request_list;
  pos_report_characteristics_e     pos_report_characteristics;
  meas_periodicity_e               pos_meas_periodicity;
  pos_meas_quantities_l            pos_meas_quantities;
  fixed_bitstring<64, false, true> sfn_initisation_time;
  srs_cfg_s                        srs_cfg;
  meas_beam_info_request_e         meas_beam_info_request;
  uint16_t                         sys_frame_num;
  uint8_t                          slot_num;
  meas_periodicity_extended_e      pos_meas_periodicity_extended;
  resp_time_s                      resp_time;
  fixed_bitstring<16, false, true> meas_characteristics_request_ind;
  meas_time_occasion_e             meas_time_occasion;
  pos_meas_amount_e                pos_meas_amount;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningMeasurementRequest ::= SEQUENCE
using positioning_meas_request_s = elementary_procedure_option<positioning_meas_request_ies_container>;

struct positioning_meas_resp_ies_container {
  bool                   pos_meas_result_list_present = false;
  bool                   crit_diagnostics_present     = false;
  uint16_t               transaction_id;
  uint32_t               lmf_meas_id;
  uint32_t               ran_meas_id;
  pos_meas_result_list_l pos_meas_result_list;
  crit_diagnostics_s     crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningMeasurementResponse ::= SEQUENCE
using positioning_meas_resp_s = elementary_procedure_option<positioning_meas_resp_ies_container>;

struct positioning_meas_upd_ies_container {
  bool                             srs_cfg_present                          = false;
  bool                             trp_meas_upd_list_present                = false;
  bool                             meas_characteristics_request_ind_present = false;
  bool                             meas_time_occasion_present               = false;
  uint16_t                         transaction_id;
  uint32_t                         lmf_meas_id;
  uint32_t                         ran_meas_id;
  srs_cfg_s                        srs_cfg;
  trp_meas_upd_list_l              trp_meas_upd_list;
  fixed_bitstring<16, false, true> meas_characteristics_request_ind;
  meas_time_occasion_e             meas_time_occasion;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PositioningMeasurementUpdate ::= SEQUENCE
using positioning_meas_upd_s = elementary_procedure_option<positioning_meas_upd_ies_container>;

struct private_ie_container_empty_l {
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
  bool        qo_e_info_present = false;
  uint64_t    gnb_cu_ue_f1ap_id;
  uint64_t    gnb_du_ue_f1ap_id;
  qo_e_info_s qo_e_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoEInformationTransfer ::= SEQUENCE
using qo_e_info_transfer_s = elementary_procedure_option<qo_e_info_transfer_ies_container>;

struct rrc_delivery_report_ies_container {
  uint64_t              gnb_cu_ue_f1ap_id;
  uint64_t              gnb_du_ue_f1ap_id;
  rrc_delivery_status_s rrc_delivery_status;
  uint8_t               srb_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCDeliveryReport ::= SEQUENCE
using rrc_delivery_report_s = elementary_procedure_option<rrc_delivery_report_ies_container>;

struct ref_time_info_report_ies_container {
  uint16_t        transaction_id;
  time_ref_info_s time_ref_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReferenceTimeInformationReport ::= SEQUENCE
using ref_time_info_report_s = elementary_procedure_option<ref_time_info_report_ies_container>;

struct ref_time_info_report_ctrl_ies_container {
  uint16_t              transaction_id;
  report_request_type_s report_request_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReferenceTimeInformationReportingControl ::= SEQUENCE
using ref_time_info_report_ctrl_s = elementary_procedure_option<ref_time_info_report_ctrl_ies_container>;

struct reset_ies_container {
  uint16_t     transaction_id;
  cause_c      cause;
  reset_type_c reset_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Reset ::= SEQUENCE
using reset_s = elementary_procedure_option<reset_ies_container>;

struct reset_ack_ies_container {
  bool                                    ue_associated_lc_f1_conn_list_res_ack_present = false;
  bool                                    crit_diagnostics_present                      = false;
  uint16_t                                transaction_id;
  ue_associated_lc_f1_conn_list_res_ack_l ue_associated_lc_f1_conn_list_res_ack;
  crit_diagnostics_s                      crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResetAcknowledge ::= SEQUENCE
using reset_ack_s = elementary_procedure_option<reset_ack_ies_container>;

struct res_status_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  uint16_t           gnb_cu_meas_id;
  uint16_t           gnb_du_meas_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceStatusFailure ::= SEQUENCE
using res_status_fail_s = elementary_procedure_option<res_status_fail_ies_container>;

struct res_status_request_ies_container {
  bool                             gnb_du_meas_id_present         = false;
  bool                             report_characteristics_present = false;
  bool                             cell_to_report_list_present    = false;
  bool                             report_periodicity_present     = false;
  uint16_t                         transaction_id;
  uint16_t                         gnb_cu_meas_id;
  uint16_t                         gnb_du_meas_id;
  regist_request_e                 regist_request;
  fixed_bitstring<32, false, true> report_characteristics;
  cell_to_report_list_l            cell_to_report_list;
  report_periodicity_e             report_periodicity;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceStatusRequest ::= SEQUENCE
using res_status_request_s = elementary_procedure_option<res_status_request_ies_container>;

struct res_status_resp_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  uint16_t           gnb_cu_meas_id;
  uint16_t           gnb_du_meas_id;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceStatusResponse ::= SEQUENCE
using res_status_resp_s = elementary_procedure_option<res_status_resp_ies_container>;

struct res_status_upd_ies_container {
  bool                    hardware_load_ind_present     = false;
  bool                    tnl_capacity_ind_present      = false;
  bool                    cell_meas_result_list_present = false;
  uint16_t                transaction_id;
  uint16_t                gnb_cu_meas_id;
  uint16_t                gnb_du_meas_id;
  hardware_load_ind_s     hardware_load_ind;
  tnl_capacity_ind_s      tnl_capacity_ind;
  cell_meas_result_list_l cell_meas_result_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceStatusUpdate ::= SEQUENCE
using res_status_upd_s = elementary_procedure_option<res_status_upd_ies_container>;

struct sys_info_delivery_cmd_ies_container {
  uint16_t      transaction_id;
  nr_cgi_s      nr_cgi;
  sitype_list_l sitype_list;
  uint64_t      confirmed_ue_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationDeliveryCommand ::= SEQUENCE
using sys_info_delivery_cmd_s = elementary_procedure_option<sys_info_delivery_cmd_ies_container>;

struct trp_info_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRPInformationFailure ::= SEQUENCE
using trp_info_fail_s = elementary_procedure_option<trp_info_fail_ies_container>;

struct trp_info_request_ies_container {
  bool                         trp_list_present = false;
  uint16_t                     transaction_id;
  trp_list_l                   trp_list;
  trp_info_type_list_trp_req_l trp_info_type_list_trp_req;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRPInformationRequest ::= SEQUENCE
using trp_info_request_s = elementary_procedure_option<trp_info_request_ies_container>;

struct trp_info_resp_ies_container {
  bool                     crit_diagnostics_present = false;
  uint16_t                 transaction_id;
  trp_info_list_trp_resp_l trp_info_list_trp_resp;
  crit_diagnostics_s       crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRPInformationResponse ::= SEQUENCE
using trp_info_resp_s = elementary_procedure_option<trp_info_resp_ies_container>;

struct trace_start_ies_container {
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  trace_activation_s trace_activation;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TraceStart ::= SEQUENCE
using trace_start_s = elementary_procedure_option<trace_start_ies_container>;

struct write_replace_warning_request_ies_container {
  bool                         cells_to_be_broadcast_list_present = false;
  uint16_t                     transaction_id;
  pws_sys_info_s               pws_sys_info;
  uint32_t                     repeat_period;
  uint32_t                     numof_broadcast_request;
  cells_to_be_broadcast_list_l cells_to_be_broadcast_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WriteReplaceWarningRequest ::= SEQUENCE
using write_replace_warning_request_s = elementary_procedure_option<write_replace_warning_request_ies_container>;

struct write_replace_warning_resp_ies_container {
  bool                             cells_broadcast_completed_list_present = false;
  bool                             crit_diagnostics_present               = false;
  bool                             ded_si_delivery_needed_ue_list_present = false;
  uint16_t                         transaction_id;
  cells_broadcast_completed_list_l cells_broadcast_completed_list;
  crit_diagnostics_s               crit_diagnostics;
  ded_si_delivery_needed_ue_list_l ded_si_delivery_needed_ue_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WriteReplaceWarningResponse ::= SEQUENCE
using write_replace_warning_resp_s = elementary_procedure_option<write_replace_warning_resp_ies_container>;

} // namespace f1ap
} // namespace asn1

extern template struct asn1::protocol_ie_field_s<asn1::f1ap::access_and_mob_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::access_success_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::bap_map_cfg_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::bap_map_cfg_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::bap_map_cfg_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_mod_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_mod_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_mod_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_release_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_release_complete_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::broadcast_context_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::cu_du_radio_info_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::cell_traffic_trace_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::du_cu_radio_info_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::deactiv_trace_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::e_c_id_meas_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::e_c_id_meas_initiation_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::e_c_id_meas_initiation_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::e_c_id_meas_initiation_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::e_c_id_meas_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::e_c_id_meas_termination_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::error_ind_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ue_associated_lc_f1_conn_item_res_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::mc_paging_cell_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::paging_cell_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::trp_info_item_trp_resp_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::trp_info_type_item_trp_req_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ue_associated_lc_f1_conn_item_res_ack_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::f1_removal_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::f1_removal_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::f1_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::f1_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::f1_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_cu_cfg_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_cu_cfg_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_cu_cfg_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_cfg_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_cfg_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_cfg_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_res_cfg_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_res_cfg_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_res_cfg_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_res_coordination_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_res_coordination_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::gnb_du_status_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::iab_tnl_address_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::iab_tnl_address_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::iab_tnl_address_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::iab_up_cfg_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::iab_up_cfg_upd_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::iab_up_cfg_upd_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::meas_activation_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::meas_precfg_confirm_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::meas_precfg_refuse_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::meas_precfg_required_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_mod_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_mod_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_mod_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_release_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_release_complete_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_release_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_context_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_distribution_release_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_distribution_release_complete_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_distribution_setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_distribution_setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_distribution_setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::multicast_group_paging_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::network_access_rate_reduction_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::notify_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pdc_meas_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pdc_meas_initiation_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pdc_meas_initiation_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pdc_meas_initiation_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pdc_meas_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pdc_meas_termination_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::prs_cfg_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::prs_cfg_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::prs_cfg_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pws_cancel_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pws_cancel_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pws_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pws_restart_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::paging_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::pos_sys_info_delivery_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_activation_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_activation_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_activation_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_assist_info_ctrl_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_assist_info_feedback_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_deactivation_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_info_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_info_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_info_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_info_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_meas_abort_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_meas_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_meas_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_meas_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_meas_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_meas_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::positioning_meas_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::qo_e_info_transfer_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::rrc_delivery_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ref_time_info_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::ref_time_info_report_ctrl_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::reset_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::reset_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::res_status_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::res_status_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::res_status_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::res_status_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::sys_info_delivery_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::trp_info_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::trp_info_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::trp_info_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::trace_start_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::write_replace_warning_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::f1ap::write_replace_warning_resp_ies_o>;

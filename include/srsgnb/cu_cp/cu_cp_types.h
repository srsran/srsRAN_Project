/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/adt/slotted_array.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/nr_cgi.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include "srsgnb/ran/up_transport_layer_info.h"
#include "srsgnb/rlc/rlc_config.h"
#include "srsgnb/security/security.h"
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

namespace srsgnb {
namespace srs_cu_cp {

/// Maximum number of UEs supported by CU-CP (implementation-defined).
enum ue_index_t : uint16_t {
  MIN_UE_INDEX     = 0,
  MAX_UE_INDEX     = 1023,
  MAX_NOF_UES      = 1024,
  INVALID_UE_INDEX = MAX_NOF_UES
};

/// Maximum number of DUs supported by CU-CP (implementation-defined).
enum du_index_t : uint16_t { MIN_DU_INDEX = 0, MAX_DU_INDEX = 1, MAX_NOF_DUS = 2, INVALID_DU_INDEX = MAX_NOF_DUS };

/// Maximum number of CU-UPs supported by CU-CP (implementation-defined).
enum cu_up_index_t : uint16_t {
  MIN_CU_UP_INDEX     = 0,
  MAX_CU_UP_INDEX     = 1,
  MAX_NOF_CU_UPS      = 2,
  INVALID_CU_UP_INDEX = MAX_NOF_CU_UPS
};

#define MAX_NOF_CU_UES (du_index_t::MAX_NOF_DUS * ue_index_t::MAX_NOF_UES)

/// Maximum number of cells per DU supported by CU-CP (implementation-defined).
enum du_cell_index_t : uint16_t {
  MIN_DU_CELL_INDEX     = 0,
  MAX_DU_CELL_INDEX     = 15,
  MAX_NOF_DU_CELLS      = 16,
  INVALID_DU_CELL_INDEX = MAX_NOF_DU_CELLS
};

/// Convert integer to DU index type.
constexpr inline du_index_t int_to_du_index(std::underlying_type_t<du_index_t> idx)
{
  return static_cast<du_index_t>(idx);
}

constexpr inline std::underlying_type_t<du_index_t> du_index_to_int(du_index_t du_index)
{
  return static_cast<std::underlying_type_t<du_index_t>>(du_index);
}

/// Convert integer to CU-UP index type.
constexpr inline cu_up_index_t int_to_cu_up_index(std::underlying_type_t<cu_up_index_t> idx)
{
  return static_cast<cu_up_index_t>(idx);
}

/// Convert integer to CU UE index type.
constexpr inline ue_index_t int_to_ue_index(std::underlying_type_t<ue_index_t> idx)
{
  return static_cast<ue_index_t>(idx);
}

constexpr inline bool is_ue_index_valid(ue_index_t ue_idx)
{
  return ue_idx < MAX_NOF_UES;
}

/// Convert integer to CU cell index type.
inline du_cell_index_t int_to_du_cell_index(std::underlying_type_t<du_cell_index_t> idx)
{
  return static_cast<du_cell_index_t>(idx);
}

/// \brief CU_CP_UE_ID internally used to identify the UE CU-CP-wide.
/// \remark The CU_CP_UE_ID is derived from the DU index and the UE's index at the DU
/// by (DU_INDEX * MAX_NOF_UES) + UE_INDEX
enum class cu_cp_ue_id_t : uint64_t {
  min     = 0,
  max     = (MAX_NOF_DUS * MAX_NOF_UES) + MAX_NOF_UES,
  invalid = (MAX_NOF_DUS * MAX_NOF_UES) + MAX_NOF_UES + 1
};

/// Convert CU_CP_UE_ID  type to integer.
inline uint64_t cu_cp_ue_id_to_uint(cu_cp_ue_id_t id)
{
  return static_cast<uint64_t>(id);
}

/// Convert integer to CU_CP_UE_ID type.
inline cu_cp_ue_id_t uint_to_cu_cp_ue_id(std::underlying_type_t<cu_cp_ue_id_t> id)
{
  return static_cast<cu_cp_ue_id_t>(id);
}

inline cu_cp_ue_id_t get_cu_cp_ue_id(du_index_t du_index, ue_index_t ue_index)
{
  return uint_to_cu_cp_ue_id((du_index * MAX_NOF_UES) + ue_index);
}

inline cu_cp_ue_id_t get_cu_cp_ue_id(std::underlying_type_t<du_index_t> du_index,
                                     std::underlying_type_t<ue_index_t> ue_index)
{
  return uint_to_cu_cp_ue_id((du_index * MAX_NOF_UES) + ue_index);
}

inline ue_index_t get_ue_index_from_cu_cp_ue_id(cu_cp_ue_id_t ngap_id)
{
  if (ngap_id == cu_cp_ue_id_t::invalid) {
    return INVALID_UE_INDEX;
  }
  return int_to_ue_index(cu_cp_ue_id_to_uint(ngap_id) % MAX_NOF_UES);
}

inline du_index_t get_du_index_from_cu_cp_ue_id(cu_cp_ue_id_t ngap_id)
{
  if (ngap_id == cu_cp_ue_id_t::invalid) {
    return INVALID_DU_INDEX;
  }
  std::underlying_type_t<ue_index_t> ue_idx = get_ue_index_from_cu_cp_ue_id(ngap_id);
  return int_to_du_index((cu_cp_ue_id_to_uint(ngap_id) - ue_idx) / MAX_NOF_UES);
}

/// \brief RAN_UE_ID (non ASN1 type of RAN_UE_NGAP_ID).
/// \remark See TS 38.413 Section 9.3.3.2: RAN_UE_NGAP_ID valid values: (0..2^32-1)
enum class ran_ue_id_t : uint64_t { min = 0, max = std::numeric_limits<uint64_t>::max(), invalid = 0x1ffffffff };

/// Convert RAN_UE_ID type to integer.
inline uint64_t ran_ue_id_to_uint(ran_ue_id_t id)
{
  return static_cast<uint64_t>(id);
}

/// Convert integer to RAN_UE_ID type.
inline ran_ue_id_t uint_to_ran_ue_id(std::underlying_type_t<ran_ue_id_t> id)
{
  return static_cast<ran_ue_id_t>(id);
}

// ASN1 types converted to common types

// See TS 38.463 Section 9.3.1.21: PDU Session ID valid values: (0..255)
constexpr static uint16_t MAX_NOF_PDU_SESSIONS = 256;

/// \brief PDU Session ID.
/// \remark See TS 38.463 Section 9.3.1.21: PDU Session ID valid values: (0..255)
enum class pdu_session_id_t : uint16_t { min = 0, max = MAX_NOF_PDU_SESSIONS - 1, invalid = MAX_NOF_PDU_SESSIONS };

constexpr inline uint16_t pdu_session_id_to_uint(pdu_session_id_t id)
{
  return static_cast<uint16_t>(id);
}

/// Convert integer to PDU Session ID type.
constexpr inline pdu_session_id_t uint_to_pdu_session_id(uint16_t idx)
{
  return static_cast<pdu_session_id_t>(idx);
}

// See TS 38.463 Section 9.3.1.24: QoS Flow ID valid values: (0..63)
constexpr static uint8_t MAX_NOF_QOS_FLOWS = 64;

/// \brief QoS Flow ID.
/// \remark See TS 38.463 Section 9.3.1.21: QoS Flow ID valid values: (0..63)
enum class qos_flow_id_t : uint8_t { min = 0, max = MAX_NOF_QOS_FLOWS - 1, invalid = MAX_NOF_QOS_FLOWS };

constexpr inline uint8_t qos_flow_id_to_uint(qos_flow_id_t id)
{
  return static_cast<uint8_t>(id);
}

/// Convert integer to QoS Flow ID type.
constexpr inline qos_flow_id_t uint_to_qos_flow_id(uint8_t idx)
{
  return static_cast<qos_flow_id_t>(idx);
}

enum class cu_cp_cause_t : uint8_t {
  radio_network = 0,
  transport     = 1,
  nas           = 2,
  protocol      = 3,
  misc          = 4,
  choice_exts   = 5,
  nulltype      = 6
};

struct cu_cp_rohc_profiles {
  bool profile0x0001 = false;
  bool profile0x0002 = false;
  bool profile0x0003 = false;
  bool profile0x0004 = false;
  bool profile0x0006 = false;
  bool profile0x0101 = false;
  bool profile0x0102 = false;
  bool profile0x0103 = false;
  bool profile0x0104 = false;
};

struct cu_cp_rohc {
  cu_cp_rohc_profiles profiles;
  bool                drb_continue_rohc_present = false;
  optional<uint16_t>  max_cid;
};

struct cu_cp_ul_only_rohc_profiles {
  bool profile0x0006 = false;
};

struct cu_cp_ul_only_rohc {
  cu_cp_ul_only_rohc_profiles profiles;
  bool                        drb_continue_rohc_present = false;
  optional<uint16_t>          max_cid;
};

struct cu_cp_hdr_compress {
  optional<cu_cp_rohc>         rohc;
  optional<cu_cp_ul_only_rohc> ul_only_rohc;
};

struct cu_cp_drb {
  cu_cp_hdr_compress hdr_compress;
  optional<int16_t>  discard_timer;
  optional<uint8_t>  pdcp_sn_size_ul;
  optional<uint8_t>  pdcp_sn_size_dl;
  bool               integrity_protection_present   = false;
  bool               status_report_required_present = false;
  bool               out_of_order_delivery_present  = false;
};

struct cu_cp_primary_path {
  optional<uint8_t> cell_group;
  optional<uint8_t> lc_ch;
};

struct cu_cp_more_than_one_rlc {
  cu_cp_primary_path primary_path;
  optional<int32_t>  ul_data_split_thres;
  optional<bool>     pdcp_dupl;
};

struct cu_cp_pdcp_config {
  optional<cu_cp_drb>               drb;
  optional<cu_cp_more_than_one_rlc> more_than_one_rlc;
  optional<uint16_t>                t_reordering;
  bool                              ciphering_disabled_present = false;
};

struct cu_cp_sdap_config {
  pdu_session_id_t           pdu_session = pdu_session_id_t::invalid;
  std::string                sdap_hdr_dl;
  std::string                sdap_hdr_ul;
  bool                       default_drb                 = false;
  std::vector<qos_flow_id_t> mapped_qos_flows_to_add     = {};
  std::vector<qos_flow_id_t> mapped_qos_flows_to_release = {};
};

struct cu_cp_qos_characteristics {
  bool        is_dynamic_5qi = false;
  uint16_t    five_qi;
  uint8_t     prio_level_arp;
  std::string pre_emption_cap;
  std::string pre_emption_vulnerability;
};

struct qos_flow_setup_request_item {
  qos_flow_id_t             qos_flow_id = qos_flow_id_t::invalid;
  cu_cp_qos_characteristics qos_characteristics;
  optional<uint8_t>         erab_id;
  optional<std::string>     add_qos_flow_info;
  optional<std::string>     reflective_qos_attribute;
};

struct cu_cp_s_nssai {
  optional<uint64_t> sd;
  uint64_t           sst;
};

struct cu_cp_pdu_session_res_setup_item {
  pdu_session_id_t                                              pdu_session_id = pdu_session_id_t::invalid;
  byte_buffer                                                   pdu_session_nas_pdu;
  cu_cp_s_nssai                                                 s_nssai;
  uint64_t                                                      pdu_session_aggregate_maximum_bit_rate_dl;
  uint64_t                                                      pdu_session_aggregate_maximum_bit_rate_ul;
  up_transport_layer_info                                       ul_ngu_up_tnl_info;
  std::string                                                   pdu_session_type;
  slotted_id_vector<qos_flow_id_t, qos_flow_setup_request_item> qos_flow_setup_request_items;
};

struct cu_cp_pdu_session_resource_setup_message {
  cu_cp_ue_id_t                                                         cu_cp_ue_id = cu_cp_ue_id_t::invalid;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item> pdu_session_res_setup_items;
  uint64_t                                                              ue_aggregate_maximum_bit_rate_dl;
  std::string                                                           serving_plmn;
};

struct cu_cp_associated_qos_flow {
  qos_flow_id_t         qos_flow_id = qos_flow_id_t::invalid;
  optional<std::string> qos_flow_map_ind;
};

struct cu_cp_qos_flow_failed_to_setup_item {
  qos_flow_id_t qos_flow_id = qos_flow_id_t::invalid;
  cu_cp_cause_t cause;
};

struct cu_cp_qos_flow_per_tnl_information {
  up_transport_layer_info                                     up_tp_layer_info;
  slotted_id_vector<qos_flow_id_t, cu_cp_associated_qos_flow> associated_qos_flow_list;
};

struct cu_cp_security_result {
  std::string confidentiality_protection_result;
  std::string integrity_protection_result;
};

struct cu_cp_pdu_session_resource_setup_response_transfer {
  std::vector<cu_cp_qos_flow_per_tnl_information>                       add_dl_qos_flow_per_tnl_info = {};
  cu_cp_qos_flow_per_tnl_information                                    dlqos_flow_per_tnl_info;
  slotted_id_vector<qos_flow_id_t, cu_cp_associated_qos_flow>           associated_qos_flow_list;
  slotted_id_vector<qos_flow_id_t, cu_cp_qos_flow_failed_to_setup_item> qos_flow_failed_to_setup_list;
  optional<cu_cp_security_result>                                       security_result;
};

struct cu_cp_pdu_session_res_setup_response_item {
  pdu_session_id_t                                   pdu_session_id = pdu_session_id_t::invalid;
  cu_cp_pdu_session_resource_setup_response_transfer pdu_session_resource_setup_response_transfer;
};

struct cu_cp_pdu_session_resource_setup_unsuccessful_transfer {
  cu_cp_cause_t cause;
  // TODO: Add crit diagnostics
};

struct cu_cp_pdu_session_res_setup_failed_item {
  pdu_session_id_t                                       pdu_session_id = pdu_session_id_t::invalid;
  cu_cp_pdu_session_resource_setup_unsuccessful_transfer pdu_session_resource_setup_unsuccessful_transfer;
};

struct cu_cp_pdu_session_resource_setup_response_message {
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_response_item> pdu_session_res_setup_response_items;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_failed_item>   pdu_session_res_failed_to_setup_items;
  // TODO: Add crit diagnostics
};

struct cu_cp_drb_setup_message {
  drb_id_t                                                      drb_id;
  srsgnb::rlc_mode                                              rlc;
  cu_cp_qos_characteristics                                     qos_info;
  std::vector<up_transport_layer_info>                          gtp_tunnels = {};
  cu_cp_s_nssai                                                 s_nssai;
  slotted_id_vector<qos_flow_id_t, qos_flow_setup_request_item> qos_flows_mapped_to_drb;

  uint8_t dl_pdcp_sn_length; // id-DLPDCPSNLength 161
  uint8_t ul_pdcp_sn_length; // id-ULPDCPSNLength 192
};

struct cu_cp_ue_context_modification_request {
  ue_index_t                                           ue_index;
  slotted_id_vector<drb_id_t, cu_cp_drb_setup_message> cu_cp_drb_setup_msgs;
  optional<uint64_t>                                   ue_aggregate_maximum_bit_rate_ul;
};

struct cu_cp_du_to_cu_rrc_info {
  byte_buffer cell_group_cfg;
  byte_buffer meas_gap_cfg;
  byte_buffer requested_p_max_fr1;
};

struct cu_cp_dl_up_tnl_info_to_be_setup_item {
  up_transport_layer_info dl_up_tnl_info;
};

struct cu_cp_drbs_setup_modified_item {
  drb_id_t                                           drb_id;
  optional<lcid_t>                                   lcid;
  std::vector<cu_cp_dl_up_tnl_info_to_be_setup_item> dl_up_tnl_info_to_be_setup_list = {};
};

struct cu_cp_srbs_failed_to_be_setup_mod_item {
  srb_id_t                srb_id;
  optional<cu_cp_cause_t> cause;
};

struct cu_cp_drbs_failed_to_be_setup_modified_item {
  drb_id_t                drb_id;
  optional<cu_cp_cause_t> cause;
};

struct cu_cp_scell_failed_to_setup_mod_item {
  nr_cell_id_t            scell_id;
  optional<cu_cp_cause_t> cause;
};

struct cu_cp_crit_diagnostics_item {
  std::string iecrit;
  uint32_t    ie_id;
  std::string type_of_error;
};

struct cu_cp_crit_diagnostics {
  std::vector<cu_cp_crit_diagnostics_item> ies_crit_diagnostics = {};
  optional<uint16_t>                       proc_code;
  optional<std::string>                    trigger_msg;
  optional<std::string>                    proc_crit;
  optional<uint16_t>                       transaction_id;
};

struct cu_cp_associated_scell_item {
  nr_cell_id_t scell_id;
};

struct cu_cp_srbs_setup_modified_item {
  srb_id_t srb_id;
  lcid_t   lcid;
};

struct cu_cp_ue_context_modification_response {
  bool success = false;
  // ue context modification response
  byte_buffer                                                              res_coordination_transfer_container;
  cu_cp_du_to_cu_rrc_info                                                  du_to_cu_rrc_info;
  slotted_id_vector<drb_id_t, cu_cp_drbs_setup_modified_item>              drbs_setup_mod_list;
  slotted_id_vector<drb_id_t, cu_cp_drbs_setup_modified_item>              drbs_modified_list;
  slotted_id_vector<srb_id_t, cu_cp_srbs_failed_to_be_setup_mod_item>      srbs_failed_to_be_setup_mod_list;
  slotted_id_vector<drb_id_t, cu_cp_drbs_failed_to_be_setup_modified_item> drbs_failed_to_be_setup_mod_list;
  std::vector<cu_cp_scell_failed_to_setup_mod_item>                        scell_failed_to_setup_mod_list = {};
  slotted_id_vector<drb_id_t, cu_cp_drbs_failed_to_be_setup_modified_item> drbs_failed_to_be_modified_list;
  optional<std::string>                                                    inactivity_monitoring_resp;
  optional<srsgnb::rnti_t>                                                 c_rnti;
  std::vector<cu_cp_associated_scell_item>                                 associated_scell_list = {};
  slotted_id_vector<srb_id_t, cu_cp_srbs_setup_modified_item>              srbs_setup_mod_list;
  slotted_id_vector<srb_id_t, cu_cp_srbs_setup_modified_item>              srbs_modified_list;
  optional<std::string>                                                    full_cfg;

  // UE Context Modification Failure
  optional<cu_cp_cause_t> cause;

  // Common
  optional<cu_cp_crit_diagnostics> crit_diagnostics;
};

/// Arguments for the RRC Reconfiguration procedure.

struct cu_cp_srb_to_add_mod {
  bool                        reestablish_pdcp_present = false;
  bool                        discard_on_pdcp_present  = false;
  srb_id_t                    srb_id;
  optional<cu_cp_pdcp_config> pdcp_cfg;
};

struct cu_cp_cn_assoc {
  optional<uint8_t>           eps_bearer_id;
  optional<cu_cp_sdap_config> sdap_cfg;
};

struct cu_cp_drb_to_add_mod {
  bool                        reestablish_pdcp_present = false;
  bool                        recover_pdcp_present     = false;
  optional<cu_cp_cn_assoc>    cn_assoc;
  drb_id_t                    drb_id;
  optional<cu_cp_pdcp_config> pdcp_cfg;
};

struct cu_cp_security_algorithm_config {
  std::string           ciphering_algorithm;
  optional<std::string> integrity_prot_algorithm;
};

struct cu_cp_security_config {
  optional<cu_cp_security_algorithm_config> security_algorithm_cfg;
  optional<std::string>                     key_to_use;
};

struct cu_cp_radio_bearer_config {
  slotted_id_vector<srb_id_t, cu_cp_srb_to_add_mod> srb_to_add_mod_list;
  slotted_id_vector<drb_id_t, cu_cp_drb_to_add_mod> drb_to_add_mod_list;
  std::vector<drb_id_t>                             drb_to_release_list = {};
  optional<cu_cp_security_config>                   security_cfg;
  bool                                              srb3_to_release_present = false;
};

struct cu_cp_meas_config {
  // TODO: add meas config
};

struct cu_cp_master_key_upd {
  bool        key_set_change_ind = false;
  uint8_t     next_hop_chaining_count;
  byte_buffer nas_container;
};

struct cu_cp_delay_budget_report_cfg {
  std::string type;
  std::string delay_budget_report_prohibit_timer;
};

struct cu_cp_other_cfg {
  optional<cu_cp_delay_budget_report_cfg> delay_budget_report_cfg;
};

struct cu_cp_rrc_recfg_v1530_ies {
  bool                           full_cfg_present = false;
  byte_buffer                    master_cell_group;
  std::vector<byte_buffer>       ded_nas_msg_list = {};
  optional<cu_cp_master_key_upd> master_key_upd;
  byte_buffer                    ded_sib1_delivery;
  byte_buffer                    ded_sys_info_delivery;
  optional<cu_cp_other_cfg>      other_cfg;

  // TODO: Add rrc_recfg_v1540_ies_s
  // optional<cu_cp_rrc_recfg_v1540_ies> non_crit_ext;
};

struct cu_cp_rrc_reconfiguration_procedure_message {
  optional<cu_cp_radio_bearer_config> radio_bearer_cfg;
  byte_buffer                         secondary_cell_group;
  optional<cu_cp_meas_config>         meas_cfg;
  optional<cu_cp_rrc_recfg_v1530_ies> non_crit_ext;
};

} // namespace srs_cu_cp
} // namespace srsgnb

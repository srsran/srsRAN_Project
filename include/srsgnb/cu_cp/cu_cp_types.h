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
  return int_to_ue_index(cu_cp_ue_id_to_uint(ngap_id) % MAX_NOF_UES);
}

inline du_index_t get_du_index_from_cu_cp_ue_id(cu_cp_ue_id_t ngap_id)
{
  std::underlying_type_t<ue_index_t> ue_idx = get_ue_index_from_cu_cp_ue_id(ngap_id);
  return int_to_du_index((cu_cp_ue_id_to_uint(ngap_id) - ue_idx) / MAX_NOF_UES);
}

// ASN1 types converted to common types

enum class cu_cp_cause_t : uint8_t {
  radio_network = 0,
  transport     = 1,
  nas           = 2,
  protocol      = 3,
  misc          = 4,
  choice_exts   = 5,
  nulltype      = 6
};

struct qos_characteristics {
  bool        is_dynamic_5qi;
  uint16_t    five_qi;
  uint8_t     prio_level_arp;
  std::string pre_emption_cap;
  std::string pre_emption_vulnerability;
};

struct qos_flow_setup_request_item {
  uint8_t               qos_flow_id;
  qos_characteristics   qos_charact;
  optional<uint8_t>     erab_id;
  optional<std::string> add_qos_flow_info;
  optional<std::string> reflective_qos_attribute;
};

struct cu_cp_gtp_tunnel {
  std::string transport_layer_address;
  uint64_t    gtp_teid;
};

struct cu_cp_s_nssai {
  optional<uint64_t> sd;
  uint64_t           sst;
};

struct cu_cp_pdu_session_res_setup_item {
  uint16_t                                 pdu_session_id;
  byte_buffer                              pdu_session_nas_pdu;
  cu_cp_s_nssai                            s_nssai;
  uint64_t                                 pdu_session_aggregate_maximum_bit_rate_dl;
  uint64_t                                 pdu_session_aggregate_maximum_bit_rate_ul;
  cu_cp_gtp_tunnel                         ul_ngu_up_tnl_info;
  std::string                              pdu_session_type;
  std::vector<qos_flow_setup_request_item> qos_flow_setup_request_items;
};

struct cu_cp_pdu_session_resource_setup_message {
  std::vector<cu_cp_pdu_session_res_setup_item> pdu_session_res_setup_items;
  uint64_t                                      ue_aggregate_maximum_bit_rate_dl;
};

struct cu_cp_associated_qos_flow {
  uint8_t               qos_flow_id;
  optional<std::string> qos_flow_map_ind;
};

struct cu_cp_qos_flow_failed_to_setup_item {
  uint8_t       qos_flow_id;
  cu_cp_cause_t cause;
};

struct cu_cp_qos_flow_per_tnl_information {
  cu_cp_gtp_tunnel                       uptransport_layer_info;
  std::vector<cu_cp_associated_qos_flow> associated_qos_flow_list;
};

struct cu_cp_security_result {
  std::string confidentiality_protection_result;
  std::string integrity_protection_result;
};

struct cu_cp_pdu_session_resource_setup_response_transfer {
  std::vector<cu_cp_qos_flow_per_tnl_information>  add_dl_qos_flow_per_tnl_info;
  cu_cp_qos_flow_per_tnl_information               dlqos_flow_per_tnl_info;
  std::vector<cu_cp_associated_qos_flow>           associated_qos_flow_list;
  std::vector<cu_cp_qos_flow_failed_to_setup_item> qos_flow_failed_to_setup_list;
  optional<cu_cp_security_result>                  security_result;
};

struct cu_cp_pdu_session_res_setup_response_item {
  uint16_t                                           pdu_session_id;
  cu_cp_pdu_session_resource_setup_response_transfer pdu_session_resource_setup_response_transfer;
};

struct cu_cp_pdu_session_resource_setup_unsuccessful_transfer {
  cu_cp_cause_t cause;
  // TODO: Add crit diagnostics
};

struct cu_cp_pdu_session_res_setup_failed_item {
  uint16_t                                               pdu_session_id;
  cu_cp_pdu_session_resource_setup_unsuccessful_transfer pdu_session_resource_setup_unsuccessful_transfer;
};

struct cu_cp_pdu_session_resource_setup_response_message {
  std::vector<cu_cp_pdu_session_res_setup_response_item> pdu_session_res_setup_response_items;
  std::vector<cu_cp_pdu_session_res_setup_failed_item>   pdu_session_res_failed_to_setup_items;
  // TODO: Add crit diagnostics
};

} // namespace srs_cu_cp
} // namespace srsgnb

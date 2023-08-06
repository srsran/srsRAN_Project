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

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/ran/cause.h"
#include "srsran/ran/crit_diagnostics.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/s_nssai.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/rlc/rlc_config.h"
#include <cstdint>
#include <map>
#include <string>
#include <type_traits>
#include <vector>

namespace srsran {
namespace srs_cu_cp {

/// Maximum number of UEs per DU (implementation-defined).
const uint16_t MAX_NOF_UES_PER_DU = 1024;
/// Maximum number of DUs supported by CU-CP (implementation-defined).
const uint16_t MAX_NOF_DUS = 2;
/// Maximum number of UEs supported by CU-CP (implementation-defined).
#define MAX_NOF_CU_UES (MAX_NOF_DUS * MAX_NOF_UES_PER_DU)
/// Maximum number of CU-UPs supported by CU-CP (implementation-defined).
const uint16_t MAX_NOF_CU_UPS = 2;
/// Maximum number of cells per DU supported by CU-CP (implementation-defined).
const uint16_t MAX_NOF_DU_CELLS = 16;

/// \brief ue_index internally used to identify the UE CU-CP-wide.
/// \remark The ue_index is derived from the DU index and the maximum number of UEs per DU.
enum class ue_index_t : uint64_t { min = 0, max = MAX_NOF_CU_UES - 1, invalid = MAX_NOF_CU_UES };

/// Convert ue_index  type to integer.
inline uint64_t ue_index_to_uint(ue_index_t index)
{
  return static_cast<uint64_t>(index);
}

/// Convert integer to ue_index type.
inline ue_index_t uint_to_ue_index(std::underlying_type_t<ue_index_t> index)
{
  return static_cast<ue_index_t>(index);
}

/// Maximum number of DUs supported by CU-CP (implementation-defined).
enum class du_index_t : uint16_t { min = 0, max = MAX_NOF_DUS - 1, invalid = MAX_NOF_DUS };

/// Convert integer to DU index type.
constexpr inline du_index_t uint_to_du_index(std::underlying_type_t<du_index_t> index)
{
  return static_cast<du_index_t>(index);
}

/// Convert DU index type to integer.
constexpr inline std::underlying_type_t<du_index_t> du_index_to_uint(du_index_t du_index)
{
  return static_cast<std::underlying_type_t<du_index_t>>(du_index);
}

/// Get DU index from UE index.
inline du_index_t get_du_index_from_ue_index(ue_index_t index)
{
  if (index == ue_index_t::invalid) {
    return du_index_t::invalid;
  }
  return uint_to_du_index((ue_index_to_uint(index) / MAX_NOF_UES_PER_DU));
}

/// Generate a UE index from DU index and an index.
inline ue_index_t generate_ue_index(du_index_t du_index, uint16_t index)
{
  return uint_to_ue_index(du_index_to_uint(du_index) * MAX_NOF_UES_PER_DU + index);
}

/// Maximum number of CU-UPs supported by CU-CP (implementation-defined).
enum class cu_up_index_t : uint16_t { min = 0, max = MAX_NOF_CU_UPS - 1, invalid = MAX_NOF_CU_UPS };

/// Convert integer to CU-UP index type.
constexpr inline cu_up_index_t uint_to_cu_up_index(std::underlying_type_t<cu_up_index_t> index)
{
  return static_cast<cu_up_index_t>(index);
}

/// Convert CU-UP index type to integer.
constexpr inline std::underlying_type_t<cu_up_index_t> cu_up_index_to_uint(cu_up_index_t cu_up_index)
{
  return static_cast<std::underlying_type_t<cu_up_index_t>>(cu_up_index);
}

/// Maximum number of cells per DU supported by CU-CP (implementation-defined).
enum class du_cell_index_t : uint16_t { min = 0, max = MAX_NOF_DU_CELLS - 1, invalid = MAX_NOF_DU_CELLS };

/// Convert integer to DU cell index type.
inline du_cell_index_t uint_to_du_cell_index(std::underlying_type_t<du_cell_index_t> index)
{
  return static_cast<du_cell_index_t>(index);
}

/// Convert DU cell index type to integer.
constexpr inline std::underlying_type_t<du_cell_index_t> du_cell_index_to_uint(du_cell_index_t du_cell_index)
{
  return static_cast<std::underlying_type_t<du_cell_index_t>>(du_cell_index);
}

/// \brief The UE creation is triggered from the F1AP.
/// It carries an RRC container and the C-RNTI if the DU sent an Initial UL RRC transfer. If the user is created
/// during handover the RNTI is only allocated after the Random Access.
struct cu_cp_ue_creation_message {
  ue_index_t          ue_index = ue_index_t::invalid;
  nr_cell_global_id_t cgi;
  uint32_t            tac;
  byte_buffer         du_to_cu_rrc_container;
  rnti_t              c_rnti;
};

// Globally unique AMF identifier.
struct guami_t {
  optional<std::string> plmn;
  uint16_t              amf_set_id;
  uint8_t               amf_pointer;
  uint8_t               amf_region_id;
};

/// QoS Configuration, i.e. 5QI and the associated PDCP
/// and SDAP configuration for DRBs
struct cu_cp_qos_config {
  pdcp_config pdcp;
};

// ASN1 types converted to common types

struct cu_cp_tai {
  std::string plmn_id;
  uint32_t    tac;
};

struct cu_cp_tx_bw {
  subcarrier_spacing nr_scs;
  uint16_t           nr_nrb;
};

struct cu_cp_sul_info {
  uint32_t    sul_nr_arfcn;
  cu_cp_tx_bw sul_tx_bw;
};

struct cu_cp_supported_sul_freq_band_item {
  uint16_t freq_band_ind_nr;
};

struct cu_cp_freq_band_nr_item {
  uint16_t                                        freq_band_ind_nr;
  std::vector<cu_cp_supported_sul_freq_band_item> supported_sul_band_list;
};

struct cu_cp_nr_freq_info {
  uint32_t                             nr_arfcn;
  optional<cu_cp_sul_info>             sul_info;
  std::vector<cu_cp_freq_band_nr_item> freq_band_list_nr;
};

struct cu_cp_fdd_info {
  cu_cp_nr_freq_info ul_nr_freq_info;
  cu_cp_nr_freq_info dl_nr_freq_info;
  cu_cp_tx_bw        ul_tx_bw;
  cu_cp_tx_bw        dl_tx_bw;
};

struct cu_cp_tdd_info {
  cu_cp_nr_freq_info nr_freq_info;
  cu_cp_tx_bw        tx_bw;
};

struct cu_cp_nr_mode_info {
  // choice
  optional<cu_cp_fdd_info> fdd;
  optional<cu_cp_tdd_info> tdd;
};

struct cu_cp_served_cell_info {
  nr_cell_global_id_t      nr_cgi;
  pci_t                    nr_pci;
  optional<uint32_t>       five_gs_tac;
  optional<uint32_t>       cfg_eps_tac;
  std::vector<std::string> served_plmns;
  cu_cp_nr_mode_info       nr_mode_info;
  byte_buffer              meas_timing_cfg;
};

struct cu_cp_gnb_du_sys_info {
  byte_buffer mib_msg;
  byte_buffer sib1_msg;
};

struct cu_cp_du_served_cells_item {
  cu_cp_served_cell_info          served_cell_info;
  optional<cu_cp_gnb_du_sys_info> gnb_du_sys_info; // not optional for NG-RAN
};

struct cu_cp_user_location_info_nr {
  nr_cell_global_id_t nr_cgi;
  cu_cp_tai           tai;
  optional<uint64_t>  time_stamp;
};

struct cu_cp_alloc_and_retention_prio {
  uint8_t     prio_level_arp;
  std::string pre_emption_cap;
  std::string pre_emption_vulnerability;
};

struct cu_cp_gbr_qos_info {
  uint64_t              max_flow_bit_rate_dl;
  uint64_t              max_flow_bit_rate_ul;
  uint64_t              guaranteed_flow_bit_rate_dl;
  uint64_t              guaranteed_flow_bit_rate_ul;
  optional<std::string> notif_ctrl;
  optional<uint16_t>    max_packet_loss_rate_dl;
  optional<uint16_t>    max_packet_loss_rate_ul;
};

struct cu_cp_qos_flow_level_qos_params {
  qos_characteristics_t          qos_characteristics;
  cu_cp_alloc_and_retention_prio alloc_and_retention_prio;
  optional<cu_cp_gbr_qos_info>   gbr_qos_info;
  optional<bool>                 add_qos_flow_info;
  optional<bool>                 reflective_qos_attribute;
};

struct qos_flow_setup_request_item {
  qos_flow_id_t                   qos_flow_id = qos_flow_id_t::invalid;
  cu_cp_qos_flow_level_qos_params qos_flow_level_qos_params;
  optional<uint8_t>               erab_id;
};

struct cu_cp_pdu_session_res_setup_item {
  pdu_session_id_t                                              pdu_session_id = pdu_session_id_t::invalid;
  byte_buffer                                                   pdu_session_nas_pdu;
  s_nssai_t                                                     s_nssai;
  optional<uint64_t>                                            pdu_session_aggregate_maximum_bit_rate_dl;
  optional<uint64_t>                                            pdu_session_aggregate_maximum_bit_rate_ul;
  up_transport_layer_info                                       ul_ngu_up_tnl_info;
  std::string                                                   pdu_session_type;
  optional<security_indication_t>                               security_ind;
  slotted_id_vector<qos_flow_id_t, qos_flow_setup_request_item> qos_flow_setup_request_items;
};

struct cu_cp_pdu_session_resource_setup_request {
  ue_index_t                                                            ue_index = ue_index_t::invalid;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item> pdu_session_res_setup_items;
  uint64_t                                                              ue_aggregate_maximum_bit_rate_dl;
  std::string                                                           serving_plmn;
};

enum class cu_cp_qos_flow_map_ind { ul = 0, dl };

struct cu_cp_associated_qos_flow {
  qos_flow_id_t                    qos_flow_id = qos_flow_id_t::invalid;
  optional<cu_cp_qos_flow_map_ind> qos_flow_map_ind;
};
struct cu_cp_qos_flow_with_cause_item {
  qos_flow_id_t qos_flow_id = qos_flow_id_t::invalid;
  cause_t       cause       = cause_t::nulltype;
};

using cu_cp_qos_flow_failed_to_setup_item = cu_cp_qos_flow_with_cause_item;

struct cu_cp_qos_flow_per_tnl_information {
  up_transport_layer_info                                     up_tp_layer_info;
  slotted_id_vector<qos_flow_id_t, cu_cp_associated_qos_flow> associated_qos_flow_list;
};

struct cu_cp_pdu_session_resource_setup_response_transfer {
  std::vector<cu_cp_qos_flow_per_tnl_information>                       add_dl_qos_flow_per_tnl_info;
  cu_cp_qos_flow_per_tnl_information                                    dlqos_flow_per_tnl_info;
  slotted_id_vector<qos_flow_id_t, cu_cp_associated_qos_flow>           associated_qos_flow_list;
  slotted_id_vector<qos_flow_id_t, cu_cp_qos_flow_failed_to_setup_item> qos_flow_failed_to_setup_list;
  optional<security_result_t>                                           security_result;
};

struct cu_cp_pdu_session_res_setup_response_item {
  pdu_session_id_t                                   pdu_session_id = pdu_session_id_t::invalid;
  cu_cp_pdu_session_resource_setup_response_transfer pdu_session_resource_setup_response_transfer;
};

struct cu_cp_pdu_session_resource_setup_unsuccessful_transfer {
  cause_t                      cause = cause_t::nulltype;
  optional<crit_diagnostics_t> crit_diagnostics;
};

struct cu_cp_pdu_session_res_setup_failed_item {
  pdu_session_id_t                                       pdu_session_id = pdu_session_id_t::invalid;
  cu_cp_pdu_session_resource_setup_unsuccessful_transfer pdu_session_resource_setup_unsuccessful_transfer;
};

struct cu_cp_pdu_session_resource_setup_response {
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_response_item> pdu_session_res_setup_response_items;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_failed_item>   pdu_session_res_failed_to_setup_items;
  optional<crit_diagnostics_t>                                                   crit_diagnostics;
};

struct cu_cp_pdu_session_res_release_cmd_transfer {
  cause_t cause = cause_t::nulltype;
};

struct cu_cp_pdu_session_res_to_release_item_rel_cmd {
  pdu_session_id_t                           pdu_session_id = pdu_session_id_t::invalid;
  cu_cp_pdu_session_res_release_cmd_transfer pdu_session_res_release_cmd_transfer;
};

struct cu_cp_pdu_session_resource_release_command {
  ue_index_t         ue_index = ue_index_t::invalid;
  optional<uint16_t> ran_paging_prio;
  byte_buffer        nas_pdu;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_to_release_item_rel_cmd>
      pdu_session_res_to_release_list_rel_cmd;
};

struct cu_cp_volume_timed_report_item {
  uint64_t start_time_stamp;
  uint64_t end_time_stamp;
  uint64_t usage_count_ul;
  uint64_t usage_count_dl;
};

struct cu_cp_pdu_session_usage_report {
  std::string                                 rat_type;
  std::vector<cu_cp_volume_timed_report_item> pdu_session_timed_report_list;
};

struct cu_cp_qos_flows_usage_report_item {
  qos_flow_id_t                               qos_flow_id;
  std::string                                 rat_type;
  std::vector<cu_cp_volume_timed_report_item> qos_flows_timed_report_list;
};

struct cu_cp_secondary_rat_usage_info {
  optional<cu_cp_pdu_session_usage_report>                            pdu_session_usage_report;
  slotted_id_vector<qos_flow_id_t, cu_cp_qos_flows_usage_report_item> qos_flows_usage_report_list;
};

struct cu_cp_pdu_session_res_release_resp_transfer {
  optional<cu_cp_secondary_rat_usage_info> secondary_rat_usage_info;
};

struct cu_cp_pdu_session_res_released_item_rel_res {
  pdu_session_id_t                            pdu_session_id = pdu_session_id_t::invalid;
  cu_cp_pdu_session_res_release_resp_transfer pdu_session_res_release_resp_transfer;
};

struct cu_cp_pdu_session_resource_release_response {
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_released_item_rel_res>
                                        pdu_session_res_released_list_rel_res;
  optional<cu_cp_user_location_info_nr> user_location_info;
  optional<crit_diagnostics_t>          crit_diagnostics;
};

using cu_cp_qos_flow_add_or_mod_item = qos_flow_setup_request_item;

struct cu_cp_pdu_session_res_modify_request_transfer {
  // All IEs are optional
  // id-PDUSessionAggregateMaximumBitRate
  // id-UL-NGU-UP-TNLModifyList
  // id-NetworkInstance
  // id-QosFlowAddOrModifyRequestList
  slotted_id_vector<qos_flow_id_t, cu_cp_qos_flow_add_or_mod_item> qos_flow_add_or_modify_request_list;
  // id-QosFlowToReleaseList
  slotted_id_vector<qos_flow_id_t, cu_cp_qos_flow_with_cause_item> qos_flow_to_release_list;
  // id-AdditionalUL-NGU-UP-TNLInformation
  // id-CommonNetworkInstance
  // id-AdditionalRedundantUL-NGU-UP-TNLInformation
  // id-RedundantCommonNetworkInstance
};

struct cu_cp_pdu_session_res_modify_item_mod_req {
  pdu_session_id_t                              pdu_session_id = pdu_session_id_t::invalid;
  byte_buffer                                   nas_pdu;
  cu_cp_pdu_session_res_modify_request_transfer transfer;
};

struct cu_cp_pdu_session_resource_modify_request {
  ue_index_t                                                                     ue_index = ue_index_t::invalid;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_modify_item_mod_req> pdu_session_res_modify_items;
};

using cu_cp_pdu_session_resource_failed_to_modify_item = cu_cp_pdu_session_res_setup_failed_item;

struct qos_flow_add_or_mod_response_item {
  qos_flow_id_t qos_flow_id = qos_flow_id_t::invalid;
};

struct cu_cp_pdu_session_res_modify_response_transfer {
  std::vector<cu_cp_qos_flow_per_tnl_information>                               add_dl_qos_flow_per_tnl_info;
  optional<slotted_id_vector<qos_flow_id_t, qos_flow_add_or_mod_response_item>> qos_flow_add_or_modify_response_list;
};

struct cu_cp_pdu_session_resource_modify_response_item {
  pdu_session_id_t                               pdu_session_id;
  cu_cp_pdu_session_res_modify_response_transfer transfer;
};

struct cu_cp_pdu_session_resource_modify_response {
  ue_index_t ue_index = ue_index_t::invalid;
  // id-PDUSessionResourceModifyListModRes
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_resource_modify_response_item> pdu_session_res_modify_list;
  // id-PDUSessionResourceFailedToModifyListModRes
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_resource_failed_to_modify_item>
      pdu_session_res_failed_to_modify_list;
  // id-UserLocationInformation
  // id-CriticalityDiagnostics
};

struct cu_cp_ngap_ue_context_release_command {
  ue_index_t ue_index = ue_index_t::invalid;
  cause_t    cause    = cause_t::nulltype;
};

struct cu_cp_ue_context_release_request {
  ue_index_t                    ue_index = ue_index_t::invalid;
  std::vector<pdu_session_id_t> pdu_session_res_list_cxt_rel_req;
  cause_t                       cause = cause_t::nulltype;
};

struct cu_cp_recommended_cell_item {
  nr_cell_global_id_t ngran_cgi;
  optional<uint16_t>  time_stayed_in_cell;
};

struct cu_cp_recommended_cells_for_paging {
  std::vector<cu_cp_recommended_cell_item> recommended_cell_list;
};

struct cu_cp_global_gnb_id {
  std::string plmn_id;
  std::string gnb_id;
};

struct cu_cp_amf_paging_target {
  bool                          is_global_ran_node_id;
  bool                          is_tai;
  optional<cu_cp_global_gnb_id> global_ran_node_id;
  optional<cu_cp_tai>           tai;
};

struct cu_cp_recommended_ran_node_item {
  cu_cp_amf_paging_target amf_paging_target;
};

struct cu_cp_recommended_ran_nodes_for_paging {
  std::vector<cu_cp_recommended_ran_node_item> recommended_ran_node_list;
};

struct cu_cp_info_on_recommended_cells_and_ran_nodes_for_paging {
  cu_cp_recommended_cells_for_paging     recommended_cells_for_paging;
  cu_cp_recommended_ran_nodes_for_paging recommended_ran_nodes_for_paging;
};

struct cu_cp_ue_context_release_complete {
  optional<cu_cp_user_location_info_nr>                              user_location_info;
  optional<cu_cp_info_on_recommended_cells_and_ran_nodes_for_paging> info_on_recommended_cells_and_ran_nodes_for_paging;
  std::vector<pdu_session_id_t>                                      pdu_session_res_list_cxt_rel_cpl;
  optional<crit_diagnostics_t>                                       crit_diagnostics;
};

struct cu_cp_five_g_s_tmsi {
  uint16_t amf_set_id;
  uint8_t  amf_pointer;
  uint32_t five_g_tmsi;
};

struct cu_cp_tai_list_for_paging_item {
  cu_cp_tai tai;
};

struct cu_cp_ue_radio_cap_for_paging {
  byte_buffer ue_radio_cap_for_paging_of_nr;
};

struct cu_cp_assist_data_for_recommended_cells {
  cu_cp_recommended_cells_for_paging recommended_cells_for_paging;
};

struct cu_cp_paging_attempt_info {
  uint8_t               paging_attempt_count;
  uint8_t               intended_nof_paging_attempts;
  optional<std::string> next_paging_area_scope;
};

struct cu_cp_assist_data_for_paging {
  optional<cu_cp_assist_data_for_recommended_cells> assist_data_for_recommended_cells;
  optional<cu_cp_paging_attempt_info>               paging_attempt_info;
};

struct cu_cp_paging_message {
  cu_cp_five_g_s_tmsi                         ue_paging_id;
  optional<uint16_t>                          paging_drx;
  std::vector<cu_cp_tai_list_for_paging_item> tai_list_for_paging;
  optional<uint8_t>                           paging_prio;
  optional<cu_cp_ue_radio_cap_for_paging>     ue_radio_cap_for_paging;
  optional<bool>                              paging_origin;
  optional<cu_cp_assist_data_for_paging>      assist_data_for_paging;
};

struct cu_cp_inactivity_notification {
  ue_index_t                    ue_index    = ue_index_t::invalid;
  bool                          ue_inactive = false;
  std::vector<drb_id_t>         inactive_drbs;
  std::vector<pdu_session_id_t> inactive_pdu_sessions;
};

struct cu_cp_inter_du_handover_request {
  ue_index_t          source_ue_index = ue_index_t::invalid;
  du_index_t          target_du_index = du_index_t::invalid;
  nr_cell_global_id_t cgi;
  pci_t               target_pci = INVALID_PCI;
};

struct cu_cp_inter_du_handover_response {
  // Place-holder for possible return values.
  bool success = false;
};

struct cu_cp_inter_ngran_node_n2_handover_request {
  ue_index_t   ue_index = ue_index_t::invalid;
  unsigned     gnb_id;
  nr_cell_id_t nci;
};

struct cu_cp_inter_ngran_node_n2_handover_response {
  // Place-holder for possible return values.
  bool success = false;
};

} // namespace srs_cu_cp
} // namespace srsran

namespace fmt {

// ue index formatter
template <>
struct formatter<srsran::srs_cu_cp::ue_index_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_cu_cp::ue_index_t& idx, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (idx == srsran::srs_cu_cp::ue_index_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", (unsigned)idx);
  }
};

// du index formatter
template <>
struct formatter<srsran::srs_cu_cp::du_index_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_cu_cp::du_index_t& idx, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (idx == srsran::srs_cu_cp::du_index_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", (unsigned)idx);
  }
};

// cu_up index formatter
template <>
struct formatter<srsran::srs_cu_cp::cu_up_index_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_cu_cp::cu_up_index_t& idx, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (idx == srsran::srs_cu_cp::cu_up_index_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", (unsigned)idx);
  }
};

// du cell index formatter
template <>
struct formatter<srsran::srs_cu_cp::du_cell_index_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_cu_cp::du_cell_index_t& idx, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (idx == srsran::srs_cu_cp::du_cell_index_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", (unsigned)idx);
  }
};

} // namespace fmt

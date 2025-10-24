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

#pragma once

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/slotted_vector.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/ran/cause/e1ap_cause.h"
#include "srsran/ran/cause/f1ap_cause.h"
#include "srsran/ran/cause/ngap_cause.h"
#include "srsran/ran/crit_diagnostics.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/gnb_constants.h"
#include "srsran/ran/gnb_id.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/rb_id.h"
#include "srsran/ran/s_nssai.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/ran/tac.h"
#include "srsran/ran/up_transport_layer_info.h"
#include <cstdint>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

namespace srsran {
namespace srs_cu_cp {

/// Maximum number of DUs supported by CU-CP (implementation-defined).
const uint16_t MAX_NOF_DUS = 65535;
/// Maximum number of cells per DU supported by CU-CP (implementation-defined).
const uint16_t MAX_NOF_DU_CELLS = MAX_CELLS_PER_DU;
/// Maximum number of CU-UPs supported by CU-CP (implementation-defined).
const uint16_t MAX_NOF_CU_UPS = 65535;
/// Maximum number of UEs supported by CU-CP (implementation-defined).
const uint64_t MAX_NOF_CU_UES = 4294967295; // 2^32 - 1
/// Maximum number of AMFs supported by CU-CP (implementation-defined).
const uint16_t MAX_NOF_AMFS = 65535;

/// \brief ue_index internally used to identify the UE CU-CP-wide.
/// \remark The ue_index is derived from the maximum number of DUs and the maximum number of UEs per DU.
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
constexpr du_index_t uint_to_du_index(std::underlying_type_t<du_index_t> index)
{
  return static_cast<du_index_t>(index);
}

/// Convert DU index type to integer.
constexpr std::underlying_type_t<du_index_t> du_index_to_uint(du_index_t du_index)
{
  return static_cast<std::underlying_type_t<du_index_t>>(du_index);
}

/// Maximum number of CU-UPs supported by CU-CP (implementation-defined).
enum class cu_up_index_t : uint16_t { min = 0, max = MAX_NOF_CU_UPS - 1, invalid = MAX_NOF_CU_UPS };

/// Convert integer to CU-UP index type.
constexpr cu_up_index_t uint_to_cu_up_index(std::underlying_type_t<cu_up_index_t> index)
{
  return static_cast<cu_up_index_t>(index);
}

/// Convert CU-UP index type to integer.
constexpr std::underlying_type_t<cu_up_index_t> cu_up_index_to_uint(cu_up_index_t cu_up_index)
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
constexpr std::underlying_type_t<du_cell_index_t> du_cell_index_to_uint(du_cell_index_t du_cell_index)
{
  return static_cast<std::underlying_type_t<du_cell_index_t>>(du_cell_index);
}

/// Maximum number of AMFs supported by CU-CP (implementation-defined).
enum class amf_index_t : uint16_t { min = 0, max = MAX_NOF_AMFS - 1, invalid = MAX_NOF_AMFS };

/// Convert integer to AMF index type.
constexpr amf_index_t uint_to_amf_index(std::underlying_type_t<amf_index_t> index)
{
  return static_cast<amf_index_t>(index);
}

/// Convert AMF index type to integer.
constexpr std::underlying_type_t<amf_index_t> amf_index_to_uint(amf_index_t amf_index)
{
  return static_cast<std::underlying_type_t<amf_index_t>>(amf_index);
}

/// Notification from the E1AP/F1AP that transaction reference information for some UEs has been lost.
struct ue_transaction_info_loss_event {
  std::vector<ue_index_t> ues_lost;
};

/// Common interface reset message for E1AP (E1 Reset), F1AP (F1 Reset) and NGAP (NG Reset).
struct cu_cp_reset {
  std::variant<e1ap_cause_t, f1ap_cause_t, ngap_cause_t> cause;
  bool                                                   interface_reset = false;
  std::vector<ue_index_t>                                ues_to_reset;
};

/// QoS Configuration, i.e. 5QI and the associated PDCP
/// and SDAP configuration for DRBs
struct cu_cp_qos_config {
  pdcp_config pdcp;
};

// ASN1 types converted to common types

struct cu_cp_tai {
  plmn_identity plmn_id = plmn_identity::test_value();
  tac_t         tac;
};

struct cu_cp_user_location_info_nr {
  nr_cell_global_id_t     nr_cgi;
  cu_cp_tai               tai;
  std::optional<uint64_t> time_stamp;
};

/// <AMF Identifier> = <AMF Region ID><AMF Set ID><AMF Pointer>
/// with AMF Region ID length is 8 bits, AMF Set ID length is 10 bits and AMF Pointer length is 6 bits
struct cu_cp_amf_identifier_t {
  uint8_t  amf_region_id = 0;
  uint16_t amf_set_id    = 0;
  uint8_t  amf_pointer   = 0;
};

struct cu_cp_five_g_s_tmsi {
  cu_cp_five_g_s_tmsi() = default;

  cu_cp_five_g_s_tmsi(const bounded_bitset<48>& five_g_s_tmsi_) : five_g_s_tmsi(five_g_s_tmsi_)
  {
    srsran_assert(five_g_s_tmsi_.size() == 48, "Invalid size for 5G-S-TMSI ({})", five_g_s_tmsi_.size());
  }

  cu_cp_five_g_s_tmsi(uint64_t amf_set_id, uint64_t amf_pointer, uint64_t five_g_tmsi)
  {
    five_g_s_tmsi.emplace();
    five_g_s_tmsi->resize(48);
    five_g_s_tmsi->from_uint64((amf_set_id << 38U) + (amf_pointer << 32U) + five_g_tmsi);
  }

  uint16_t get_amf_set_id() const
  {
    srsran_assert(five_g_s_tmsi.has_value(), "five_g_s_tmsi is not set");
    return five_g_s_tmsi.value().to_uint64() >> 38U;
  }

  uint8_t get_amf_pointer() const
  {
    srsran_assert(five_g_s_tmsi.has_value(), "five_g_s_tmsi is not set");
    return (five_g_s_tmsi.value().to_uint64() & 0x3f00000000) >> 32U;
  }

  uint32_t get_five_g_tmsi() const
  {
    srsran_assert(five_g_s_tmsi.has_value(), "five_g_s_tmsi is not set");
    return (five_g_s_tmsi.value().to_uint64() & 0xffffffff);
  }

  uint64_t to_number() const { return five_g_s_tmsi->to_uint64(); }

private:
  std::optional<bounded_bitset<48>> five_g_s_tmsi;
};

struct cu_cp_initial_ue_message {
  ue_index_t                         ue_index = ue_index_t::invalid;
  byte_buffer                        nas_pdu;
  establishment_cause_t              establishment_cause;
  cu_cp_user_location_info_nr        user_location_info;
  std::optional<cu_cp_five_g_s_tmsi> five_g_s_tmsi;
  std::optional<uint16_t>            amf_set_id;
};

struct cu_cp_ul_nas_transport {
  ue_index_t                  ue_index = ue_index_t::invalid;
  byte_buffer                 nas_pdu;
  cu_cp_user_location_info_nr user_location_info;
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
  std::optional<cu_cp_sul_info>        sul_info;
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
  std::optional<cu_cp_fdd_info> fdd;
  std::optional<cu_cp_tdd_info> tdd;
};

struct cu_cp_served_cell_info {
  nr_cell_global_id_t        nr_cgi;
  pci_t                      nr_pci;
  std::optional<tac_t>       five_gs_tac;
  std::optional<tac_t>       cfg_eps_tac;
  std::vector<plmn_identity> served_plmns;
  cu_cp_nr_mode_info         nr_mode_info;
  byte_buffer                meas_timing_cfg;

  cu_cp_served_cell_info() = default;
  cu_cp_served_cell_info(const cu_cp_served_cell_info& other) :
    nr_cgi(other.nr_cgi),
    nr_pci(other.nr_pci),
    five_gs_tac(other.five_gs_tac),
    cfg_eps_tac(other.cfg_eps_tac),
    served_plmns(other.served_plmns),
    nr_mode_info(other.nr_mode_info),
    meas_timing_cfg(other.meas_timing_cfg.copy())
  {
  }
  cu_cp_served_cell_info& operator=(const cu_cp_served_cell_info& other)
  {
    if (this != &other) {
      nr_cgi          = other.nr_cgi;
      nr_pci          = other.nr_pci;
      five_gs_tac     = other.five_gs_tac;
      cfg_eps_tac     = other.cfg_eps_tac;
      served_plmns    = other.served_plmns;
      nr_mode_info    = other.nr_mode_info;
      meas_timing_cfg = other.meas_timing_cfg.copy();
    }
    return *this;
  }
};

struct cu_cp_gnb_du_sys_info {
  byte_buffer mib_msg;
  byte_buffer sib1_msg;

  cu_cp_gnb_du_sys_info() = default;
  cu_cp_gnb_du_sys_info(const cu_cp_gnb_du_sys_info& other) : mib_msg(other.mib_msg), sib1_msg(other.sib1_msg) {}
  cu_cp_gnb_du_sys_info& operator=(const cu_cp_gnb_du_sys_info& other)
  {
    if (this != &other) {
      mib_msg  = other.mib_msg.copy();
      sib1_msg = other.sib1_msg.copy();
    }
    return *this;
  }
};

struct cu_cp_du_served_cells_item {
  cu_cp_served_cell_info               served_cell_info;
  std::optional<cu_cp_gnb_du_sys_info> gnb_du_sys_info; // not optional for NG-RAN
};

struct qos_flow_setup_request_item {
  qos_flow_id_t                 qos_flow_id = qos_flow_id_t::invalid;
  qos_flow_level_qos_parameters qos_flow_level_qos_params;
  std::optional<uint8_t>        erab_id;
};

struct cu_cp_pdu_session_res_setup_item {
  pdu_session_id_t                                              pdu_session_id = pdu_session_id_t::invalid;
  byte_buffer                                                   pdu_session_nas_pdu;
  s_nssai_t                                                     s_nssai;
  std::optional<uint64_t>                                       pdu_session_aggregate_maximum_bit_rate_dl;
  std::optional<uint64_t>                                       pdu_session_aggregate_maximum_bit_rate_ul;
  up_transport_layer_info                                       ul_ngu_up_tnl_info;
  pdu_session_type_t                                            pdu_session_type;
  std::optional<security_indication_t>                          security_ind;
  slotted_id_vector<qos_flow_id_t, qos_flow_setup_request_item> qos_flow_setup_request_items;
};

struct cu_cp_pdu_session_resource_setup_request {
  ue_index_t                                                            ue_index = ue_index_t::invalid;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item> pdu_session_res_setup_items;
  uint64_t                                                              ue_aggregate_maximum_bit_rate_dl;
  uint64_t                                                              ue_aggregate_maximum_bit_rate_ul;
  plmn_identity                                                         serving_plmn = plmn_identity::test_value();
  byte_buffer                                                           nas_pdu; ///< optional NAS PDU
};

enum class cu_cp_qos_flow_map_ind { ul = 0, dl };

struct cu_cp_associated_qos_flow {
  qos_flow_id_t                         qos_flow_id = qos_flow_id_t::invalid;
  std::optional<cu_cp_qos_flow_map_ind> qos_flow_map_ind;
};
struct cu_cp_qos_flow_with_cause_item {
  qos_flow_id_t qos_flow_id = qos_flow_id_t::invalid;
  ngap_cause_t  cause;
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
  std::optional<security_result_t>                                      security_result;
};

struct cu_cp_pdu_session_res_setup_response_item {
  pdu_session_id_t                                   pdu_session_id = pdu_session_id_t::invalid;
  cu_cp_pdu_session_resource_setup_response_transfer pdu_session_resource_setup_response_transfer;
};

struct cu_cp_pdu_session_resource_setup_unsuccessful_transfer {
  ngap_cause_t                      cause;
  std::optional<crit_diagnostics_t> crit_diagnostics;
};

struct cu_cp_pdu_session_res_setup_failed_item {
  pdu_session_id_t                                       pdu_session_id = pdu_session_id_t::invalid;
  cu_cp_pdu_session_resource_setup_unsuccessful_transfer unsuccessful_transfer;
};

struct cu_cp_pdu_session_resource_setup_response {
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_response_item> pdu_session_res_setup_response_items;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_failed_item>   pdu_session_res_failed_to_setup_items;
  std::optional<crit_diagnostics_t>                                              crit_diagnostics;
};

struct cu_cp_pdu_session_res_release_cmd_transfer {
  ngap_cause_t cause;
};

struct cu_cp_pdu_session_res_to_release_item_rel_cmd {
  pdu_session_id_t                           pdu_session_id = pdu_session_id_t::invalid;
  cu_cp_pdu_session_res_release_cmd_transfer pdu_session_res_release_cmd_transfer;
};

struct cu_cp_pdu_session_resource_release_command {
  ue_index_t              ue_index = ue_index_t::invalid;
  std::optional<uint16_t> ran_paging_prio;
  byte_buffer             nas_pdu;
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
  std::optional<cu_cp_pdu_session_usage_report>                       pdu_session_usage_report;
  slotted_id_vector<qos_flow_id_t, cu_cp_qos_flows_usage_report_item> qos_flows_usage_report_list;
};

struct cu_cp_pdu_session_res_release_resp_transfer {
  std::optional<cu_cp_secondary_rat_usage_info> secondary_rat_usage_info;
};

struct cu_cp_pdu_session_res_released_item_rel_res {
  pdu_session_id_t                            pdu_session_id = pdu_session_id_t::invalid;
  cu_cp_pdu_session_res_release_resp_transfer resp_transfer;
};

struct cu_cp_pdu_session_resource_release_response {
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_released_item_rel_res> released_pdu_sessions;
  std::optional<cu_cp_user_location_info_nr>                                       user_location_info;
  std::optional<crit_diagnostics_t>                                                crit_diagnostics;
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
  std::vector<cu_cp_qos_flow_per_tnl_information> add_dl_qos_flow_per_tnl_info;
  std::optional<slotted_id_vector<qos_flow_id_t, qos_flow_add_or_mod_response_item>>
      qos_flow_add_or_modify_response_list;
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

struct cu_cp_ue_context_release_command {
  ue_index_t                          ue_index = ue_index_t::invalid;
  ngap_cause_t                        cause;
  bool                                requires_rrc_release = true;
  std::optional<std::chrono::seconds> release_wait_time    = std::nullopt;
};

struct cu_cp_ue_context_release_request {
  ue_index_t                    ue_index = ue_index_t::invalid;
  std::vector<pdu_session_id_t> pdu_session_res_list_cxt_rel_req;
  ngap_cause_t                  cause;
};

struct cu_cp_recommended_cell_item {
  nr_cell_global_id_t     ngran_cgi;
  std::optional<uint16_t> time_stayed_in_cell;
};

struct cu_cp_recommended_cells_for_paging {
  std::vector<cu_cp_recommended_cell_item> recommended_cell_list;
};

struct cu_cp_global_gnb_id {
  plmn_identity plmn_id = plmn_identity::test_value();
  gnb_id_t      gnb_id;
};

struct cu_cp_amf_paging_target {
  bool                               is_global_ran_node_id;
  bool                               is_tai;
  std::optional<cu_cp_global_gnb_id> global_ran_node_id;
  std::optional<cu_cp_tai>           tai;
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
  ue_index_t                                 ue_index = ue_index_t::invalid;
  std::optional<cu_cp_user_location_info_nr> user_location_info;
  std::optional<cu_cp_info_on_recommended_cells_and_ran_nodes_for_paging>
                                    info_on_recommended_cells_and_ran_nodes_for_paging;
  std::vector<pdu_session_id_t>     pdu_session_res_list_cxt_rel_cpl;
  std::optional<crit_diagnostics_t> crit_diagnostics;
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
  uint8_t                    paging_attempt_count;
  uint8_t                    intended_nof_paging_attempts;
  std::optional<std::string> next_paging_area_scope;
};

struct cu_cp_assist_data_for_paging {
  std::optional<cu_cp_assist_data_for_recommended_cells> assist_data_for_recommended_cells;
  std::optional<cu_cp_paging_attempt_info>               paging_attempt_info;
};

struct cu_cp_paging_message {
  cu_cp_five_g_s_tmsi                          ue_paging_id;
  std::optional<uint16_t>                      paging_drx;
  std::vector<cu_cp_tai_list_for_paging_item>  tai_list_for_paging;
  std::optional<uint8_t>                       paging_prio;
  std::optional<cu_cp_ue_radio_cap_for_paging> ue_radio_cap_for_paging;
  std::optional<bool>                          paging_origin;
  std::optional<cu_cp_assist_data_for_paging>  assist_data_for_paging;
};

struct cu_cp_bearer_context_release_request {
  ue_index_t   ue_index = ue_index_t::invalid;
  ngap_cause_t cause;
};

struct cu_cp_inactivity_notification {
  ue_index_t                    ue_index    = ue_index_t::invalid;
  bool                          ue_inactive = false;
  std::vector<drb_id_t>         inactive_drbs;
  std::vector<pdu_session_id_t> inactive_pdu_sessions;
};

struct cu_cp_intra_cu_handover_request {
  ue_index_t          source_ue_index = ue_index_t::invalid;
  du_index_t          target_du_index = du_index_t::invalid;
  nr_cell_global_id_t cgi;
  pci_t               target_pci = INVALID_PCI;
};

struct cu_cp_intra_cu_handover_response {
  // Place-holder for possible return values.
  bool success = false;
};

} // namespace srs_cu_cp
} // namespace srsran

namespace fmt {

// UE index formatter.
template <>
struct formatter<srsran::srs_cu_cp::ue_index_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_cu_cp::ue_index_t& idx, FormatContext& ctx) const
  {
    if (idx == srsran::srs_cu_cp::ue_index_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", (unsigned)idx);
  }
};

// DU index formatter.
template <>
struct formatter<srsran::srs_cu_cp::du_index_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_cu_cp::du_index_t& idx, FormatContext& ctx) const
  {
    if (idx == srsran::srs_cu_cp::du_index_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", (unsigned)idx);
  }
};

// CU-UP index formatter.
template <>
struct formatter<srsran::srs_cu_cp::cu_up_index_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_cu_cp::cu_up_index_t& idx, FormatContext& ctx) const
  {
    if (idx == srsran::srs_cu_cp::cu_up_index_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", (unsigned)idx);
  }
};

// DU cell index formatter.
template <>
struct formatter<srsran::srs_cu_cp::du_cell_index_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_cu_cp::du_cell_index_t& idx, FormatContext& ctx) const
  {
    if (idx == srsran::srs_cu_cp::du_cell_index_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", (unsigned)idx);
  }
};

// AMF index formatter.
template <>
struct formatter<srsran::srs_cu_cp::amf_index_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_cu_cp::amf_index_t& idx, FormatContext& ctx) const
  {
    if (idx == srsran::srs_cu_cp::amf_index_t::invalid) {
      return format_to(ctx.out(), "invalid");
    }
    return format_to(ctx.out(), "{}", (unsigned)idx);
  }
};

} // namespace fmt

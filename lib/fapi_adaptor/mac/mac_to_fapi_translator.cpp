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

#include "mac_to_fapi_translator.h"
#include "srsran/fapi/message_validators.h"
#include "srsran/fapi/slot_last_message_notifier.h"
#include "srsran/fapi/slot_message_gateway.h"
#include "srsran/fapi_adaptor/mac/messages/pdcch.h"
#include "srsran/fapi_adaptor/mac/messages/pdsch.h"
#include "srsran/fapi_adaptor/mac/messages/prach.h"
#include "srsran/fapi_adaptor/mac/messages/pucch.h"
#include "srsran/fapi_adaptor/mac/messages/pusch.h"
#include "srsran/fapi_adaptor/mac/messages/ssb.h"

using namespace srsran;
using namespace fapi_adaptor;

namespace {

/// Helper struct to group DCIs into FAPI PDCCH PDUs.
struct pdcch_group {
  const dci_context_information* info;
  const dci_payload*             payload;

  pdcch_group(const dci_context_information* info_, const dci_payload* payload_) : info(info_), payload(payload_) {}

  bool operator==(const pdcch_group& other) const
  {
    return std::tie(info->coreset_cfg->id, *info->bwp_cfg, info->starting_symbol) ==
           std::tie(other.info->coreset_cfg->id, *other.info->bwp_cfg, other.info->starting_symbol);
  }
  bool operator<(const pdcch_group& other) const
  {
    return std::tie(info->coreset_cfg->id, *info->bwp_cfg, info->starting_symbol) <
           std::tie(other.info->coreset_cfg->id, *other.info->bwp_cfg, other.info->starting_symbol);
  }
};

} // namespace

template <typename builder_type, typename pdu_type>
static void add_pdcch_pdus_to_builder(builder_type&                  builder,
                                      span<const pdu_type>           pdcch_info,
                                      span<const dci_payload>        payloads,
                                      const precoding_matrix_mapper& pm_mapper,
                                      unsigned                       cell_nof_prbs)
{
  srsran_assert(pdcch_info.size() == payloads.size(), "Size mismatch");

  if (pdcch_info.empty()) {
    return;
  }

  static_vector<pdcch_group, MAX_DL_PDUS_PER_SLOT> groups;
  for (unsigned i = 0, e = pdcch_info.size(); i != e; ++i) {
    groups.emplace_back(&pdcch_info[i].ctx, &payloads[i]);
  }

  // Group DCIs into FAPI PDCCH PDUs.
  std::sort(groups.begin(), groups.end());
  auto* pivot = groups.begin();
  auto* end   = groups.end();
  while (pivot != end) {
    auto* i = pivot;
    pivot   = std::lower_bound(pivot, end, *pivot, std::equal_to<>{});

    mac_pdcch_pdu pdu;
    pdu.bwp_cfg      = i->info->bwp_cfg;
    pdu.coreset_cfg  = i->info->coreset_cfg;
    pdu.start_symbol = i->info->starting_symbol;
    for (; i != pivot; ++i) {
      pdu.dcis.emplace_back(i->info, i->payload);
    }

    fapi::dl_pdcch_pdu_builder pdcch_builder = builder.add_pdcch_pdu(pdu.dcis.size());
    convert_pdcch_mac_to_fapi(pdcch_builder, pdu, pm_mapper, cell_nof_prbs);
  }
}

static void add_ssb_pdus_to_dl_request(fapi::dl_tti_request_message_builder& builder, span<const dl_ssb_pdu> ssb_pdus)
{
  for (const auto& pdu : ssb_pdus) {
    fapi::dl_ssb_pdu_builder ssb_builder = builder.add_ssb_pdu();
    convert_ssb_mac_to_fapi(ssb_builder, pdu);
  }
}

static fapi::nzp_csi_rs_epre_to_ssb to_nzp_csi_rs_epre_to_ssb(int value)
{
  switch (value) {
    case -3:
      return fapi::nzp_csi_rs_epre_to_ssb::dB_minus_3;
    case 0:
      return fapi::nzp_csi_rs_epre_to_ssb::dB0;
    case 3:
      return fapi::nzp_csi_rs_epre_to_ssb::dB3;
    case 6:
      return fapi::nzp_csi_rs_epre_to_ssb::dB6;
    default:
      return fapi::nzp_csi_rs_epre_to_ssb::dB0;
  }
}

static void add_csi_rs_pdus_to_dl_request(fapi::dl_tti_request_message_builder& builder,
                                          span<const csi_rs_info>               csi_rs_list)
{
  for (const auto& pdu : csi_rs_list) {
    bool                        is_nzp_csi  = pdu.type == csi_rs_type::CSI_RS_NZP;
    fapi::dl_csi_rs_pdu_builder csi_builder = builder.add_csi_rs_pdu(pdu.crbs.start(),
                                                                     pdu.crbs.length(),
                                                                     pdu.type,
                                                                     pdu.row,
                                                                     pdu.freq_domain,
                                                                     pdu.symbol0,
                                                                     pdu.symbol1,
                                                                     pdu.cdm_type,
                                                                     pdu.freq_density,
                                                                     (is_nzp_csi) ? pdu.scrambling_id : 0);

    csi_builder.set_bwp_parameters(pdu.bwp_cfg->scs, pdu.bwp_cfg->cp);
    if (is_nzp_csi) {
      csi_builder.set_tx_power_info_parameters(pdu.power_ctrl_offset_profile_nr,
                                               to_nzp_csi_rs_epre_to_ssb(pdu.power_ctrl_offset_ss_profile_nr));
    } else {
      // ZP-CSI type does not use these values.
      csi_builder.set_tx_power_info_parameters(0, fapi::nzp_csi_rs_epre_to_ssb::dB0);
    }
    csi_builder.set_maintenance_v3_tx_power_info_parameters({});
  }
}

static void add_pdsch_pdus_to_dl_request(fapi::dl_tti_request_message_builder& builder,
                                         span<const sib_information>           sibs,
                                         span<const rar_information>           rars,
                                         span<const dl_msg_alloc>              ue_grants,
                                         span<const dl_paging_allocation>      paging,
                                         unsigned                              nof_csi_pdus,
                                         const precoding_matrix_mapper&        pm_mapper,
                                         unsigned                              cell_nof_prbs)
{
  for (const auto& pdu : sibs) {
    fapi::dl_pdsch_pdu_builder pdsch_builder = builder.add_pdsch_pdu();
    convert_pdsch_mac_to_fapi(pdsch_builder, pdu, nof_csi_pdus, pm_mapper, cell_nof_prbs);
  }

  for (const auto& pdu : rars) {
    fapi::dl_pdsch_pdu_builder pdsch_builder = builder.add_pdsch_pdu();
    convert_pdsch_mac_to_fapi(pdsch_builder, pdu, nof_csi_pdus, pm_mapper, cell_nof_prbs);
  }

  for (const auto& pdu : ue_grants) {
    fapi::dl_pdsch_pdu_builder pdsch_builder = builder.add_pdsch_pdu();
    convert_pdsch_mac_to_fapi(pdsch_builder, pdu, nof_csi_pdus, pm_mapper, cell_nof_prbs);
  }

  for (const auto& pdu : paging) {
    fapi::dl_pdsch_pdu_builder pdsch_builder = builder.add_pdsch_pdu();
    convert_pdsch_mac_to_fapi(pdsch_builder, pdu, nof_csi_pdus, pm_mapper, cell_nof_prbs);
  }
}

/// Clears the PDUs in the given DL_TTI.request message.
static void clear_dl_tti_pdus(fapi::dl_tti_request_message& msg)
{
  msg.pdus.clear();
  msg.num_pdus_of_each_type = {};
}

void mac_to_fapi_translator::on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res)
{
  fapi::dl_tti_request_message         msg;
  fapi::dl_tti_request_message_builder builder(msg);

  // This FAPI implementation does not support PDU groups.
  const unsigned num_pdu_groups = 0;
  builder.set_basic_parameters(dl_res.slot.sfn(), dl_res.slot.slot_index(), num_pdu_groups);

  // Add PDCCH PDUs to the DL_TTI.request message.
  add_pdcch_pdus_to_builder(builder,
                            span<const pdcch_dl_information>(dl_res.dl_res->dl_pdcchs),
                            dl_res.dl_pdcch_pdus,
                            *pm_mapper,
                            cell_nof_prbs);

  // Add SSB PDUs to the DL_TTI.request message.
  add_ssb_pdus_to_dl_request(builder, dl_res.ssb_pdus);

  // Add CSI-RS PDUs to the DL_TTI.request message.
  add_csi_rs_pdus_to_dl_request(builder, dl_res.dl_res->csi_rs);

  // Add PDSCH PDUs to the DL_TTI.request message.
  add_pdsch_pdus_to_dl_request(builder,
                               dl_res.dl_res->bc.sibs,
                               dl_res.dl_res->rar_grants,
                               dl_res.dl_res->ue_grants,
                               dl_res.dl_res->paging_grants,
                               dl_res.dl_res->csi_rs.size(),
                               *pm_mapper,
                               cell_nof_prbs);

  bool is_pdsch_pdu_present_in_dl_tti = msg.num_pdus_of_each_type[static_cast<size_t>(fapi::dl_pdu_type::PDSCH)] != 0;
  bool is_ul_dci_present              = dl_res.dl_res->ul_pdcchs.size() != 0;

  if (!is_pdsch_pdu_present_in_dl_tti && !is_ul_dci_present) {
    builder.set_last_message_in_slot_flag();
  }

  // Validate the DL_TTI.request message.
  error_type<fapi::validator_report> result = validate_dl_tti_request(msg);

  if (!result) {
    log_validator_report(result.error(), logger);

    clear_dl_tti_pdus(msg);
  }

  // Send the message.
  msg_gw.dl_tti_request(msg);

  bool is_ul_dci_last_message_in_slot = !is_pdsch_pdu_present_in_dl_tti && is_ul_dci_present;

  handle_ul_dci_request(dl_res.dl_res->ul_pdcchs, dl_res.ul_pdcch_pdus, dl_res.slot, is_ul_dci_last_message_in_slot);
}

void mac_to_fapi_translator::on_new_downlink_data(const mac_dl_data_result& dl_data)
{
  srsran_assert(!dl_data.si_pdus.empty() || !dl_data.rar_pdus.empty() || !dl_data.ue_pdus.empty() ||
                    !dl_data.paging_pdus.empty(),
                "Received a mac_dl_data_result object with zero payloads");

  fapi::tx_data_request_message msg;
  fapi::tx_data_request_builder builder(msg);

  builder.set_basic_parameters(dl_data.slot.sfn(), dl_data.slot.slot_index());

  // Make sure PDUs are added to the builder in the same order as for the DL_TTI.request message.
  unsigned fapi_index = 0;

  // Add SIB1 PDUs to the Tx_Data.request message.
  for (const auto& pdu : dl_data.si_pdus) {
    builder.add_pdu_custom_payload(fapi_index, pdu.cw_index, {pdu.pdu.data(), pdu.pdu.size()});
    if (pdu.cw_index == 0) {
      ++fapi_index;
    }
  }

  // Add RAR PDUs to the Tx_Data.request message.
  for (const auto& pdu : dl_data.rar_pdus) {
    builder.add_pdu_custom_payload(fapi_index, pdu.cw_index, {pdu.pdu.data(), pdu.pdu.size()});
    if (pdu.cw_index == 0) {
      ++fapi_index;
    }
  }

  // Add UE specific PDUs to the Tx_Data.request message.
  for (const auto& pdu : dl_data.ue_pdus) {
    builder.add_pdu_custom_payload(fapi_index, pdu.cw_index, {pdu.pdu.data(), pdu.pdu.size()});
    if (pdu.cw_index == 0) {
      ++fapi_index;
    }
  }

  // Add Paging PDU to the Tx_Data.request message.
  for (const auto& pdu : dl_data.paging_pdus) {
    builder.add_pdu_custom_payload(fapi_index, pdu.cw_index, {pdu.pdu.data(), pdu.pdu.size()});
    if (pdu.cw_index == 0) {
      ++fapi_index;
    }
  }

  // Send the message.
  msg_gw.tx_data_request(msg);
}

/// Clears the PDUs of the given UL_TTI.request message.
static void clear_ul_tti_pdus(fapi::ul_tti_request_message& msg)
{
  msg.pdus.clear();
  msg.num_pdus_of_each_type = {};
}

void mac_to_fapi_translator::on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res)
{
  fapi::ul_tti_request_message         msg;
  fapi::ul_tti_request_message_builder builder(msg);

  // :TODO: Should we check here the numerology matches incoming slot and configured numerology for cell?
  builder.set_basic_parameters(ul_res.slot.sfn(), ul_res.slot.slot_index());

  // Add PRACH PDUs to the UL_TTI.request message.
  for (const auto& pdu : ul_res.ul_res->prachs) {
    fapi::ul_prach_pdu_builder pdu_builder = builder.add_prach_pdu();
    convert_prach_mac_to_fapi(pdu_builder, pdu);
  }

  // Add PUSCH PDUs to the UL_TTI.request message.
  for (const auto& pdu : ul_res.ul_res->puschs) {
    fapi::ul_pusch_pdu_builder pdu_builder = builder.add_pusch_pdu();
    convert_pusch_mac_to_fapi(pdu_builder, pdu, *part2_mapper);
  }

  for (const auto& pdu : ul_res.ul_res->pucchs) {
    fapi::ul_pucch_pdu_builder pdu_builder = builder.add_pucch_pdu(pdu.format);
    convert_pucch_mac_to_fapi(pdu_builder, pdu);
  }

  // Validate the UL_TTI.request message.
  error_type<fapi::validator_report> result = validate_ul_tti_request(msg);

  if (!result) {
    log_validator_report(result.error(), logger);

    clear_ul_tti_pdus(msg);
  }

  // Send the message.
  msg_gw.ul_tti_request(msg);
}

void mac_to_fapi_translator::handle_ul_dci_request(span<const pdcch_ul_information> pdcch_info,
                                                   span<const dci_payload>          payloads,
                                                   slot_point                       slot,
                                                   bool                             is_last_message_in_slot)
{
  // This message is optional, do not send it empty.
  if (pdcch_info.empty()) {
    return;
  }

  fapi::ul_dci_request_message         msg;
  fapi::ul_dci_request_message_builder builder(msg);

  builder.set_basic_parameters(slot.sfn(), slot.slot_index());
  add_pdcch_pdus_to_builder(builder, pdcch_info, payloads, *pm_mapper, cell_nof_prbs);

  if (is_last_message_in_slot) {
    builder.set_last_message_in_slot_flag();
  }

  // Validate the UL_DCI.request message.
  error_type<fapi::validator_report> result = validate_ul_dci_request(msg);
  if (!result) {
    log_validator_report(result.error(), logger);

    return;
  }

  // Send the message.
  msg_gw.ul_dci_request(msg);
}

void mac_to_fapi_translator::on_cell_results_completion(slot_point slot)
{
  last_msg_notifier.on_last_message(slot);
}

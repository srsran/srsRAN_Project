/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_to_fapi_translator.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/fapi/slot_message_gateway.h"
#include "srsgnb/fapi_adaptor/mac/messages/pdcch.h"
#include "srsgnb/fapi_adaptor/mac/messages/pdsch.h"
#include "srsgnb/fapi_adaptor/mac/messages/prach.h"
#include "srsgnb/fapi_adaptor/mac/messages/pucch.h"
#include "srsgnb/fapi_adaptor/mac/messages/pusch.h"
#include "srsgnb/fapi_adaptor/mac/messages/ssb.h"

using namespace srsgnb;
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
static void
add_pdcch_pdus_to_builder(builder_type& builder, span<const pdu_type> pdcch_info, span<const dci_payload> payloads)
{
  srsgnb_assert(pdcch_info.size() == payloads.size(), "Size mismatch");

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
    convert_pdcch_mac_to_fapi(pdcch_builder, pdu);
  }
}

static void add_ssb_pdus_to_dl_request(fapi::dl_tti_request_message_builder& builder, span<const dl_ssb_pdu> ssb_pdus)
{
  for (const auto& pdu : ssb_pdus) {
    fapi::dl_ssb_pdu_builder ssb_builder = builder.add_ssb_pdu();
    convert_ssb_mac_to_fapi(ssb_builder, pdu);
  }
}

static void add_pdsch_pdus_to_dl_request(fapi::dl_tti_request_message_builder& builder,
                                         pdsch_pdu_registry&                   pdsch_registry,
                                         span<const sib_information>           sibs,
                                         span<const rar_information>           rars,
                                         span<const dl_msg_alloc>              ue_grants)
{
  for (const auto& pdu : sibs) {
    fapi::dl_pdsch_pdu_builder pdsch_builder = builder.add_pdsch_pdu();
    convert_pdsch_mac_to_fapi(pdsch_builder, pdu);
    // Add one entry per codeword to the registry.
    for (unsigned i = 0, e = pdu.pdsch_cfg.codewords.size(); i != e; ++i) {
      pdsch_registry.register_pdu(pdsch_builder.get_pdu_id(), i, pdsch_pdu_registry::sib);
    }
  }

  for (const auto& pdu : rars) {
    fapi::dl_pdsch_pdu_builder pdsch_builder = builder.add_pdsch_pdu();
    convert_pdsch_mac_to_fapi(pdsch_builder, pdu);
    // Add one entry per codeword to the registry.
    for (unsigned i = 0, e = pdu.pdsch_cfg.codewords.size(); i != e; ++i) {
      pdsch_registry.register_pdu(pdsch_builder.get_pdu_id(), i, pdsch_pdu_registry::rar);
    }
  }

  for (const auto& pdu : ue_grants) {
    fapi::dl_pdsch_pdu_builder pdsch_builder = builder.add_pdsch_pdu();
    convert_pdsch_mac_to_fapi(pdsch_builder, pdu);
    // Add one entry per codeword to the registry.
    for (unsigned i = 0, e = pdu.pdsch_cfg.codewords.size(); i != e; ++i) {
      pdsch_registry.register_pdu(pdsch_builder.get_pdu_id(), i, pdsch_pdu_registry::ue);
    }
  }
}

/// Clears the PDUs of the given DL_TTI.request message.
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
  add_pdcch_pdus_to_builder(builder, span<const pdcch_dl_information>(dl_res.dl_res->dl_pdcchs), dl_res.dl_pdcch_pdus);

  // Add SSB PDUs to the DL_TTI.request message.
  add_ssb_pdus_to_dl_request(builder, dl_res.ssb_pdus);

  {
    std::lock_guard<std::mutex> lock(mutex);
    // Add PDSCH PDUs to the DL_TTI.request message.
    add_pdsch_pdus_to_dl_request(
        builder, pdsch_registry, dl_res.dl_res->bc.sibs, dl_res.dl_res->rar_grants, dl_res.dl_res->ue_grants);
  }

  // Validate the DL_TTI.request message.
  error_type<fapi::validator_report> result = validate_dl_tti_request(msg);

  if (!result) {
    log_validator_report(result.error());

    clear_dl_tti_pdus(msg);
  }

  // Send the message.
  msg_gw.dl_tti_request(msg);

  handle_ul_dci_request(dl_res.dl_res->ul_pdcchs, dl_res.ul_pdcch_pdus, dl_res.slot);
}

void mac_to_fapi_translator::on_new_downlink_data(const mac_dl_data_result& dl_data)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Sanity checks.
  srsgnb_assert(dl_data.sib1_pdus.size() == pdsch_registry.get_nof_pdus(pdsch_pdu_registry::sib),
                "Number of PDUs ({}) and Payloads ({}) for SIB PDUs doesn't match",
                pdsch_registry.get_nof_pdus(pdsch_pdu_registry::sib),
                dl_data.sib1_pdus.size());

  srsgnb_assert(dl_data.rar_pdus.size() == pdsch_registry.get_nof_pdus(pdsch_pdu_registry::rar),
                "Number of PDUs ({}) and Payloads ({}) for RAR PDUs doesn't match",
                pdsch_registry.get_nof_pdus(pdsch_pdu_registry::rar),
                dl_data.rar_pdus.size());

  srsgnb_assert(dl_data.ue_pdus.size() == pdsch_registry.get_nof_pdus(pdsch_pdu_registry::ue),
                "Number of PDUs ({}) and Payloads ({}) for SIB PDUs doesn't match",
                pdsch_registry.get_nof_pdus(pdsch_pdu_registry::ue),
                dl_data.ue_pdus.size());
  srsgnb_assert(dl_data.sib1_pdus.size() || dl_data.rar_pdus.size() || dl_data.ue_pdus.size(),
                "Error, received a Tx_Data.request with zero payloads");

  fapi::tx_data_request_message msg;
  fapi::tx_data_request_builder builder(msg);

  builder.set_basic_parameters(dl_data.slot.sfn(), dl_data.slot.slot_index());

  // Add SIB1 PDUs to the Tx_Data.request message.
  const static_vector<span<const uint8_t>, MAX_SIB1_PDUS_PER_SLOT>& sib1_pdus = dl_data.sib1_pdus;
  for (unsigned i = 0, e = sib1_pdus.size(); i != e; ++i) {
    const pdsch_pdu_registry::pdu_struct& registry_pdu = pdsch_registry.get_fapi_pdu_index(i, pdsch_pdu_registry::sib);
    builder.add_pdu_custom_payload(
        registry_pdu.fapi_index, registry_pdu.cw_index, {sib1_pdus[i].data(), sib1_pdus[i].size()});
  }

  // Add RAR PDUs to the Tx_Data.request message.
  const static_vector<span<const uint8_t>, MAX_RAR_PDUS_PER_SLOT>& rar_pdus = dl_data.rar_pdus;
  for (unsigned i = 0, e = rar_pdus.size(); i != e; ++i) {
    const pdsch_pdu_registry::pdu_struct& registry_pdu = pdsch_registry.get_fapi_pdu_index(i, pdsch_pdu_registry::rar);
    builder.add_pdu_custom_payload(
        registry_pdu.fapi_index, registry_pdu.cw_index, {rar_pdus[i].data(), rar_pdus[i].size()});
  }

  // Add UE specific PDUs.
  const static_vector<span<const uint8_t>, MAX_UE_PDUS_PER_SLOT>& ue_pdus = dl_data.ue_pdus;
  for (unsigned i = 0, e = ue_pdus.size(); i != e; ++i) {
    const pdsch_pdu_registry::pdu_struct& registry_pdu = pdsch_registry.get_fapi_pdu_index(i, pdsch_pdu_registry::ue);
    builder.add_pdu_custom_payload(
        registry_pdu.fapi_index, registry_pdu.cw_index, {ue_pdus[i].data(), ue_pdus[i].size()});
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
    convert_pusch_mac_to_fapi(pdu_builder, pdu.pusch_cfg);
  }

  for (const auto& pdu : ul_res.ul_res->pucchs) {
    fapi::ul_pucch_pdu_builder pdu_builder = builder.add_pucch_pdu(pdu.format);
    convert_pucch_mac_to_fapi(pdu_builder, pdu);
  }

  // Validate the UL_TTI.request message.
  error_type<fapi::validator_report> result = validate_ul_tti_request(msg);

  if (!result) {
    log_validator_report(result.error());

    clear_ul_tti_pdus(msg);
  }

  // Send the message.
  msg_gw.ul_tti_request(msg);
}

void mac_to_fapi_translator::handle_ul_dci_request(span<const pdcch_ul_information> pdcch_info,
                                                   span<const dci_payload>          payloads,
                                                   slot_point                       slot)
{
  // This message is optional, do not send it empty.
  if (pdcch_info.empty()) {
    return;
  }

  fapi::ul_dci_request_message         msg;
  fapi::ul_dci_request_message_builder builder(msg);

  builder.set_basic_parameters(slot.sfn(), slot.slot_index());
  add_pdcch_pdus_to_builder(builder, pdcch_info, payloads);

  // Validate the UL_DCI.request message.
  error_type<fapi::validator_report> result = validate_ul_dci_request(msg);
  if (!result) {
    log_validator_report(result.error());

    return;
  }

  // Send the message.
  msg_gw.ul_dci_request(msg);
}

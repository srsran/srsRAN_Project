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

#include "fapi_to_phy_translator.h"
#include "srsran/fapi/message_builders.h"
#include "srsran/fapi_adaptor/phy/messages/csi_rs.h"
#include "srsran/fapi_adaptor/phy/messages/pdcch.h"
#include "srsran/fapi_adaptor/phy/messages/pdsch.h"
#include "srsran/fapi_adaptor/phy/messages/prach.h"
#include "srsran/fapi_adaptor/phy/messages/pucch.h"
#include "srsran/fapi_adaptor/phy/messages/pusch.h"
#include "srsran/fapi_adaptor/phy/messages/ssb.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/support/resource_grid_pool.h"
#include "srsran/phy/upper/downlink_processor.h"
#include "srsran/phy/upper/unique_tx_buffer.h"
#include "srsran/phy/upper/uplink_request_processor.h"
#include "srsran/phy/upper/uplink_slot_pdu_repository.h"

using namespace srsran;
using namespace fapi_adaptor;

namespace {

class downlink_processor_dummy : public downlink_processor
{
public:
  void process_pdcch(const pdcch_processor::pdu_t& pdu) override
  {
    srslog::fetch_basic_logger("FAPI").warning("Could not enqueue PDCCH PDU in the downlink processor");
  }
  void process_pdsch(unique_tx_buffer                                                                     rm_buffer,
                     const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
                     const pdsch_processor::pdu_t&                                                        pdu) override
  {
    srslog::fetch_basic_logger("FAPI").warning("Could not enqueue PDSCH PDU in the downlink processor");
  }
  void process_ssb(const ssb_processor::pdu_t& pdu) override
  {
    srslog::fetch_basic_logger("FAPI").warning("Could not enqueue SSB PDU in the downlink processor");
  }
  void process_nzp_csi_rs(const nzp_csi_rs_generator::config_t& config) override
  {
    srslog::fetch_basic_logger("FAPI").warning("Could not enqueue NZP-CSI-RS PDU in the downlink processor");
  }
  bool configure_resource_grid(const resource_grid_context& context, resource_grid& grid) override { return true; }
  void finish_processing_pdus() override {}
};

class slot_error_notifier_dummy : public fapi::slot_error_message_notifier
{
public:
  void on_error_indication(const fapi::error_indication_message& msg) override {}
};

} // namespace

/// This dummy object is passed to the constructor of the FAPI-to-PHY translator as a placeholder for the actual
/// downlink processor, which will be later set up using the downlink processor pool.
static downlink_processor_dummy dummy_dl_processor;

/// This dummy object is passed to the constructor of the FAPI-to-PHY translator as a placeholder for the actual error
/// notifier.
static slot_error_notifier_dummy dummy_error_notifier;

fapi_to_phy_translator::fapi_to_phy_translator(const fapi_to_phy_translator_config&  config,
                                               fapi_to_phy_translator_dependencies&& dependencies) :
  sector_id(config.sector_id),
  nof_slots_request_headroom(config.nof_slots_request_headroom),
  logger(*dependencies.logger),
  dl_pdu_validator(*dependencies.dl_pdu_validator),
  buffer_pool(*dependencies.buffer_pool),
  ul_request_processor(*dependencies.ul_request_processor),
  ul_rg_pool(*dependencies.ul_rg_pool),
  ul_pdu_validator(*dependencies.ul_pdu_validator),
  ul_pdu_repository(*dependencies.ul_pdu_repository),
  slot_controller_mngr(*dependencies.dl_processor_pool,
                       *dependencies.dl_rg_pool,
                       sector_id,
                       config.nof_slots_request_headroom),
  pm_repo(std::move(dependencies.pm_repo)),
  part2_repo(std::move(dependencies.part2_repo)),
  error_notifier(dummy_error_notifier),
  scs(config.scs),
  scs_common(config.scs_common),
  prach_cfg(*config.prach_cfg),
  carrier_cfg(*config.carrier_cfg),
  prach_ports(config.prach_ports.begin(), config.prach_ports.end())
{
  srsran_assert(pm_repo, "Invalid precoding matrix repository");
  srsran_assert(part2_repo, "Invalid UCI Part2 repository");
  srsran_assert(!prach_ports.empty(), "The PRACH ports must not be empty.");
}

fapi_to_phy_translator::slot_based_upper_phy_controller::slot_based_upper_phy_controller() :
  dl_processor(dummy_dl_processor)
{
}

fapi_to_phy_translator::slot_based_upper_phy_controller::slot_based_upper_phy_controller(
    downlink_processor_pool& dl_processor_pool,
    resource_grid_pool&      rg_pool,
    slot_point               slot_,
    unsigned                 sector_id) :
  slot(slot_), dl_processor(dl_processor_pool.get_processor(slot_, 0))
{
  resource_grid_context context = {slot_, sector_id};
  // Grab the resource grid.
  // FIXME: 0 is hardcoded as the sector as in this implementation there is one DU per sector, so each DU have its own
  // resource grid pool and downlink processor pool. It is also in the previous get processor call of the downlink
  // processor pool
  resource_grid& grid = rg_pool.get_resource_grid({slot_, 0});

  // Configure the downlink processor.
  bool success = dl_processor.get().configure_resource_grid(context, grid);

  // Swap the DL processor with a dummy if it failed to configure the resource grid.
  if (!success) {
    dl_processor = dummy_dl_processor;
  }
}

fapi_to_phy_translator::slot_based_upper_phy_controller&
fapi_to_phy_translator::slot_based_upper_phy_controller::operator=(
    fapi_to_phy_translator::slot_based_upper_phy_controller&& other)
{
  using std::swap;
  swap(slot, other.slot);
  swap(dl_processor, other.dl_processor);
  return *this;
}

fapi_to_phy_translator::slot_based_upper_phy_controller::~slot_based_upper_phy_controller()
{
  dl_processor.get().finish_processing_pdus();
}

namespace {

/// Helper struct to store the downlink channel PHY PDUs.
struct downlink_pdus {
  static_vector<pdcch_processor::pdu_t, MAX_DL_PDCCH_PDUS_PER_SLOT>       pdcch;
  static_vector<pdsch_processor::pdu_t, MAX_PDSCH_PDUS_PER_SLOT>          pdsch;
  static_vector<ssb_processor::pdu_t, MAX_SSB_PER_SLOT>                   ssb;
  static_vector<nzp_csi_rs_generator::config_t, MAX_CSI_RS_PDUS_PER_SLOT> csi_rs;
};

/// Helper struct to store the uplink channel PHY PDUs.
struct uplink_pdus {
  static_vector<uplink_processor::pucch_pdu, MAX_PUCCH_PDUS_PER_SLOT> pucch;
  static_vector<uplink_processor::pusch_pdu, MAX_PUSCH_PDUS_PER_SLOT> pusch;
  static_vector<prach_buffer_context, MAX_PRACH_OCCASIONS_PER_SLOT>   prach;
};

} // namespace

/// Gets a RE pattern from the CSI-RS pattern for a given port.
static re_pattern get_re_pattern_port(const csi_rs_pattern& pattern_all_ports, unsigned i_port)
{
  return re_pattern(pattern_all_ports.rb_begin,
                    pattern_all_ports.rb_end,
                    pattern_all_ports.rb_stride,
                    pattern_all_ports.prb_patterns[i_port].re_mask,
                    pattern_all_ports.prb_patterns[i_port].symbol_mask);
}

/// \brief Returns a list of the RE patterns that carry CSI-RS for the given DL_TTI.request.
/// Each element of the list refers to a CSI-RS PDU with the same index.
static static_vector<re_pattern_list, MAX_CSI_RS_PDUS_PER_SLOT>
generate_csi_re_pattern_list(const fapi::dl_tti_request_message& msg, uint16_t cell_bandwidth_prb)
{
  static_vector<re_pattern_list, MAX_CSI_RS_PDUS_PER_SLOT> re_pattern_lst;

  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case fapi::dl_pdu_type::CSI_RS: {
        csi_rs_pattern pattern;
        get_csi_rs_pattern_from_fapi_pdu(pattern, pdu.csi_rs_pdu, cell_bandwidth_prb);

        auto& re_pat = re_pattern_lst.emplace_back();
        for (unsigned port = 0, nof_ports = pattern.prb_patterns.size(); port != nof_ports; ++port) {
          re_pat.merge(get_re_pattern_port(pattern, port));
        }
        break;
      }
      default:
        break;
    }
  }

  return re_pattern_lst;
}

/// \brief Translates, validates and returns the FAPI PDUs to PHY PDUs.
/// \note If a PDU fails the validation, the whole DL_TTI.request message is dropped.
static expected<downlink_pdus> translate_dl_tti_pdus_to_phy_pdus(const fapi::dl_tti_request_message& msg,
                                                                 const downlink_pdu_validator&       dl_pdu_validator,
                                                                 srslog::basic_logger&               logger,
                                                                 subcarrier_spacing                  scs_common,
                                                                 uint16_t                            cell_bandwidth_prb,
                                                                 const precoding_matrix_repository&  pm_repo)
{
  downlink_pdus pdus;
  const auto&   csi_re_patterns = generate_csi_re_pattern_list(msg, cell_bandwidth_prb);

  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case fapi::dl_pdu_type::CSI_RS: {
        if (pdu.csi_rs_pdu.type != csi_rs_type::CSI_RS_NZP && pdu.csi_rs_pdu.type != csi_rs_type::CSI_RS_ZP) {
          logger.warning("Only NZP-CSI-RS and ZP-CSI-RS PDU types are supported. Skipping DL_TTI.request");

          return {default_error_t{}};
        }
        // ZP-CSI does not need any further work to do.
        if (pdu.csi_rs_pdu.type == csi_rs_type::CSI_RS_ZP) {
          break;
        }
        nzp_csi_rs_generator::config_t& csi_pdu = pdus.csi_rs.emplace_back();
        convert_csi_rs_fapi_to_phy(csi_pdu, pdu.csi_rs_pdu, msg.sfn, msg.slot, cell_bandwidth_prb);
        if (!dl_pdu_validator.is_valid(csi_pdu)) {
          logger.warning("Upper PHY flagged a CSI-RS PDU as having an invalid configuration. Skipping DL_TTI.request");

          return {default_error_t{}};
        }
        break;
      }
      case fapi::dl_pdu_type::PDCCH: {
        // For each DCI in the PDCCH PDU, create a pdcch_processor::pdu_t.
        for (unsigned i_dci = 0, i_dci_end = pdu.pdcch_pdu.dl_dci.size(); i_dci != i_dci_end; ++i_dci) {
          pdcch_processor::pdu_t& pdcch_pdu = pdus.pdcch.emplace_back();
          convert_pdcch_fapi_to_phy(pdcch_pdu, pdu.pdcch_pdu, msg.sfn, msg.slot, i_dci, pm_repo);
          if (!dl_pdu_validator.is_valid(pdcch_pdu)) {
            logger.warning("Upper PHY flagged a DL DCI PDU with index '{}' as having an invalid configuration. "
                           "Skipping DL_TTI.request",
                           i_dci);

            return {default_error_t{}};
          }
        }
        break;
      }
      case fapi::dl_pdu_type::PDSCH: {
        pdsch_processor::pdu_t& pdsch_pdu = pdus.pdsch.emplace_back();
        convert_pdsch_fapi_to_phy(pdsch_pdu, pdu.pdsch_pdu, msg.sfn, msg.slot, csi_re_patterns, pm_repo);
        if (!dl_pdu_validator.is_valid(pdsch_pdu)) {
          logger.warning("Upper PHY flagged a PDSCH PDU as having an invalid configuration. Skipping DL_TTI.request");

          return {default_error_t{}};
        }
        break;
      }
      case fapi::dl_pdu_type::SSB: {
        ssb_processor::pdu_t& ssb_pdu = pdus.ssb.emplace_back();
        convert_ssb_fapi_to_phy(ssb_pdu, pdu.ssb_pdu, msg.sfn, msg.slot, scs_common);
        if (!dl_pdu_validator.is_valid(ssb_pdu)) {
          logger.warning("Upper PHY flagged a SSB PDU as having an invalid configuration. Skipping DL_TTI.request");

          return {default_error_t{}};
        }
        break;
      }
      default:
        srsran_assert(0, "DL_TTI.request PDU type value '{}' not recognized.", static_cast<unsigned>(pdu.pdu_type));
    }
  }

  return pdus;
}

void fapi_to_phy_translator::dl_tti_request(const fapi::dl_tti_request_message& msg)
{
  // :TODO: check the current slot matches the DL_TTI.request slot. Do this in a different class.
  // :TODO: check the messages order. Do this in a different class.
  slot_point  slot(scs, msg.sfn, msg.slot);
  slot_point  current_slot = get_current_slot();
  trace_point tp           = l1_tracer.now();

  if (!pdsch_repository.empty()) {
    logger.warning(
        "Could not process '{}' PDSCH PDUs from the slot '{}'", pdsch_repository.pdus.size(), pdsch_repository.slot);
  }

  // Reset the repository.
  pdsch_repository.reset(slot);

  // Release the controller of the previous slot in case that it has not been released before. In case that it already
  // is released, this call will do nothing.
  slot_controller_mngr.release_controller(slot - 1);

  // Ignore messages that do not correspond to the current slot.
  if (!is_message_in_time(msg)) {
    logger.warning("Real-time failure in FAPI: Received late DL_TTI.request from slot {}.{}", msg.sfn, msg.slot);
    // Raise out of sync error.
    error_notifier.get().on_error_indication(fapi::build_out_of_sync_error_indication(
        msg.sfn, msg.slot, fapi::message_type_id::dl_tti_request, current_slot.sfn(), current_slot.slot_index()));
    l2_tracer << instant_trace_event{"dl_tti_req_late", instant_trace_event::cpu_scope::global};
    return;
  }

  // Create controller for the current slot.
  slot_based_upper_phy_controller& controller = slot_controller_mngr.acquire_controller(slot);

  // Translate the downlink PDUs.
  expected<downlink_pdus> pdus = translate_dl_tti_pdus_to_phy_pdus(
      msg, dl_pdu_validator, logger, scs_common, carrier_cfg.dl_grid_size[to_numerology_value(scs_common)], *pm_repo);

  // Raise invalid format error.
  if (!pdus.has_value()) {
    error_notifier.get().on_error_indication(
        fapi::build_msg_slot_error_indication(msg.sfn, msg.slot, fapi::message_type_id::dl_tti_request));
    return;
  }

  // Process the PDUs.
  for (const auto& ssb : pdus.value().ssb) {
    controller->process_ssb(ssb);
  }
  for (const auto& pdcch : pdus.value().pdcch) {
    controller->process_pdcch(pdcch);
  }
  for (const auto& csi : pdus.value().csi_rs) {
    controller->process_nzp_csi_rs(csi);
  }
  for (const auto& pdsch : pdus.value().pdsch) {
    pdsch_repository.pdus.push_back(pdsch);
  }

  if (msg.is_last_message_in_slot) {
    srsran_assert(pdsch_repository.empty(),
                  "The DL_TTI.request message in slot '{}' has been marked as the last message in the slot, but a "
                  "TX_Data.request message is also being expected",
                  slot);
    slot_controller_mngr.release_controller(slot);
  }
  l1_tracer << trace_event("dl_tti_request", tp);
}

/// Returns true if the given PUCCH PDU is valid, otherwise false.
static bool is_pucch_pdu_valid(const uplink_pdu_validator& ul_pdu_validator, const uplink_processor::pucch_pdu& ul_pdu)
{
  switch (ul_pdu.context.format) {
    case pucch_format::FORMAT_0:
      return ul_pdu_validator.is_valid(ul_pdu.format0);
    case pucch_format::FORMAT_1:
      return ul_pdu_validator.is_valid(ul_pdu.format1);
    case pucch_format::FORMAT_2:
      return ul_pdu_validator.is_valid(ul_pdu.format2);
    case pucch_format::FORMAT_3:
      return ul_pdu_validator.is_valid(ul_pdu.format3);
    case pucch_format::FORMAT_4:
      return ul_pdu_validator.is_valid(ul_pdu.format4);
    default:
      break;
  }

  return false;
}

/// Returns a PRACH detector slot configuration using the given PRACH buffer context.
static prach_detector::configuration get_prach_dectector_config_from(const prach_buffer_context& context)
{
  prach_detector::configuration config;
  config.root_sequence_index   = context.root_sequence_index;
  config.format                = context.format;
  config.restricted_set        = context.restricted_set;
  config.zero_correlation_zone = context.zero_correlation_zone;
  config.start_preamble_index  = context.start_preamble_index;
  config.nof_preamble_indices  = context.nof_preamble_indices;
  config.ra_scs                = to_ra_subcarrier_spacing(context.pusch_scs);
  if (config.format < prach_format_type::three) {
    config.ra_scs = prach_subcarrier_spacing::kHz1_25;
  } else if (config.format == prach_format_type::three) {
    config.ra_scs = prach_subcarrier_spacing::kHz5;
  }
  config.nof_rx_ports = context.ports.size();

  return config;
}

/// \brief Translates, validates and returns the FAPI PDUs to PHY PDUs.
/// \note If a PDU fails the validation, the whole UL_TTI.request message is dropped.
static expected<uplink_pdus> translate_ul_tti_pdus_to_phy_pdus(const fapi::ul_tti_request_message&  msg,
                                                               const uplink_pdu_validator&          ul_pdu_validator,
                                                               const fapi::prach_config&            prach_cfg,
                                                               const fapi::carrier_config&          carrier_cfg,
                                                               span<const uint8_t>                  ports,
                                                               srslog::basic_logger&                logger,
                                                               uci_part2_correspondence_repository& part2_repo,
                                                               unsigned                             sector_id)
{
  uplink_pdus pdus;
  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case fapi::ul_pdu_type::PRACH: {
        prach_buffer_context& context = pdus.prach.emplace_back();
        convert_prach_fapi_to_phy(context, pdu.prach_pdu, prach_cfg, carrier_cfg, ports, msg.sfn, msg.slot, sector_id);
        if (!ul_pdu_validator.is_valid(get_prach_dectector_config_from(context))) {
          logger.warning(
              "Upper PHY flagged a PRACH PDU as having an invalid configuration. Skipping UL_TTI.request in slot");

          return {default_error_t{}};
        }

        break;
      }
      case fapi::ul_pdu_type::PUCCH: {
        uplink_processor::pucch_pdu& ul_pdu = pdus.pucch.emplace_back();
        convert_pucch_fapi_to_phy(ul_pdu, pdu.pucch_pdu, msg.sfn, msg.slot, carrier_cfg.num_rx_ant);
        if (!is_pucch_pdu_valid(ul_pdu_validator, ul_pdu)) {
          logger.warning("Upper PHY flagged a PUCCH PDU as having an invalid configuration. Skipping UL_TTI.request");

          return {default_error_t{}};
        }

        break;
      }
      case fapi::ul_pdu_type::PUSCH: {
        uplink_processor::pusch_pdu& ul_pdu = pdus.pusch.emplace_back();
        convert_pusch_fapi_to_phy(ul_pdu, pdu.pusch_pdu, msg.sfn, msg.slot, carrier_cfg.num_rx_ant, part2_repo);
        if (!ul_pdu_validator.is_valid(ul_pdu.pdu)) {
          logger.warning("Upper PHY flagged a PUSCH PDU as having an invalid configuration. Skipping UL_TTI.request");

          return {default_error_t{}};
        }
        break;
      }
      case fapi::ul_pdu_type::SRS:
      default:
        srsran_assert(0, "UL_TTI.request PDU type value '{}' not recognized.", static_cast<unsigned>(pdu.pdu_type));
    }
  }
  return pdus;
}

void fapi_to_phy_translator::ul_tti_request(const fapi::ul_tti_request_message& msg)
{
  // :TODO: check the messages order. Do this in a different class.
  slot_point  slot(scs, msg.sfn, msg.slot);
  slot_point  current_slot = get_current_slot();
  trace_point tp           = l1_tracer.now();

  // Clear the repository for the message slot.
  ul_pdu_repository.clear_slot(slot);

  // Release the controller of the previous slot in case that it has not been released before. In case that it already
  // is released, this call will do nothing.
  slot_controller_mngr.release_controller(slot - 1);

  // Ignore messages that do not correspond to the current slot.
  if (!is_message_in_time(msg)) {
    logger.warning("Real-time failure in FAPI: Received late UL_TTI.request from slot {}.{}", msg.sfn, msg.slot);
    // Raise out of sync error.
    error_notifier.get().on_error_indication(fapi::build_out_of_sync_error_indication(
        msg.sfn, msg.slot, fapi::message_type_id::ul_tti_request, current_slot.sfn(), current_slot.slot_index()));
    l2_tracer << instant_trace_event{"ul_tti_req_late", instant_trace_event::cpu_scope::global};
    return;
  }

  expected<uplink_pdus> pdus = translate_ul_tti_pdus_to_phy_pdus(
      msg, ul_pdu_validator, prach_cfg, carrier_cfg, prach_ports, logger, *part2_repo, sector_id);

  // Raise invalid format error.
  if (!pdus.has_value()) {
    error_notifier.get().on_error_indication(
        fapi::build_msg_slot_error_indication(msg.sfn, msg.slot, fapi::message_type_id::ul_tti_request));
    return;
  }

  // Process the PRACHs
  for (const auto& context : pdus.value().prach) {
    ul_request_processor.process_prach_request(context);
  }

  if (pdus.value().pusch.empty() && pdus.value().pucch.empty()) {
    return;
  }

  // Add the PUCCH and PUSCH PDUs to the repository for later processing.
  for (const auto& pdu : pdus.value().pusch) {
    ul_pdu_repository.add_pusch_pdu(slot, pdu);
  }
  for (const auto& pdu : pdus.value().pucch) {
    ul_pdu_repository.add_pucch_pdu(slot, pdu);
  }

  // Notify to capture uplink slot.
  resource_grid_context rg_context;
  rg_context.slot   = slot;
  rg_context.sector = sector_id;

  // Get ul_resource_grid.
  resource_grid_context pool_context = rg_context;
  pool_context.sector                = 0;
  resource_grid& ul_rg               = ul_rg_pool.get_resource_grid(pool_context);
  // Request to capture uplink slot.
  ul_request_processor.process_uplink_slot_request(rg_context, ul_rg);
  l1_tracer << trace_event("ul_tti_request", tp);
}

void fapi_to_phy_translator::ul_dci_request(const fapi::ul_dci_request_message& msg)
{
  slot_point  current_slot = get_current_slot();
  trace_point tp           = l1_tracer.now();

  // Ignore messages that do not correspond to the current slot.
  if (!is_message_in_time(msg)) {
    logger.warning("Real-time failure in FAPI: Received UL_DCI.request message from slot {}.{}", msg.sfn, msg.slot);
    // Raise invalid sfn error.
    error_notifier.get().on_error_indication(fapi::build_invalid_sfn_error_indication(
        msg.sfn, msg.slot, fapi::message_type_id::ul_dci_request, current_slot.sfn(), current_slot.slot_index()));
    l2_tracer << instant_trace_event{"ul_dci_req_late", instant_trace_event::cpu_scope::global};
    return;
  }

  static_vector<pdcch_processor::pdu_t, MAX_PUCCH_PDUS_PER_SLOT> pdus;
  for (const auto& pdu : msg.pdus) {
    // For each DCI in the PDCCH PDU, create a pdcch_processor::pdu_t.
    for (unsigned i_dci = 0, i_dci_end = pdu.pdu.dl_dci.size(); i_dci != i_dci_end; ++i_dci) {
      pdcch_processor::pdu_t& pdcch_pdu = pdus.emplace_back();
      convert_pdcch_fapi_to_phy(pdcch_pdu, pdu.pdu, msg.sfn, msg.slot, i_dci, *pm_repo);
      if (!dl_pdu_validator.is_valid(pdcch_pdu)) {
        logger.warning("Upper PHY flagged a UL DCI PDU with index '{}' as having an invalid configuration. Skipping "
                       "UL_DCI.request",
                       i_dci);
        // Raise invalid format error.
        error_notifier.get().on_error_indication(fapi::build_msg_ul_dci_error_indication(msg.sfn, msg.slot));
        return;
      }
    }
  }

  slot_point                       slot(scs, msg.sfn, msg.slot);
  slot_based_upper_phy_controller& controller = slot_controller_mngr.get_controller(slot);
  for (const auto& pdcch_pdu : pdus) {
    controller->process_pdcch(pdcch_pdu);
  }

  // No more data to process, controller can be removed.
  if (msg.is_last_message_in_slot) {
    srsran_assert(pdsch_repository.empty(),
                  "The UL_DCI.request message in slot '{}' has been marked as the last message in the slot, but a "
                  "TX_Data.request message is also being expected",
                  slot);
    slot_controller_mngr.release_controller(slot);
  }
  l1_tracer << trace_event("ul_dci_request", tp);
}

void fapi_to_phy_translator::tx_data_request(const fapi::tx_data_request_message& msg)
{
  slot_point  current_slot = get_current_slot();
  trace_point tp           = l1_tracer.now();

  // Ignore messages that do not correspond to the current slot.
  if (!is_message_in_time(msg)) {
    logger.warning("Real-time failure in FAPI: Received TX_Data.request from slot {}.{}", msg.sfn, msg.slot);
    // Raise invalid sfn error.
    error_notifier.get().on_error_indication(fapi::build_invalid_sfn_error_indication(
        msg.sfn, msg.slot, fapi::message_type_id::tx_data_request, current_slot.sfn(), current_slot.slot_index()));
    l2_tracer << instant_trace_event{"tx_data_req_late", instant_trace_event::cpu_scope::global};

    pdsch_repository.clear();

    return;
  }

  if (msg.pdus.size() != pdsch_repository.pdus.size()) {
    logger.warning("Invalid TX_Data.request. Message contains '{}' payload PDUs but expected '{}'",
                   msg.pdus.size(),
                   pdsch_repository.pdus.size());
    // Raise invalid format error.
    error_notifier.get().on_error_indication(fapi::build_msg_tx_error_indication(msg.sfn, msg.slot));

    pdsch_repository.clear();

    return;
  }

  // Check that the slot of the TX_Data.request matches the slot of the PDSCH PDUs stored.
  slot_point slot(scs, msg.sfn, msg.slot);
  if (slot != pdsch_repository.slot) {
    logger.warning("Received a TX_Data.request message for slot '{}' that does not match slot '{}' of the previous "
                   "DL_TTI.request message",
                   slot,
                   pdsch_repository.slot);

    pdsch_repository.clear();

    return;
  }

  // Skip message if there are no PDSCH PDUs inside the repository. This may be caused by an unsupported PDU in the
  // DL_TTI.request.
  if (pdsch_repository.empty()) {
    // Raise invalid format error.
    error_notifier.get().on_error_indication(fapi::build_msg_tx_error_indication(msg.sfn, msg.slot));
    return;
  }

  slot_based_upper_phy_controller& controller = slot_controller_mngr.get_controller(slot);
  for (unsigned i = 0, e = msg.pdus.size(); i != e; ++i) {
    // Get transport block data.
    static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> data;
    const fapi::tx_data_req_pdu&                                                  pdu = msg.pdus[i];
    data.emplace_back(pdu.tlv_custom.payload, pdu.tlv_custom.length.value());

    // Get PDSCH transmission configuration.
    const pdsch_processor::pdu_t& pdsch_config = pdsch_repository.pdus[i];

    // Calculate number of codeblocks.
    unsigned nof_cb = ldpc::compute_nof_codeblocks(pdu.tlv_custom.length.to_bits(), pdsch_config.ldpc_base_graph);

    // Prepare buffer identifier.
    trx_buffer_identifier id(pdsch_config.rnti,
                             (pdsch_config.context.has_value()) ? pdsch_config.context->get_h_id() : 0);

    // Get transmit buffer.
    unique_tx_buffer buffer = (pdsch_config.context.has_value()) ? buffer_pool.reserve(pdsch_config.slot, id, nof_cb)
                                                                 : buffer_pool.reserve(pdsch_config.slot, nof_cb);

    // Check the soft buffer is valid.
    if (!buffer.is_valid()) {
      logger.warning("No PDSCH buffer available for {}.", id);
      return;
    }

    // Process PDSCH.
    controller->process_pdsch(std::move(buffer), data, pdsch_repository.pdus[i]);
  }

  slot_controller_mngr.release_controller(slot);

  // All the PDSCH PDUs have been processed. Clear the repository.
  pdsch_repository.clear();

  // Run PDSCH buffer housekeeping.
  buffer_pool.run_slot(slot);

  l1_tracer << trace_event("tx_data_request", tp);
}

void fapi_to_phy_translator::handle_new_slot(slot_point slot)
{
  trace_point tp = l1_tracer.now();

  update_current_slot(slot);

  // Update the logger context.
  logger.set_context(slot.sfn(), slot.slot_index());

  l1_tracer << trace_event("handle_new_slot", tp);
}

template <typename T>
bool fapi_to_phy_translator::is_message_in_time(const T& msg) const
{
  slot_point msg_slot(scs, msg.sfn, msg.slot);
  slot_point current_slot         = get_current_slot();
  slot_point last_allowed_message = current_slot - int(nof_slots_request_headroom);

  return last_allowed_message <= msg_slot && msg_slot <= current_slot;
}

fapi_to_phy_translator::slot_based_upper_phy_controller&
fapi_to_phy_translator::slot_based_upper_phy_controller_manager::get_controller(slot_point slot)
{
  return controller(slot);
}

void fapi_to_phy_translator::slot_based_upper_phy_controller_manager::release_controller(slot_point slot)
{
  controller(slot) = slot_based_upper_phy_controller();
}

fapi_to_phy_translator::slot_based_upper_phy_controller&
fapi_to_phy_translator::slot_based_upper_phy_controller_manager::acquire_controller(slot_point slot)
{
  controller(slot) = slot_based_upper_phy_controller(dl_processor_pool, rg_pool, slot, sector_id);

  return controller(slot);
}

fapi_to_phy_translator::slot_based_upper_phy_controller_manager::slot_based_upper_phy_controller_manager(
    downlink_processor_pool& dl_processor_pool_,
    resource_grid_pool&      rg_pool_,
    unsigned                 sector_id_,
    unsigned                 nof_slots_request_headroom) :
  dl_processor_pool(dl_processor_pool_),
  rg_pool(rg_pool_),
  sector_id(sector_id_),
  // The manager should be able to manage the current slot plus the requests headroom size.
  controllers(nof_slots_request_headroom + 1U)
{
}

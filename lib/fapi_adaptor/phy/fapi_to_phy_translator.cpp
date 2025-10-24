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

#include "fapi_to_phy_translator.h"
#include "srsran/adt/expected.h"
#include "srsran/fapi/message_builders.h"
#include "srsran/fapi_adaptor/phy/messages/csi_rs.h"
#include "srsran/fapi_adaptor/phy/messages/pdcch.h"
#include "srsran/fapi_adaptor/phy/messages/pdsch.h"
#include "srsran/fapi_adaptor/phy/messages/prach.h"
#include "srsran/fapi_adaptor/phy/messages/prs.h"
#include "srsran/fapi_adaptor/phy/messages/pucch.h"
#include "srsran/fapi_adaptor/phy/messages/pusch.h"
#include "srsran/fapi_adaptor/phy/messages/srs.h"
#include "srsran/fapi_adaptor/phy/messages/ssb.h"
#include "srsran/instrumentation/traces/critical_traces.h"
#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/support/resource_grid_pool.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/phy/upper/channel_processors/prach_detector.h"
#include "srsran/phy/upper/downlink_processor.h"
#include "srsran/phy/upper/uplink_pdu_slot_repository.h"
#include "srsran/phy/upper/uplink_pdu_validator.h"
#include "srsran/phy/upper/uplink_request_processor.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace fapi_adaptor;

namespace {

class error_notifier_dummy : public fapi::error_message_notifier
{
public:
  void on_error_indication(const fapi::error_indication_message& msg) override {}
};

} // namespace

/// This dummy object is passed to the constructor of the FAPI-to-PHY translator as a placeholder for the actual error
/// notifier.
static error_notifier_dummy dummy_error_notifier;

fapi_to_phy_translator::fapi_to_phy_translator(const fapi_to_phy_translator_config&  config,
                                               fapi_to_phy_translator_dependencies&& dependencies) :
  sector_id(config.sector_id),
  nof_slots_request_headroom(config.nof_slots_request_headroom),
  allow_request_on_empty_ul_tti(config.allow_request_on_empty_ul_tti),
  logger(*dependencies.logger),
  dl_pdu_validator(*dependencies.dl_pdu_validator),
  ul_request_processor(*dependencies.ul_request_processor),
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
  prach_cfg(config.prach_cfg),
  carrier_cfg(config.carrier_cfg),
  prach_ports(config.prach_ports.begin(), config.prach_ports.end())
{
  srsran_assert(pm_repo, "Invalid precoding matrix repository");
  srsran_assert(part2_repo, "Invalid UCI Part2 repository");
  srsran_assert(!prach_ports.empty(), "The PRACH ports must not be empty.");
}

fapi_to_phy_translator::slot_based_upper_phy_controller::slot_based_upper_phy_controller(
    downlink_processor_pool& dl_processor_pool,
    resource_grid_pool&      rg_pool,
    slot_point               slot_,
    unsigned                 sector_id) :
  slot(slot_)
{
  resource_grid_context context = {slot_, sector_id};
  // Grab the resource grid.
  shared_resource_grid grid = rg_pool.allocate_resource_grid(slot_);

  // If the resource grid is not valid, all DL transmissions for this slot shall be discarded.
  if (!grid) {
    dl_processor = {};
    return;
  }

  // Obtain the downlink processor controller associated with the given slot.
  downlink_processor_controller& dl_proc_controller = dl_processor_pool.get_processor_controller(slot);

  // Configure the downlink processor.
  dl_processor = dl_proc_controller.configure_resource_grid(context, std::move(grid));

  // Swap the DL processor with a dummy if it failed to configure the resource grid.
  if (!dl_processor.is_valid()) {
    dl_processor = {};
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

namespace {

/// Helper struct to store the downlink channel PHY PDUs.
struct downlink_pdus {
  static_vector<pdcch_processor::pdu_t, MAX_DL_PDCCH_PDUS_PER_SLOT>       pdcch;
  static_vector<pdsch_processor::pdu_t, MAX_PDSCH_PDUS_PER_SLOT>          pdsch;
  static_vector<ssb_processor::pdu_t, MAX_SSB_PER_SLOT>                   ssb;
  static_vector<nzp_csi_rs_generator::config_t, MAX_CSI_RS_PDUS_PER_SLOT> csi_rs;
  static_vector<prs_generator_configuration, MAX_PRS_PDUS_PER_SLOT>       prs;
};

/// Helper struct to store the uplink channel PHY PDUs.
struct uplink_pdus {
  static_vector<uplink_pdu_slot_repository::pucch_pdu, MAX_PUCCH_PDUS_PER_SLOT> pucch;
  static_vector<uplink_pdu_slot_repository::pusch_pdu, MAX_PUSCH_PDUS_PER_SLOT> pusch;
  static_vector<prach_buffer_context, MAX_PRACH_OCCASIONS_PER_SLOT>             prach;
  static_vector<uplink_pdu_slot_repository::srs_pdu, MAX_SRS_PDUS_PER_SLOT>     srs;
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
                                                                 const precoding_matrix_repository&  pm_repo,
                                                                 unsigned                            sector_id)
{
  downlink_pdus pdus;
  const auto&   csi_re_patterns = generate_csi_re_pattern_list(msg, cell_bandwidth_prb);

  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case fapi::dl_pdu_type::CSI_RS: {
        if (pdu.csi_rs_pdu.type != csi_rs_type::CSI_RS_NZP && pdu.csi_rs_pdu.type != csi_rs_type::CSI_RS_ZP) {
          logger.warning("Sector#{}: Skipping DL_TTI.request: Only NZP-CSI-RS and ZP-CSI-RS PDU types are supported",
                         sector_id);

          return make_unexpected(default_error_t{});
        }
        // ZP-CSI does not need any further work to do.
        if (pdu.csi_rs_pdu.type == csi_rs_type::CSI_RS_ZP) {
          break;
        }
        nzp_csi_rs_generator::config_t& csi_pdu = pdus.csi_rs.emplace_back();
        convert_csi_rs_fapi_to_phy(csi_pdu, pdu.csi_rs_pdu, msg.sfn, msg.slot, cell_bandwidth_prb);
        error_type<std::string> phy_csi_validator = dl_pdu_validator.is_valid(csi_pdu);
        if (!phy_csi_validator.has_value()) {
          logger.warning("Sector#{}: Skipping DL_TTI.request: NZP-CSI-RS PDU flagged as invalid by the Upper PHY with "
                         "the following error\n    {}",
                         sector_id,
                         phy_csi_validator.error());

          return make_unexpected(default_error_t{});
        }
        break;
      }
      case fapi::dl_pdu_type::PDCCH: {
        // For each DCI in the PDCCH PDU, create a pdcch_processor::pdu_t.
        for (unsigned i_dci = 0, i_dci_end = pdu.pdcch_pdu.dl_dci.size(); i_dci != i_dci_end; ++i_dci) {
          pdcch_processor::pdu_t& pdcch_pdu = pdus.pdcch.emplace_back();
          convert_pdcch_fapi_to_phy(pdcch_pdu, pdu.pdcch_pdu, msg.sfn, msg.slot, i_dci, pm_repo);
          error_type<std::string> phy_pdsch_validator = dl_pdu_validator.is_valid(pdcch_pdu);
          if (!phy_pdsch_validator.has_value()) {
            logger.warning("Sector#{}: Skipping DL_TTI.request: DL DCI PDU with index '{}' flagged as invalid by the "
                           "Upper PHY with the following error\n    {}",
                           sector_id,
                           i_dci,
                           phy_pdsch_validator.error());

            return make_unexpected(default_error_t{});
          }
        }
        break;
      }
      case fapi::dl_pdu_type::PDSCH: {
        pdsch_processor::pdu_t& pdsch_pdu = pdus.pdsch.emplace_back();
        convert_pdsch_fapi_to_phy(pdsch_pdu, pdu.pdsch_pdu, msg.sfn, msg.slot, csi_re_patterns, pm_repo);
        error_type<std::string> phy_pdsch_validator = dl_pdu_validator.is_valid(pdsch_pdu);
        if (!phy_pdsch_validator.has_value()) {
          logger.warning("Sector#{}: Skipping DL_TTI.request: PDSCH PDU flagged as invalid by the Upper PHY with the "
                         "following error\n    {}",
                         sector_id,
                         phy_pdsch_validator.error());
          return make_unexpected(default_error_t{});
        }
        break;
      }
      case fapi::dl_pdu_type::SSB: {
        ssb_processor::pdu_t& ssb_pdu = pdus.ssb.emplace_back();
        convert_ssb_fapi_to_phy(ssb_pdu, pdu.ssb_pdu, msg.sfn, msg.slot, scs_common);
        error_type<std::string> phy_ssb_validator = dl_pdu_validator.is_valid(ssb_pdu);
        if (!phy_ssb_validator.has_value()) {
          logger.warning("Sector#{}: Skipping DL_TTI.request: SSB PDU flagged as invalid by the Upper PHY with the "
                         "following error\n    {}",
                         sector_id,
                         phy_ssb_validator.error());

          return make_unexpected(default_error_t{});
        }
        break;
      }
      case fapi::dl_pdu_type::PRS: {
        prs_generator_configuration& prs_pdu = pdus.prs.emplace_back();
        convert_prs_fapi_to_phy(prs_pdu, pdu.prs_pdu, msg.sfn, msg.slot, pm_repo);
        error_type<std::string> phy_prs_validator = dl_pdu_validator.is_valid(prs_pdu);
        if (!phy_prs_validator.has_value()) {
          logger.warning("Sector#{}: Skipping DL_TTI.request: PRS PDU flagged as invalid by the Upper PHY with the "
                         "following error\n    {}",
                         sector_id,
                         phy_prs_validator.error());

          return make_unexpected(default_error_t{});
        }
        break;
      }
      default:
        srsran_assert(0,
                      "Sector#{}: DL_TTI.request PDU type value '{}' not recognized.",
                      sector_id,
                      static_cast<unsigned>(pdu.pdu_type));
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
  trace_point tp           = l1_dl_tracer.now();

  if (!pdsch_repository.empty()) {
    logger.warning("Sector#{}: Could not process '{}' PDSCH PDUs from the slot '{}'",
                   sector_id,
                   pdsch_repository.pdus.size(),
                   pdsch_repository.slot);
  }

  // Reset the repository.
  pdsch_repository.reset(slot);

  // Release the controller of the previous slot in case that it has not been released before. In case that it already
  // is released, this call will do nothing.
  slot_controller_mngr.release_controller(slot - 1);

  // Ignore messages that do not correspond to the current slot.
  if (!is_message_in_time(msg)) {
    logger.warning("Sector#{}: Real-time failure in FAPI: Received late DL_TTI.request from slot {}.{}",
                   sector_id,
                   msg.sfn,
                   msg.slot);
    // Raise out of sync error.
    error_notifier.get().on_error_indication(fapi::build_out_of_sync_error_indication(
        msg.sfn, msg.slot, fapi::message_type_id::dl_tti_request, current_slot.sfn(), current_slot.slot_index()));
    general_critical_tracer << instant_trace_event{
        "dl_tti_req_late", instant_trace_event::cpu_scope::global, instant_trace_event::event_criticality::severe};
    return;
  }

  // Ignore FAPI request message when there are no PDUs to process.
  if (msg.pdus.empty()) {
    return;
  }

  // Get controller for the current slot.
  slot_based_upper_phy_controller& controller = slot_controller_mngr.get_controller(slot);
  // Check if the controller is valid.
  if (!controller.is_initialized()) {
    logger.warning("Sector#{}: Could not acquire downlink processor for slot {}.{}", sector_id, msg.sfn, msg.slot);
    // Raise out of sync error.
    error_notifier.get().on_error_indication(
        fapi::build_msg_error_indication(msg.sfn, msg.slot, fapi::message_type_id::dl_tti_request));

    return;
  }

  // Translate the downlink PDUs.
  expected<downlink_pdus> pdus =
      translate_dl_tti_pdus_to_phy_pdus(msg,
                                        dl_pdu_validator,
                                        logger,
                                        scs_common,
                                        carrier_cfg.dl_grid_size[to_numerology_value(scs_common)],
                                        *pm_repo,
                                        sector_id);

  // Raise invalid format error.
  if (!pdus.has_value()) {
    error_notifier.get().on_error_indication(
        fapi::build_msg_error_indication(msg.sfn, msg.slot, fapi::message_type_id::dl_tti_request));
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
  for (const auto& prs : pdus.value().prs) {
    controller->process_prs(prs);
  }
  for (const auto& pdsch : pdus.value().pdsch) {
    pdsch_repository.pdus.push_back(pdsch);
  }

  l1_dl_tracer << trace_event("dl_tti_request", tp);
}

/// Returns true if the given PUCCH PDU is valid, otherwise false.
static error_type<std::string> is_pucch_pdu_valid(const uplink_pdu_validator&                  ul_pdu_validator,
                                                  const uplink_pdu_slot_repository::pucch_pdu& ul_pdu)
{
  return std::visit([&ul_pdu_validator](const auto& config) { return ul_pdu_validator.is_valid(config); },
                    ul_pdu.config);
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
        error_type<std::string> phy_prach_validation =
            ul_pdu_validator.is_valid(get_prach_dectector_config_from(context));
        if (!phy_prach_validation.has_value()) {
          logger.warning(
              "Sector#{}: Skipping UL_TTI.request in slot: PRACH PDU flagged as invalid by the Upper PHY with the "
              "following error\n    {}",
              sector_id,
              phy_prach_validation.error());

          return make_unexpected(default_error_t{});
        }

        break;
      }
      case fapi::ul_pdu_type::PUCCH: {
        uplink_pdu_slot_repository::pucch_pdu& ul_pdu = pdus.pucch.emplace_back();
        convert_pucch_fapi_to_phy(ul_pdu, pdu.pucch_pdu, msg.sfn, msg.slot, carrier_cfg.num_rx_ant);
        error_type<std::string> phy_pucch_validation = is_pucch_pdu_valid(ul_pdu_validator, ul_pdu);
        if (!phy_pucch_validation.has_value()) {
          logger.warning("Sector#{}: Skipping UL_TTI.request: PUCCH PDU flagged as invalid by the Upper PHY with the "
                         "following error\n    {}",
                         sector_id,
                         phy_pucch_validation.error());

          return make_unexpected(default_error_t{});
        }

        break;
      }
      case fapi::ul_pdu_type::PUSCH: {
        uplink_pdu_slot_repository::pusch_pdu& ul_pdu = pdus.pusch.emplace_back();
        convert_pusch_fapi_to_phy(ul_pdu, pdu.pusch_pdu, msg.sfn, msg.slot, carrier_cfg.num_rx_ant, part2_repo);
        error_type<std::string> phy_pusch_validator = ul_pdu_validator.is_valid(ul_pdu.pdu);
        if (!phy_pusch_validator.has_value()) {
          logger.warning("Sector#{}: Skipping UL_TTI.request: PUSCH PDU flagged as invalid by the Upper PHY with the "
                         "following error\n    {}",
                         sector_id,
                         phy_pusch_validator.error());
          return make_unexpected(default_error_t{});
        }
        break;
      }
      case fapi::ul_pdu_type::SRS: {
        uplink_pdu_slot_repository::srs_pdu& ul_pdu = pdus.srs.emplace_back();
        convert_srs_fapi_to_phy(ul_pdu, pdu.srs_pdu, sector_id, carrier_cfg.num_rx_ant, msg.sfn, msg.slot);
        error_type<std::string> srs_validation = ul_pdu_validator.is_valid(ul_pdu.config);
        if (!srs_validation.has_value()) {
          logger.warning(
              "Sector#{}: Skipping UL_TTI.request: SRS PDU flagged as invalid with the following error\n    {}",
              sector_id,
              srs_validation.error());
          return make_unexpected(default_error_t{});
        }
        break;
      }
      default:
        srsran_assert(0,
                      "Sector#{}: UL_TTI.request PDU type value '{}' not recognized.",
                      sector_id,
                      static_cast<unsigned>(pdu.pdu_type));
    }
  }

  return pdus;
}

void fapi_to_phy_translator::ul_tti_request(const fapi::ul_tti_request_message& msg)
{
  // :TODO: check the messages order. Do this in a different class.
  slot_point  slot(scs, msg.sfn, msg.slot);
  slot_point  current_slot = get_current_slot();
  trace_point tp           = l1_ul_tracer.now();

  // Release the controller of the previous slot in case that it has not been released before. In case that it already
  // is released, this call will do nothing.
  slot_controller_mngr.release_controller(slot - 1);

  // Ignore messages that do not correspond to the current slot.
  if (!is_message_in_time(msg)) {
    logger.warning("Sector#{}: Real-time failure in FAPI: Received late UL_TTI.request from slot {}.{}",
                   sector_id,
                   msg.sfn,
                   msg.slot);
    // Raise out of sync error.
    error_notifier.get().on_error_indication(fapi::build_out_of_sync_error_indication(
        msg.sfn, msg.slot, fapi::message_type_id::ul_tti_request, current_slot.sfn(), current_slot.slot_index()));
    general_critical_tracer << instant_trace_event{
        "ul_tti_req_late", instant_trace_event::cpu_scope::global, instant_trace_event::event_criticality::severe};
    return;
  }

  // No PDUs to process.
  if (msg.pdus.empty() && !allow_request_on_empty_ul_tti) {
    return;
  }

  // Obtain the PDU repository for the message slot.
  unique_uplink_pdu_slot_repository ul_pdu_slot_repository = ul_pdu_repository.get_pdu_slot_repository(slot);

  // Verify UL PDU repository is valid.
  if (!ul_pdu_slot_repository.is_valid()) {
    logger.warning(
        "Sector#{}: Real-time failure in FAPI: UL processor is busy for slot {}.{}.", sector_id, msg.sfn, msg.slot);
    // Raise out of sync error.
    error_notifier.get().on_error_indication(
        fapi::build_msg_error_indication(msg.sfn, msg.slot, fapi::message_type_id::ul_tti_request));
    general_critical_tracer << instant_trace_event{
        "ul_tti_req_busy", instant_trace_event::cpu_scope::global, instant_trace_event::event_criticality::severe};
    return;
  }

  expected<uplink_pdus> pdus = translate_ul_tti_pdus_to_phy_pdus(
      msg, ul_pdu_validator, prach_cfg, carrier_cfg, prach_ports, logger, *part2_repo, sector_id);

  // Raise invalid format error.
  if (!pdus.has_value()) {
    error_notifier.get().on_error_indication(
        fapi::build_msg_error_indication(msg.sfn, msg.slot, fapi::message_type_id::ul_tti_request));
    return;
  }

  // Process the PRACHs.
  for (const auto& context : pdus.value().prach) {
    ul_request_processor.process_prach_request(context);
  }

  if (!allow_request_on_empty_ul_tti && pdus.value().pusch.empty() && pdus.value().pucch.empty() &&
      pdus.value().srs.empty()) {
    return;
  }

  // Add the PUCCH, PUSCH and SRS PDUs to the repository for later processing.
  for (const auto& pdu : pdus.value().pusch) {
    ul_pdu_slot_repository->add_pusch_pdu(pdu);
  }
  for (const auto& pdu : pdus.value().pucch) {
    ul_pdu_slot_repository->add_pucch_pdu(pdu);
  }
  for (const auto& pdu : pdus.value().srs) {
    ul_pdu_slot_repository->add_srs_pdu(pdu);
  }

  // Release repository and obtain uplink resource grid - it ensures that the uplink processing associated with the slot
  // is ready when the UL request reaches the RU.
  shared_resource_grid ul_rg = ul_pdu_slot_repository.release();

  // Prepare the capture uplink slot context.
  resource_grid_context rg_context;
  rg_context.slot   = slot;
  rg_context.sector = sector_id;

  // Abort UL processing for this slot if the resource grid is not available.
  if (SRSRAN_UNLIKELY(!ul_rg)) {
    logger.warning("Sector#{}: Failed to allocate UL resource grid for UL_TTI.request from slot {}.{}",
                   sector_id,
                   msg.sfn,
                   msg.slot);
    // Raise out of message transmit error.
    error_notifier.get().on_error_indication(
        fapi::build_msg_error_indication(msg.sfn, msg.slot, fapi::message_type_id::ul_tti_request));
    l1_ul_tracer << instant_trace_event{"ul_tti_failed_grid", instant_trace_event::cpu_scope::global};
    return;
  }

  // Request to capture uplink slot.
  ul_request_processor.process_uplink_slot_request(rg_context, ul_rg);
  l1_ul_tracer << trace_event("ul_tti_request", tp);
}

void fapi_to_phy_translator::ul_dci_request(const fapi::ul_dci_request_message& msg)
{
  slot_point  current_slot = get_current_slot();
  trace_point tp           = l1_ul_tracer.now();

  // Ignore messages that do not correspond to the current slot.
  if (!is_message_in_time(msg)) {
    logger.warning("Sector#{}: Real-time failure in FAPI: Received UL_DCI.request message from slot {}.{}",
                   sector_id,
                   msg.sfn,
                   msg.slot);
    // Raise invalid sfn error.
    error_notifier.get().on_error_indication(fapi::build_invalid_sfn_error_indication(
        msg.sfn, msg.slot, fapi::message_type_id::ul_dci_request, current_slot.sfn(), current_slot.slot_index()));
    general_critical_tracer << instant_trace_event{
        "ul_dci_req_late", instant_trace_event::cpu_scope::global, instant_trace_event::event_criticality::severe};
    return;
  }

  if (msg.pdus.empty()) {
    return;
  }

  static_vector<pdcch_processor::pdu_t, MAX_PUCCH_PDUS_PER_SLOT> pdus;
  for (const auto& pdu : msg.pdus) {
    // For each DCI in the PDCCH PDU, create a pdcch_processor::pdu_t.
    for (unsigned i_dci = 0, i_dci_end = pdu.pdu.dl_dci.size(); i_dci != i_dci_end; ++i_dci) {
      pdcch_processor::pdu_t& pdcch_pdu = pdus.emplace_back();
      convert_pdcch_fapi_to_phy(pdcch_pdu, pdu.pdu, msg.sfn, msg.slot, i_dci, *pm_repo);
      if (!dl_pdu_validator.is_valid(pdcch_pdu)) {
        logger.warning(
            "Sector#{}: Skipping UL_DCI.request: UL DCI PDU with index '{}' flagged as invalid by the Upper PHY",
            sector_id,
            i_dci);
        // Raise invalid format error.
        error_notifier.get().on_error_indication(fapi::build_msg_ul_dci_error_indication(msg.sfn, msg.slot));
        return;
      }
    }
  }

  slot_point                       slot(scs, msg.sfn, msg.slot);
  slot_based_upper_phy_controller& controller = slot_controller_mngr.get_controller(slot);
  // Check if the controller is valid.
  if (!controller.is_initialized()) {
    logger.warning("Sector#{}: Could not acquire downlink processor for slot {}.{}", sector_id, msg.sfn, msg.slot);
    // Raise out of sync error.
    error_notifier.get().on_error_indication(fapi::build_msg_ul_dci_error_indication(msg.sfn, msg.slot));

    return;
  }
  for (const auto& pdcch_pdu : pdus) {
    controller->process_pdcch(pdcch_pdu);
  }

  l1_ul_tracer << trace_event("ul_dci_request", tp);
}

void fapi_to_phy_translator::tx_data_request(const fapi::tx_data_request_message& msg)
{
  slot_point  current_slot = get_current_slot();
  trace_point tp           = l1_dl_tracer.now();

  // Ignore messages that do not correspond to the current slot.
  if (!is_message_in_time(msg)) {
    logger.warning(
        "Sector#{}: Real-time failure in FAPI: Received TX_Data.request from slot {}.{}", sector_id, msg.sfn, msg.slot);
    // Raise invalid sfn error.
    error_notifier.get().on_error_indication(fapi::build_invalid_sfn_error_indication(
        msg.sfn, msg.slot, fapi::message_type_id::tx_data_request, current_slot.sfn(), current_slot.slot_index()));
    general_critical_tracer << instant_trace_event{
        "tx_data_req_late", instant_trace_event::cpu_scope::global, instant_trace_event::event_criticality::severe};

    pdsch_repository.clear();

    return;
  }

  if (msg.pdus.size() != pdsch_repository.pdus.size()) {
    logger.warning("Sector#{}: Invalid TX_Data.request. Message contains '{}' payload PDUs but expected '{}'",
                   sector_id,
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
    logger.warning(
        "Sector#{}: Received a TX_Data.request message for slot '{}' that does not match slot '{}' of the previous "
        "DL_TTI.request message",
        sector_id,
        slot,
        pdsch_repository.slot);

    pdsch_repository.clear();

    return;
  }

  // Message with no PDUs.
  if (msg.pdus.empty()) {
    // Check the repo for PDUs
    if (!pdsch_repository.pdus.empty()) {
      logger.warning("Sector#{}: Invalid TX_Data.request. Number of PDUs does not match repository size", sector_id);

      // Raise invalid format error.
      error_notifier.get().on_error_indication(fapi::build_msg_tx_error_indication(msg.sfn, msg.slot));

      pdsch_repository.clear();
    }
    return;
  }

  // Skip message if there are no PDSCH PDUs inside the repository. This may be caused by an unsupported PDU in the
  // DL_TTI.request.
  if (pdsch_repository.empty()) {
    logger.warning("Sector#{}: Invalid TX_Data.request. Empty PDU repository", sector_id);

    // Raise invalid format error.
    error_notifier.get().on_error_indication(fapi::build_msg_tx_error_indication(msg.sfn, msg.slot));
    return;
  }

  slot_based_upper_phy_controller& controller = slot_controller_mngr.get_controller(slot);
  // Check if the controller is valid.
  if (!controller.is_initialized()) {
    logger.warning("Sector#{}: Could not acquire downlink processor for slot {}.{}", sector_id, msg.sfn, msg.slot);
    // Raise out of sync error.
    error_notifier.get().on_error_indication(fapi::build_msg_tx_error_indication(msg.sfn, msg.slot));

    return;
  }

  for (unsigned i = 0, e = msg.pdus.size(); i != e; ++i) {
    // Process PDSCH.
    controller->process_pdsch(
        static_vector<shared_transport_block, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>{msg.pdus[i].pdu},
        pdsch_repository.pdus[i]);
  }

  // All the PDSCH PDUs have been processed. Clear the repository.
  pdsch_repository.clear();

  l1_dl_tracer << trace_event("tx_data_request", tp);
}
void fapi_to_phy_translator::on_last_message(slot_point slot)
{
  slot_controller_mngr.release_controller(slot);
}

void fapi_to_phy_translator::handle_new_slot(slot_point slot)
{
  trace_point tp = l1_common_tracer.now();

  update_current_slot(slot);

  // Update the logger context.
  logger.set_context(slot.sfn(), slot.slot_index());

  l1_common_tracer << trace_event("handle_new_slot", tp);
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
  if (auto& controller_slot = controller(slot); controller_slot.is_initialized()) {
    return controller_slot;
  }

  return acquire_controller(slot);
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

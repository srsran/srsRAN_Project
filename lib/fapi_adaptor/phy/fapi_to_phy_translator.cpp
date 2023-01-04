/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "fapi_to_phy_translator.h"
#include "srsgnb/fapi_adaptor/phy/messages/pdcch.h"
#include "srsgnb/fapi_adaptor/phy/messages/pdsch.h"
#include "srsgnb/fapi_adaptor/phy/messages/prach.h"
#include "srsgnb/fapi_adaptor/phy/messages/pucch.h"
#include "srsgnb/fapi_adaptor/phy/messages/pusch.h"
#include "srsgnb/fapi_adaptor/phy/messages/ssb.h"
#include "srsgnb/phy/support/prach_buffer_context.h"
#include "srsgnb/phy/support/resource_grid_pool.h"
#include "srsgnb/phy/upper/downlink_processor.h"
#include "srsgnb/phy/upper/uplink_request_processor.h"
#include "srsgnb/phy/upper/uplink_slot_pdu_repository.h"

using namespace srsgnb;
using namespace fapi_adaptor;

namespace {

class downlink_processor_dummy : public downlink_processor
{
public:
  void process_pdcch(const pdcch_processor::pdu_t& pdu) override {}
  void process_pdsch(const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
                     const pdsch_processor::pdu_t&                                                        pdu) override
  {
  }
  void process_ssb(const ssb_processor::pdu_t& pdu) override {}
  void process_nzp_csi_rs(const nzp_csi_rs_generator::config_t& config) override {}
  void configure_resource_grid(const resource_grid_context& context, resource_grid& grid) override {}
  void finish_processing_pdus() override {}
};

} // namespace

/// This dummy object is passed to the constructor of the FAPI-to-PHY translator as a placeholder for the actual
/// downlink processor, which will be later set up using the downlink processor pool.
static downlink_processor_dummy dummy_dl_processor;

fapi_to_phy_translator::slot_based_upper_phy_controller::slot_based_upper_phy_controller() :
  dl_processor(dummy_dl_processor)
{
}

fapi_to_phy_translator::slot_based_upper_phy_controller::slot_based_upper_phy_controller(
    downlink_processor_pool& dl_processor_pool,
    resource_grid_pool&      rg_pool,
    slot_point               slot_,
    unsigned                 sector_id) :
  slot(slot_), dl_processor(dl_processor_pool.get_processor(slot_, sector_id))
{
  resource_grid_context context = {slot_, sector_id};
  // Grab the resource grid.
  resource_grid& grid = rg_pool.get_resource_grid(context);

  // Initialize the resource grid.
  grid.set_all_zero();

  // Configure the downlink processor.
  dl_processor.get().configure_resource_grid(context, grid);
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
  static_vector<pdcch_processor::pdu_t, MAX_DL_PDCCH_PDUS_PER_SLOT> pdcch;
  static_vector<pdsch_processor::pdu_t, MAX_DL_PDSCH_PDUS_PER_SLOT> pdsch;
  static_vector<ssb_processor::pdu_t, MAX_SSB_PER_SLOT>             ssb;
};

/// Helper struct to store the uplink channel PHY PDUs.
struct uplink_pdus {
  static_vector<uplink_processor::pucch_pdu, MAX_PUCCH_PDUS_PER_SLOT> pucch;
  static_vector<uplink_processor::pusch_pdu, MAX_PUSCH_PDUS_PER_SLOT> pusch;
  static_vector<prach_buffer_context, MAX_PRACH_OCCASIONS_PER_SLOT>   prach;
};

} // namespace

/// \brief Translates, validates and returns the FAPI PDUs to PHY PDUs.
/// \note If a PDU fails the validation, the whole DL_TTI.request message is dropped.
static downlink_pdus translate_dl_tti_pdus_to_phy_pdus(const fapi::dl_tti_request_message& msg,
                                                       const downlink_pdu_validator&       dl_pdu_validator,
                                                       srslog::basic_logger&               logger,
                                                       subcarrier_spacing                  scs_common)
{
  downlink_pdus pdus;
  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case fapi::dl_pdu_type::CSI_RS:
        break;
      case fapi::dl_pdu_type::PDCCH: {
        pdcch_processor::pdu_t& pdcch_pdu = pdus.pdcch.emplace_back();
        convert_pdcch_fapi_to_phy(pdcch_pdu, pdu.pdcch_pdu, msg.sfn, msg.slot);
        if (!dl_pdu_validator.is_valid(pdcch_pdu)) {
          logger.info("Not supported PDCCH PDU detected. Skipping DL_TTI.request message in {}.{}.", msg.sfn, msg.slot);

          return {};
        }
        break;
      }
      case fapi::dl_pdu_type::PDSCH: {
        pdsch_processor::pdu_t& pdsch_pdu = pdus.pdsch.emplace_back();
        convert_pdsch_fapi_to_phy(pdsch_pdu, pdu.pdsch_pdu, msg.sfn, msg.slot);
        if (!dl_pdu_validator.is_valid(pdsch_pdu)) {
          logger.info("Not supported PDSCH PDU detected. Skipping DL_TTI.request message in {}.{}.", msg.sfn, msg.slot);

          return {};
        }
        break;
      }
      case fapi::dl_pdu_type::SSB: {
        ssb_processor::pdu_t& ssb_pdu = pdus.ssb.emplace_back();
        convert_ssb_fapi_to_phy(ssb_pdu, pdu.ssb_pdu, msg.sfn, msg.slot, scs_common);
        if (!dl_pdu_validator.is_valid(ssb_pdu)) {
          logger.info("Not supported SSB PDU detected. Skipping DL_TTI.request message in {}.{}.", msg.sfn, msg.slot);

          return {};
        }
        break;
      }
      default:
        srsgnb_assert(0, "DL_TTI.request PDU type value ([]) not recognized.", static_cast<unsigned>(pdu.pdu_type));
    }
  }
  return pdus;
}

void fapi_to_phy_translator::dl_tti_request(const fapi::dl_tti_request_message& msg)
{
  // :TODO: check the current slot matches the DL_TTI.request slot. Do this in a different class.
  // :TODO: check the messages order. Do this in a different class.

  std::lock_guard<std::mutex> lock(mutex);

  const downlink_pdus& pdus = translate_dl_tti_pdus_to_phy_pdus(msg, dl_pdu_validator, logger, scs_common);

  // Process the PDUs
  for (const auto& ssb : pdus.ssb) {
    current_slot_controller->process_ssb(ssb);
  }
  for (const auto& pdcch : pdus.pdcch) {
    current_slot_controller->process_pdcch(pdcch);
  }
  for (const auto& pdsch : pdus.pdsch) {
    pdsch_pdu_repository.push_back(pdsch);
  }
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

  return config;
}

/// \brief Translates, validates and returns the FAPI PDUs to PHY PDUs.
/// \note If a PDU fails the validation, the whole UL_TTI.request message is dropped.
static uplink_pdus translate_ul_tti_pdus_to_phy_pdus(const fapi::ul_tti_request_message& msg,
                                                     const uplink_pdu_validator&         ul_pdu_validator,
                                                     const fapi::prach_config&           prach_cfg,
                                                     const fapi::carrier_config&         carrier_cfg,
                                                     srslog::basic_logger&               logger,
                                                     unsigned                            sector_id)
{
  uplink_pdus pdus;
  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case fapi::ul_pdu_type::PRACH: {
        prach_buffer_context& context = pdus.prach.emplace_back();
        convert_prach_fapi_to_phy(context, pdu.prach_pdu, prach_cfg, carrier_cfg, msg.sfn, msg.slot, sector_id);
        if (!ul_pdu_validator.is_valid(get_prach_dectector_config_from(context))) {
          logger.info("Not supported PRACH PDU detected. Skipping UL_TTI.request message in {}.{}.", msg.sfn, msg.slot);

          return {};
        }

        break;
      }
      case fapi::ul_pdu_type::PUCCH: {
        uplink_processor::pucch_pdu& ul_pdu = pdus.pucch.emplace_back();
        convert_pucch_fapi_to_phy(ul_pdu, pdu.pucch_pdu, msg.sfn, msg.slot);
        if (!is_pucch_pdu_valid(ul_pdu_validator, ul_pdu)) {
          logger.info("Not supported PUCCH PDU detected. Skipping UL_TTI.request message in {}.{}.", msg.sfn, msg.slot);

          return {};
        }

        break;
      }
      case fapi::ul_pdu_type::PUSCH: {
        uplink_processor::pusch_pdu& ul_pdu = pdus.pusch.emplace_back();
        convert_pusch_fapi_to_phy(ul_pdu, pdu.pusch_pdu, msg.sfn, msg.slot);
        if (!ul_pdu_validator.is_valid(ul_pdu.pdu)) {
          logger.info("Not supported PUSCH PDU detected. Skipping UL_TTI.request message in {}.{}.", msg.sfn, msg.slot);

          return {};
        }
        break;
      }
      case fapi::ul_pdu_type::SRS:
      default:
        srsgnb_assert(0, "UL_TTI.request PDU type value ([]) not recognized.", static_cast<unsigned>(pdu.pdu_type));
    }
  }
  return pdus;
}

void fapi_to_phy_translator::ul_tti_request(const fapi::ul_tti_request_message& msg)
{
  // :TODO: check the messages order. Do this in a different class.

  std::lock_guard<std::mutex> lock(mutex);

  const uplink_pdus& pdus =
      translate_ul_tti_pdus_to_phy_pdus(msg, ul_pdu_validator, prach_cfg, carrier_cfg, logger, sector_id);

  // Add the PUCCH and PUSCH PDUs to the repository for later processing.
  slot_point slot(scs, msg.sfn, msg.slot);
  for (const auto& pdu : pdus.pusch) {
    ul_pdu_repository.add_pusch_pdu(slot, pdu);
  }
  for (const auto& pdu : pdus.pucch) {
    ul_pdu_repository.add_pucch_pdu(slot, pdu);
  }

  // Process the PRACHs
  for (const auto& context : pdus.prach) {
    ul_request_processor.process_prach_request(context);
  }

  if (pdus.pusch.empty() && pdus.pucch.empty()) {
    return;
  }

  // Notify to capture uplink slot.
  resource_grid_context rg_context;
  rg_context.slot   = slot;
  rg_context.sector = sector_id;
  // Get ul_resource_grid.
  resource_grid& ul_rg = ul_rg_pool.get_resource_grid(rg_context);
  // Note: this won't work for multiple UL_TTI.request messages in the same slot.
  ul_rg.set_all_zero();
  // Request to capture uplink slot.
  ul_request_processor.process_uplink_slot_request(rg_context, ul_rg);
}

void fapi_to_phy_translator::ul_dci_request(const fapi::ul_dci_request_message& msg)
{
  std::lock_guard<std::mutex> lock(mutex);

  static_vector<pdcch_processor::pdu_t, MAX_PUCCH_PDUS_PER_SLOT> pdus;
  for (const auto& pdu : msg.pdus) {
    pdcch_processor::pdu_t& pdcch_pdu = pdus.emplace_back();
    convert_pdcch_fapi_to_phy(pdcch_pdu, pdu.pdu, msg.sfn, msg.slot);
    if (!dl_pdu_validator.is_valid(pdcch_pdu)) {
      logger.info("Not supported PDCCH PDU detect. Skipping UL_DCI.request message in {}.{}.", msg.sfn, msg.slot);

      return;
    }
  }

  for (const auto& pdcch_pdu : pdus) {
    current_slot_controller->process_pdcch(pdcch_pdu);
  }
}

void fapi_to_phy_translator::tx_data_request(const fapi::tx_data_request_message& msg)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Skip if there is no PDSCH PDU in the repository. This may be caused by a PDU not supported in the
  // DL_TTI.request.
  if (pdsch_pdu_repository.empty()) {
    return;
  }

  srsgnb_assert(msg.pdus.size() == pdsch_pdu_repository.size(),
                "Invalid TX_data.request received. Message contains ({}) payload PDUs but it expects ({})",
                msg.pdus.size(),
                pdsch_pdu_repository.size());

  for (unsigned i = 0, e = msg.pdus.size(); i != e; ++i) {
    static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> data;
    const fapi::tx_data_req_pdu&                                                  pdu = msg.pdus[i];
    data.emplace_back(pdu.tlv_custom.payload, pdu.tlv_custom.length.value());

    current_slot_controller->process_pdsch(data, pdsch_pdu_repository[i]);
  }
}

void fapi_to_phy_translator::handle_new_slot(slot_point slot)
{
  std::lock_guard<std::mutex> lock(mutex);
  current_slot_controller = slot_based_upper_phy_controller(dl_processor_pool, dl_rg_pool, slot, sector_id);
  pdsch_pdu_repository.clear();
  ul_pdu_repository.clear_slot(slot);
}

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
#include "srsgnb/fapi/messages.h"
#include "srsgnb/fapi_adaptor/phy/messages/pdcch.h"
#include "srsgnb/fapi_adaptor/phy/messages/pdsch.h"
#include "srsgnb/fapi_adaptor/phy/messages/prach.h"
#include "srsgnb/fapi_adaptor/phy/messages/ssb.h"
#include "srsgnb/phy/support/prach_buffer_context.h"
#include "srsgnb/phy/support/resource_grid_pool.h"
#include "srsgnb/phy/upper/downlink_processor.h"
#include "srsgnb/phy/upper/uplink_request_processor.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

namespace {

/// Dummy implementation of a downlink processor.
class downlink_processor_dummy : public downlink_processor
{
public:
  void process_pdcch(const pdcch_processor::pdu_t& pdu) override {}
  void process_pdsch(const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
                     const pdsch_processor::pdu_t&                                                        pdu) override
  {
  }
  void process_ssb(const ssb_processor::pdu_t& pdu) override {}
  void process_nzp_csi_rs(const csi_rs_processor::config_t& config) override {}
  void configure_resource_grid(const resource_grid_context& context, resource_grid& grid) override {}
  void finish_processing_pdus() override {}
};

} // namespace

/// Instance used by the initial slot based task processor.
static downlink_processor_dummy dummy_dl_processor;

fapi_to_phy_translator::slot_based_upper_phy_controller::slot_based_upper_phy_controller() :
  dl_processor(dummy_dl_processor)
{
}

fapi_to_phy_translator::slot_based_upper_phy_controller::slot_based_upper_phy_controller(
    downlink_processor_pool& dl_processor_pool,
    resource_grid_pool&      rg_pool,
    slot_point               slot,
    unsigned                 sector_id) :
  slot(slot), dl_processor(dl_processor_pool.get_processor(slot, sector_id))
{
  resource_grid_context context = {slot, sector_id};
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

void fapi_to_phy_translator::dl_tti_request(const dl_tti_request_message& msg)
{
  // :TODO: check the current slot matches the DL_TTI.request slot. Do this in a different class.
  // :TODO: check the messages order. Do this in a different class.

  std::lock_guard<std::mutex> lock(mutex);

  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case dl_pdu_type::CSI_RS:
        break;
      case dl_pdu_type::PDCCH: {
        pdcch_processor::pdu_t pdcch_pdu;
        convert_pdcch_fapi_to_phy(pdcch_pdu, pdu.pdcch_pdu, msg.sfn, msg.slot);
        current_slot_controller->process_pdcch(pdcch_pdu);
        break;
      }
      case dl_pdu_type::PDSCH: {
        pdsch_pdus.emplace_back();
        convert_pdsch_fapi_to_phy(pdsch_pdus.back(), pdu.pdsch_pdu, msg.sfn, msg.slot);
        break;
      }
      case dl_pdu_type::SSB: {
        ssb_processor::pdu_t ssb_pdu;
        convert_ssb_fapi_to_phy(ssb_pdu, pdu.ssb_pdu, msg.sfn, msg.slot, scs_common);
        current_slot_controller->process_ssb(ssb_pdu);
        break;
      }
      default:
        srsgnb_assert(0, "DL_TTI.request PDU type value ([]) not recognized.", static_cast<unsigned>(pdu.pdu_type));
    }
  }
}

void fapi_to_phy_translator::ul_tti_request(const ul_tti_request_message& msg)
{
  // :TODO: check the messages order. Do this in a different class.

  std::lock_guard<std::mutex> lock(mutex);

  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case ul_pdu_type::PRACH: {
        prach_buffer_context context;
        convert_prach_fapi_to_phy(context, pdu.prach_pdu, prach_tlv, carrier_tlv, msg.sfn, msg.slot, sector_id);
        ul_request_processor.process_prach_request(context);
        break;
      }
      case ul_pdu_type::PUCCH:
      case ul_pdu_type::PUSCH:
      case ul_pdu_type::SRS:
      case ul_pdu_type::msg_a_PUSCH:
      default:
        srsgnb_assert(0, "UL_TTI.request PDU type value ([]) not recognized.", static_cast<unsigned>(pdu.pdu_type));
    }
  }
}

void fapi_to_phy_translator::ul_dci_request(const ul_dci_request_message& msg) {}

void fapi_to_phy_translator::tx_data_request(const tx_data_request_message& msg)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Sanity check.
  srsgnb_assert(msg.pdus.size() == pdsch_pdus.size(),
                "Invalid TX_data.request received. Message contains ({}) payload PDUs but it expects ({})",
                msg.pdus.size(),
                pdsch_pdus.size());

  for (unsigned i = 0, e = msg.pdus.size(); i != e; ++i) {
    static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> data;
    const tx_data_req_pdu&                                                        pdu = msg.pdus[i];
    data.emplace_back(pdu.tlv_custom.payload, pdu.tlv_custom.length);

    current_slot_controller->process_pdsch(data, pdsch_pdus[i]);
  }
}

void fapi_to_phy_translator::handle_new_slot(slot_point slot)
{
  std::lock_guard<std::mutex> lock(mutex);
  current_slot_controller = slot_based_upper_phy_controller(dl_processor_pool, rg_pool, slot, sector_id);
  pdsch_pdus.clear();
}

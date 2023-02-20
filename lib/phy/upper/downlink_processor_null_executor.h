/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_reader_empty.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/phy/upper/downlink_processor.h"
#include "srsran/phy/upper/upper_phy_rg_gateway.h"

namespace srsran {

class downlink_processor_null_executor : public downlink_processor
{
public:
  downlink_processor_null_executor(upper_phy_rg_gateway& gateway_, srslog::basic_logger& logger_) :
    gateway(gateway_), logger(logger_)
  {
  }

  void process_pdcch(const pdcch_processor::pdu_t& pdu) override
  {
    logger.set_context(pdu.slot.sfn(), pdu.slot.slot_index());
    logger.info("Ignored PDCCH: {}", pdu);
  }

  void process_pdsch(const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
                     const pdsch_processor::pdu_t&                                                        pdu) override
  {
    logger.set_context(pdu.slot.sfn(), pdu.slot.slot_index());
    logger.info("Ignored PDSCH: {}", pdu);
  }
  void process_ssb(const ssb_processor::pdu_t& pdu) override
  {
    logger.set_context(pdu.slot.sfn(), pdu.slot.slot_index());
    logger.info("Ignored SSB: {}", pdu);
  }
  void process_nzp_csi_rs(const nzp_csi_rs_generator::config_t& /**/) override
  {
    // Ignore.
  }
  void configure_resource_grid(const resource_grid_context& context, resource_grid& /**/) override
  {
    logger.set_context(context.slot.sfn(), context.slot.slot_index());
    logger.error("Ignoring DL PDUS.");

    // Send empty resource grid.
    gateway.send(context, rg_reader_empty);
  }

  void finish_processing_pdus() override
  {
    // Ignored.
  }

  bool is_reserved() const override { return false; }

private:
  const resource_grid_reader_empty rg_reader_empty;
  upper_phy_rg_gateway&            gateway;
  srslog::basic_logger&            logger;
};

} // namespace srsran
/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UNITTESTS_PHY_UPPER_DOWNLINK_PROCESSOR_TEST_DOUBLES_H
#define SRSGNB_UNITTESTS_PHY_UPPER_DOWNLINK_PROCESSOR_TEST_DOUBLES_H

#include "srsgnb/phy/upper/downlink_processor.h"

namespace srsgnb {

/// Implementation of a downlink processor for test only (FTO). It behaves as a dummy implementation but stores the
/// given id in construction.
class downlink_processor_fto : public downlink_processor
{
public:
  unsigned id;

  explicit downlink_processor_fto(unsigned id) : id(id) {}

  void process_pdcch(pdcch_processor::pdu_t& pdu) override {}

  void process_pdsch(const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
                     const pdsch_processor::pdu_t&                                                        pdu) override
  {
  }

  void process_ssb(const ssb_processor::pdu_t& pdu) override {}

  void process_csi_rs(const csi_rs_processor::config_t& config) override {}

  void configure_resource_grid(const resource_grid_context& context, resource_grid& grid) override {}

  void finish_processing_pdus() override {}
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_DOWNLINK_PROCESSOR_TEST_DOUBLES_H

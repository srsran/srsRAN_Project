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

/// Spy implementation of a downlink processor
class downlink_processor_spy : public downlink_processor
{
  unsigned id;
  bool     configure_resource_grid_method_called = false;
  bool     process_ssb_method_called             = false;
  bool     finish_processing_pdus_method_called  = false;

public:
  explicit downlink_processor_spy(unsigned id) : id(id) {}

  /// Returns the identifier of downlink processor.
  unsigned get_id() const { return id; }

  /// Returns true if the method has been called, otherwise false.
  bool has_configure_resource_grid_method_been_called() const { return configure_resource_grid_method_called; }
  bool has_process_ssb_method_been_called() const { return process_ssb_method_called; }
  bool has_finish_processing_pdus_method_been_called() const { return finish_processing_pdus_method_called; }

  void process_pdcch(const pdcch_processor::pdu_t& pdu) override {}

  void process_pdsch(const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
                     const pdsch_processor::pdu_t&                                                        pdu) override
  {
  }

  void process_ssb(const ssb_processor::pdu_t& pdu) override { process_ssb_method_called = true; }

  void process_nzp_csi_rs(const csi_rs_processor::config_t& config) override {}

  void configure_resource_grid(const resource_grid_context& context, resource_grid& grid) override
  {
    configure_resource_grid_method_called = true;
  }

  void finish_processing_pdus() override { finish_processing_pdus_method_called = true; }
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_DOWNLINK_PROCESSOR_TEST_DOUBLES_H

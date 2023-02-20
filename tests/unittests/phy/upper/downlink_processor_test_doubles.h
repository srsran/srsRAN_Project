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

#include "srsran/phy/upper/downlink_processor.h"

namespace srsran {

/// Spy implementation of a downlink processor
class downlink_processor_spy : public downlink_processor
{
  unsigned id;
  bool     reserved;
  bool     configure_resource_grid_method_called = false;
  bool     process_ssb_method_called             = false;
  bool     finish_processing_pdus_method_called  = false;

public:
  explicit downlink_processor_spy(unsigned id_, bool reserved_ = false) : id(id_), reserved(reserved_) {}

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

  void process_nzp_csi_rs(const nzp_csi_rs_generator::config_t& config) override {}

  void configure_resource_grid(const resource_grid_context& context, resource_grid& grid) override
  {
    configure_resource_grid_method_called = true;
  }

  void finish_processing_pdus() override { finish_processing_pdus_method_called = true; }

  bool is_reserved() const override { return reserved; }
};

} // namespace srsran

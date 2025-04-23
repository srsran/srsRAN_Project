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

#pragma once

#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/phy/upper/downlink_processor.h"

namespace srsran {

/// Spy implementation of a downlink processor
class downlink_processor_spy : public downlink_processor_base,
                               public downlink_processor_controller,
                               private unique_downlink_processor::downlink_processor_callback
{
public:
  explicit downlink_processor_spy(unsigned id_) : id(id_) {}

  /// Returns the identifier of downlink processor.
  unsigned get_id() const { return id; }

  /// Returns true if the method has been called, otherwise false.
  bool has_configure_resource_grid_method_been_called() const { return configure_resource_grid_method_called; }
  bool has_process_ssb_method_been_called() const { return process_ssb_method_called; }
  bool has_finish_processing_pdus_method_been_called() const { return finish_processing_pdus_method_called; }

  downlink_processor_controller& get_controller() override { return *this; }
  void                           stop() override
  {
    // Do nothing.
  }

private:
  unique_downlink_processor configure_resource_grid(const resource_grid_context& context,
                                                    shared_resource_grid         grid) override
  {
    configure_resource_grid_method_called = true;
    return unique_downlink_processor(*this);
  }

  unsigned id;
  bool     configure_resource_grid_method_called = false;
  bool     process_ssb_method_called             = false;
  bool     finish_processing_pdus_method_called  = false;

  void process_pdcch(const pdcch_processor::pdu_t& pdu) override {}

  void process_pdsch(static_vector<shared_transport_block, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> data,
                     const pdsch_processor::pdu_t&                                                    pdu) override
  {
  }

  void process_ssb(const ssb_processor::pdu_t& pdu) override { process_ssb_method_called = true; }

  void process_nzp_csi_rs(const nzp_csi_rs_generator::config_t& config) override {}

  void process_prs(const prs_generator_configuration& config) override {}

  void finish_processing_pdus() override { finish_processing_pdus_method_called = true; }
};

} // namespace srsran

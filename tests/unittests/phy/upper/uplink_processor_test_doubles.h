/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/uplink_pdu_slot_repository.h"
#include "srsran/phy/upper/uplink_processor.h"

namespace srsran {

class uplink_processor_spy : public uplink_processor,
                             private unique_uplink_pdu_slot_repository::uplink_pdu_slot_repository_callback,
                             private uplink_slot_processor
{
public:
  unique_uplink_pdu_slot_repository get_pdu_slot_repository(slot_point slot) override
  {
    return unique_uplink_pdu_slot_repository(*this);
  }

  void finish_adding_pdus() override {}

  uplink_slot_processor& get_slot_processor() override { return *this; }

  bool is_add_pusch_pdu_method_called() const { return has_add_pusch_pdu_method_called; }

  bool is_add_pucch_pdu_method_called() const { return has_add_pucch_pdu_method_called; }

  bool is_add_srs_pdu_method_called() const { return has_add_srs_pdu_method_called; }

  bool is_process_prach_method_called() const { return has_process_prach_method_called; }

  unsigned get_on_rx_symbol_count() const { return on_rx_symbol_count; }

  unsigned get_last_end_symbol_index() const { return last_end_symbol_index; }

private:
  void add_pusch_pdu(const uplink_pdu_slot_repository::pusch_pdu& pdu) override
  {
    has_add_pusch_pdu_method_called = true;
  }

  void add_pucch_pdu(const uplink_pdu_slot_repository::pucch_pdu& pdu) override
  {
    has_add_pucch_pdu_method_called = true;
  }

  void add_srs_pdu(const uplink_pdu_slot_repository::srs_pdu& pdu) override { has_add_srs_pdu_method_called = true; }

  void handle_rx_symbol(const shared_resource_grid& grid, unsigned end_symbol_index) override
  {
    last_end_symbol_index = end_symbol_index;
    ++on_rx_symbol_count;
  }

  void process_prach(const prach_buffer& buffer, const prach_buffer_context& context) override
  {
    has_process_prach_method_called = true;
  }

  void discard_slot() override {}

private:
  bool     has_add_pusch_pdu_method_called = false;
  bool     has_add_pucch_pdu_method_called = false;
  bool     has_add_srs_pdu_method_called   = false;
  bool     has_process_prach_method_called = false;
  unsigned last_end_symbol_index           = std::numeric_limits<unsigned>::max();
  unsigned on_rx_symbol_count              = 0;
};

} // namespace srsran

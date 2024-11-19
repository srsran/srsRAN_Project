/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/scheduler/scheduler_configurator.h"
#include "srsran/scheduler/scheduler_feedback_handler.h"

namespace srsran {

struct pucch_info;

namespace test_helper {

/// Create a detected PRACH preamble.
rach_indication_message::preamble create_preamble(unsigned preamble_id, rnti_t tc_rnti);

/// Create a RACH indication with one occation and multiple PRACH preambles.
rach_indication_message create_rach_indication(slot_point                                            next_slot_rx,
                                               const std::vector<rach_indication_message::preamble>& preambles);

/// Create dummy UCI indication based on a PUCCH PDU.
uci_indication create_uci_indication(slot_point uci_sl, du_ue_index_t ue_idx, const pucch_info& pucch_pdu);

} // namespace test_helper

} // namespace srsran

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

#include "srsran/mac/mac_pdu_handler.h"

namespace srsran {
namespace test_helpers {

/// Creates a CCCH message with the given RNTI.
mac_rx_data_indication create_ccch_message(slot_point sl_rx, rnti_t rnti);

} // namespace test_helpers
} // namespace srsran
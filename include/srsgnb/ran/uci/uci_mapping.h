/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsgnb {

/// HARQ values for UCI PUCCH Format 0 or Format 1.
enum class uci_pucch_f0_or_f1_harq_values { nack, ack, dtx };

} // namespace srsgnb
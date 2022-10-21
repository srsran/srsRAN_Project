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

inline constexpr const char* to_string(uci_pucch_f0_or_f1_harq_values value)
{
  switch (value) {
    case uci_pucch_f0_or_f1_harq_values::nack:
      return "nack";
    case uci_pucch_f0_or_f1_harq_values::ack:
      return "ack";
    case uci_pucch_f0_or_f1_harq_values::dtx:
      return "dtx";
    default:
      srsgnb_assert(0, "invalid UCI PUCCH Format 0/1 HARQ value={}", value);
      break;
  }
  return "";
}

} // namespace srsgnb
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

#include "srsran/phy/upper/channel_processors/pdsch/pdsch_processor.h"

namespace srsran {

/// Implements a parameter validator for \ref pdsch_processor_impl.
class pdsch_processor_validator_impl : public pdsch_pdu_validator
{
public:
  /// \brief Asserts PDU.
  ///
  /// It triggers an assertion if the PDU is not valid for this processor.
  static void assert_pdu(const pdsch_processor::pdu_t& pdu);

  // See interface for documentation.
  bool is_valid(const pdsch_processor::pdu_t& pdu) const override;
};

} // namespace srsran

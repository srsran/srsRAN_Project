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

#include "srsran/phy/upper/channel_processors/pdsch/factories.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_processor.h"

namespace srsran {

/// Implements a parameter validator for \ref pdsch_processor_impl.
class pdsch_processor_validator_impl : public pdsch_pdu_validator
{
public:
  // See interface for documentation.
  error_type<std::string> is_valid(const pdsch_processor::pdu_t& pdu) const override;
};

} // namespace srsran

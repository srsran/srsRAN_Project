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

#include "srsran/adt/expected.h"
#include "srsran/phy/upper/channel_estimation.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor.h"

namespace srsran {

/// Implements a parameter validator for \ref pusch_processor_impl.
class pusch_processor_validator_impl : public pusch_pdu_validator
{
public:
  /// \brief Constructs PUSCH processor validator.
  ///
  /// It requires channel estimate dimensions to limit the bandwidth, slot duration, number of receive ports and
  /// transmit layers.
  ///
  /// \param[in] ce_dims_ Provides the channel estimates dimensions.
  explicit pusch_processor_validator_impl(const channel_estimate::channel_estimate_dimensions& ce_dims_);

  // See interface for documentation.
  error_type<std::string> is_valid(const pusch_processor::pdu_t& pdu) const override;

private:
  channel_estimate::channel_estimate_dimensions ce_dims;
};

} // namespace srsran

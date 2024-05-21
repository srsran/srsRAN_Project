/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
  bool is_valid(const pusch_processor::pdu_t& pdu) const override;

private:
  channel_estimate::channel_estimate_dimensions ce_dims;
};

} // namespace srsran

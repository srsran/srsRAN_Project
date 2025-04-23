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

#include "srsran/phy/upper/channel_processors/pdcch/pdcch_processor.h"
#include "srsran/ran/pdcch/pdcch_constants.h"

namespace srsran {

/// Implements a parameter validator for \ref pdcch_processor_impl.
class pdcch_processor_validator_impl : public pdcch_pdu_validator
{
public:
  // See interface for documentation.
  error_type<std::string> is_valid(const pdcch_processor::pdu_t& pdu) const override;
};

} // namespace srsran

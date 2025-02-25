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

#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/ru/ru_timing_notifier.h"
#include <chrono>

namespace srsran {

class ru_timing_notifier;

class ru_ofh_timing_notifier_impl : public ofh::ota_symbol_boundary_notifier
{
public:
  ru_ofh_timing_notifier_impl(unsigned            nof_slot_offset_du_ru_,
                              unsigned            nof_symbols_per_slot,
                              subcarrier_spacing  scs,
                              ru_timing_notifier& timing_notifier_);

  // See interface for documentation.
  void on_new_symbol(const ofh::slot_symbol_point_context& symbol_point_context) override;

private:
  const unsigned                 nof_slot_offset_du_ru;
  const std::chrono::nanoseconds nof_slots_offset_du_ru_ns;
  const unsigned                 half_slot_symbol;
  const unsigned                 full_slot_symbol;
  ru_timing_notifier&            timing_notifier;
};

} // namespace srsran

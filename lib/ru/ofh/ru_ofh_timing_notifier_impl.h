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

#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/ofh/timing/slot_symbol_point.h"
#include "srsran/ru/ru_timing_notifier.h"

namespace srsran {

class ru_timing_notifier;

class ru_ofh_timing_notifier_impl : public ofh::ota_symbol_boundary_notifier
{
public:
  ru_ofh_timing_notifier_impl(unsigned            nof_slot_offset_du_ru_,
                              unsigned            nof_symbols_per_slot,
                              ru_timing_notifier& timing_notifier_);

  // See interface for documentation.
  void on_new_symbol(ofh::slot_symbol_point symbol_point) override;

private:
  /// Notifies new slot.
  void notify_new_slot(ofh::slot_symbol_point symbol_point);

private:
  const unsigned      nof_slot_offset_du_ru;
  const unsigned      half_slot_symbol;
  const unsigned      full_slot_symbol;
  ru_timing_notifier& timing_notifier;
  slot_point          current_slot;
};

} // namespace srsran

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

#include "srsran/ofh/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/ofh/slot_symbol_point.h"
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

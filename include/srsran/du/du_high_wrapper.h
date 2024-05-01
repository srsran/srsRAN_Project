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

#include "srsran/du/du.h"

namespace srsran {

namespace fapi {

class slot_data_message_notifier;
class slot_error_message_notifier;
class slot_time_message_notifier;

} // namespace fapi

class du_high;

/// Distributed Unit high wrapper interface. Wraps a DU high with the MAC FAPI adaptor.
class du_high_wrapper : public du
{
public:
  virtual ~du_high_wrapper() = default;

  /// Returns the DU high from this DU wrapper.
  virtual du_high& get_du_high() = 0;

  /// Returns the FAPI slot data message notifier for the given cell of this DU high wrapper.
  virtual fapi::slot_data_message_notifier& get_slot_data_message_notifier(unsigned cell_id) = 0;

  /// Returns the FAPI slot error message notifier for the given cell of this DU high wrapper.
  virtual fapi::slot_error_message_notifier& get_slot_error_message_notifier(unsigned cell_id) = 0;

  /// Returns the FAPI slot time message notifier for the given cell of this DU high wrapper.
  virtual fapi::slot_time_message_notifier& get_slot_time_message_notifier(unsigned cell_id) = 0;
};

} // namespace srsran

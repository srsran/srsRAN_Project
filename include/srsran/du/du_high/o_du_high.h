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

namespace srsran {

class du_power_controller;

namespace fapi {
class slot_data_message_notifier;
class slot_error_message_notifier;
class slot_time_message_notifier;
} // namespace fapi

namespace srs_du {

class du_high;

/// O-RAN Distributed Unit high interface. O-DU high groups the DU high with the MAC-FAPI adaptor.
class o_du_high
{
public:
  virtual ~o_du_high() = default;

  /// Returns the power controller of this O-RAN DU high.
  virtual du_power_controller& get_power_controller() = 0;

  /// Returns the DU high from this O-RAN DU high.
  virtual du_high& get_du_high() = 0;

  /// Returns the FAPI slot data message notifier for the given cell of this O-RAN DU high.
  virtual fapi::slot_data_message_notifier& get_slot_data_message_notifier(unsigned cell_id) = 0;

  /// Returns the FAPI slot error message notifier for the given cell of this O-RAN DU high.
  virtual fapi::slot_error_message_notifier& get_slot_error_message_notifier(unsigned cell_id) = 0;

  /// Returns the FAPI slot time message notifier for the given cell of this O-RAN DU high.
  virtual fapi::slot_time_message_notifier& get_slot_time_message_notifier(unsigned cell_id) = 0;
};

} // namespace srs_du
} // namespace srsran

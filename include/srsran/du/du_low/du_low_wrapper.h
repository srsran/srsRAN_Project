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

namespace fapi {
class slot_data_message_notifier;
class slot_error_message_notifier;
class slot_last_message_notifier;
class slot_message_gateway;
class slot_time_message_notifier;
} // namespace fapi

namespace srs_du {

class du_low;

/// DU low wrapper interface. Wraps the DU low with the PHY-FAPI adatpor.
class du_low_wrapper
{
public:
  /// Default destructor.
  virtual ~du_low_wrapper() = default;

  /// Returns the upper PHY of this DU low.
  virtual du_low& get_du_low() = 0;

  /// Returns a reference to the FAPI slot-based message gateway for the given cell of this DU low wrapper.
  virtual fapi::slot_message_gateway& get_slot_message_gateway(unsigned cell_id) = 0;

  /// Returns a reference to the FAPI slot-based last message notifier for the given cell of this DU low wrapper.
  virtual fapi::slot_last_message_notifier& get_slot_last_message_notifier(unsigned cell_id) = 0;

  /// Configures the FAPI slot-based, time-specific message notifier for the given cell to the given one.
  virtual void set_slot_time_message_notifier(unsigned                          cell_id,
                                              fapi::slot_time_message_notifier& fapi_time_notifier) = 0;

  /// Configures the FAPI slot-based, error-specific message notifier for the given cell to the given one.
  virtual void set_slot_error_message_notifier(unsigned                           cell_id,
                                               fapi::slot_error_message_notifier& fapi_error_notifier) = 0;

  /// Configures the FAPI slot-based, data-specific message notifier for the given cell to the given one.
  virtual void set_slot_data_message_notifier(unsigned                          cell_id,
                                              fapi::slot_data_message_notifier& fapi_data_notifier) = 0;
};

} // namespace srs_du
} // namespace srsran

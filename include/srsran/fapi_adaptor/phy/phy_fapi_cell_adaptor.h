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
#include <memory>

namespace srsran {

class upper_phy_rx_results_notifier;
class upper_phy_timing_notifier;
class upper_phy_error_notifier;

namespace fapi {
class slot_data_message_notifier;
class slot_error_message_notifier;
class slot_time_message_notifier;
class slot_last_message_notifier;
class slot_message_gateway;
class config_message_gateway;
class config_message_notifier;
} // namespace fapi

namespace fapi_adaptor {

/// PHY&ndash;FAPI bidirectional config message adaptor interface.
class phy_fapi_config_message_adaptor
{
public:
  virtual ~phy_fapi_config_message_adaptor() = default;

  /// Returns the FAPI configuration message gateway of this adaptor.
  virtual fapi::config_message_gateway& get_config_message_gateway() = 0;

  /// Sets the FAPI configuration message notifier of this adaptor.
  virtual void set_config_message_notifier(fapi::config_message_notifier& fapi_notifier) = 0;
};

/// \brief PHY&ndash;FAPI bidirectional slot message adaptor interface.
///
/// This adaptor is a collection of interfaces to translate FAPI messages into their PHY layer counterpart and vice
/// versa.
///
/// \note All implementations of this public interface must hold the ownership of all its internal components.
class phy_fapi_slot_message_adaptor
{
public:
  virtual ~phy_fapi_slot_message_adaptor() = default;

  /// Returns a reference to the error notifier used by the adaptor.
  virtual upper_phy_error_notifier& get_error_notifier() = 0;

  /// Returns a reference to the timing notifier used by the adaptor.
  virtual upper_phy_timing_notifier& get_timing_notifier() = 0;

  /// Returns a reference to the results notifier used by the adaptor.
  virtual upper_phy_rx_results_notifier& get_rx_results_notifier() = 0;

  /// Returns a reference to the slot-based message gateway used by the adaptor.
  virtual fapi::slot_message_gateway& get_slot_message_gateway() = 0;

  /// Returns a reference to the slot-based last message notifier used by the adaptor.
  virtual fapi::slot_last_message_notifier& get_slot_last_message_notifier() = 0;

  /// Configures the slot-based, time-specific message notifier to the given one.
  virtual void set_slot_time_message_notifier(fapi::slot_time_message_notifier& fapi_time_notifier) = 0;

  /// Configures the slot-based, error-specific message notifier to the given one.
  virtual void set_slot_error_message_notifier(fapi::slot_error_message_notifier& fapi_error_notifier) = 0;

  /// Configures the slot-based, data-specific message notifier to the given one.
  virtual void set_slot_data_message_notifier(fapi::slot_data_message_notifier& fapi_data_notifier) = 0;
};

/// PHY&ndash;FAPI bidirectional cell adaptor interface. It gives access the configuration and slot message interfaces.
class phy_fapi_cell_adaptor
{
public:
  virtual ~phy_fapi_cell_adaptor() = default;

  /// Returns the FAPI configuration message adaptor of this adaptor.
  virtual phy_fapi_config_message_adaptor& get_config_message_adaptor() = 0;

  /// Returns the FAPI slot message adaptor of this adaptor or nullptr if it does not exist.
  virtual phy_fapi_slot_message_adaptor* get_message_adaptor() = 0;
};

/// Factory to build a PHY-FAPI cell adaptor.
std::unique_ptr<phy_fapi_cell_adaptor> create_phy_fapi_cell_adaptor();

} // namespace fapi_adaptor
} // namespace srsran

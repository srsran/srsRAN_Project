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

#include "srsgnb/phy/lower/lower_phy_error_notifier.h"
#include "srsgnb/phy/lower/lower_phy_rx_symbol_notifier.h"
#include "srsgnb/phy/lower/lower_phy_timing_notifier.h"

namespace srsran {

/// \brief Lower PHY error notification spy class.
///
/// This class stores all errors notified by a lower PHY for a later inspection from an external entity.
class lower_phy_error_notifier_spy : public lower_phy_error_notifier
{
private:
  srslog::basic_logger&                   logger;
  std::vector<late_resource_grid_context> late_rg_errors;
  std::vector<prach_buffer_context>       prach_request_late_errors;
  std::vector<prach_buffer_context>       prach_request_overflow_errors;

public:
  /// Constructor: creates the logger and sets the log level (default log level is "warning").
  explicit lower_phy_error_notifier_spy(const std::string& log_level = "warning") :
    logger(srslog::fetch_basic_logger("Error Notifier"))
  {
    logger.set_level(srslog::str_to_basic_level(log_level));
  }

  // See interface for documentation.
  void on_late_resource_grid(const late_resource_grid_context& context) override
  {
    logger.set_context(context.slot.sfn(), context.slot.slot_index());
    logger.debug(
        "Sector {} - Detected late resource grid for symbol {}.", context.sector, context.symbol, context.symbol);

    late_rg_errors.push_back(context);
  }

  // See interface for documentation.
  void on_prach_request_late(const prach_buffer_context& context) override
  {
    logger.set_context(context.slot.sfn(), context.slot.slot_index());
    logger.debug("Sector {} - Detected PRACH request late for sector {}, slot {} and start symbol {}.",
                 context.sector,
                 context.slot,
                 context.start_symbol);

    prach_request_late_errors.push_back(context);
  }

  // See interface for documentation.
  void on_prach_request_overflow(const prach_buffer_context& context) override
  {
    logger.set_context(context.slot.sfn(), context.slot.slot_index());
    logger.debug("Sector {} - Detected PRACH request overflow for sector {}, slot {} and start symbol {}.",
                 context.sector,
                 context.slot,
                 context.start_symbol);

    prach_request_overflow_errors.push_back(context);
  }

  /// \brief Gets the errors notified through the lower_phy_timing_notifier::on_late_resource_grid() interface.
  /// \return A constant reference to the event list.
  const std::vector<late_resource_grid_context>& get_late_rg_errors() const { return late_rg_errors; }

  /// \brief Gets the total number of errors of any kind.
  ///
  /// A use case to ensure no event happened:
  /// \code
  /// TESTASSERT_EQ(0, error_notifier.get_nof_errors());
  /// \endcode
  ///
  /// \return The total number of events that have been registered.
  unsigned get_nof_errors() const
  {
    return late_rg_errors.size() + prach_request_late_errors.size() + prach_request_overflow_errors.size();
  }

  /// Clears all the recorded errors.
  void clear_all_errors()
  {
    late_rg_errors.clear();
    prach_request_late_errors.clear();
    prach_request_overflow_errors.clear();
  }
};

/// \brief Lower PHY receive symbol spy class.
///
/// This class stores all uplink reception events notified by a lower PHY for a later inspection from an external
/// entity.
class lower_phy_rx_symbol_notifier_spy : public lower_phy_rx_symbol_notifier
{
private:
  srslog::basic_logger& logger;

  struct rx_symbol_event {
    lower_phy_rx_symbol_context context;
    resource_grid_reader*       grid;
  };
  std::vector<rx_symbol_event> rx_symbol_events;

  struct rx_prach_event {
    prach_buffer_context context;
    prach_buffer*        grid;
  };
  std::vector<rx_prach_event> rx_prach_events;

public:
  /// Constructor: creates the logger and sets the log level (default log level is "warning").
  explicit lower_phy_rx_symbol_notifier_spy(const std::string& log_level = "warning") :
    logger(srslog::fetch_basic_logger("Rx Notifier"))
  {
    logger.set_level(srslog::str_to_basic_level(log_level));
  }

  // See interface for documentation.
  void on_rx_symbol(const lower_phy_rx_symbol_context& context, const resource_grid_reader& grid) override
  {
    logger.set_context(context.slot.sfn(), context.slot.slot_index());
    logger.debug("Sector {} - On Rx Symbol {}.", context.sector, context.nof_symbols);
  }

  // See interface for documentation.
  void on_rx_prach_window(const prach_buffer_context& context, const prach_buffer& buffer) override
  {
    logger.set_context(context.slot.sfn(), context.slot.slot_index());
    logger.debug("Sector {} - On Rx PRACH Window.", context.sector);
  }

  // See interface for documentation.
  void on_rx_srs_symbol(const lower_phy_rx_symbol_context& context) override
  {
    logger.set_context(context.slot.sfn(), context.slot.slot_index());
    logger.debug("Sector {} - On Rx SRS Symbol.", context.sector);
  }

  /// \brief Gets the total number of events of any kind.
  ///
  /// A use case to ensure no event happened:
  /// \code
  /// TESTASSERT_EQ(0, rx_symbol_notifier.get_nof_events());
  /// \endcode
  ///
  /// \return The total number of events that have been registered.
  unsigned get_nof_events() const { return rx_symbol_events.size() + rx_prach_events.size(); }

  /// \brief Gets the uplink received symbol event list.
  ///
  /// Uplink received symbols are notified through lower_phy_rx_symbol_notifier::on_rx_symbol() interface.
  ///
  /// \return A constant reference to the list.
  const std::vector<rx_symbol_event>& get_rx_symbol_events() const { return rx_symbol_events; }

  /// \brief Gets the uplink received PRACH window event list.
  ///
  /// Uplink received PRACH windows are notified through lower_phy_rx_symbol_notifier::on_rx_prach_window() interface.
  ///
  /// \return A constant reference to the list.
  const std::vector<rx_prach_event>& get_rx_prach_events() const { return rx_prach_events; }

  /// Clears all the stored events.
  void clear_all_events()
  {
    rx_symbol_events.clear();
    rx_prach_events.clear();
  }
};

/// \brief Lower PHY timing notify spy class.
///
/// This class stores all timing notification events reported by a lower PHY for a later inspection from an external
/// entity.
class lower_phy_timing_notifier_spy : public lower_phy_timing_notifier
{
private:
  srslog::basic_logger&                 logger;
  std::vector<lower_phy_timing_context> tti_boundaries_events;
  std::vector<lower_phy_timing_context> ul_half_slot_events;
  std::vector<lower_phy_timing_context> ul_full_slot_events;

public:
  /// Constructor: creates the logger and sets the log level (default log level is "warning").
  explicit lower_phy_timing_notifier_spy(const std::string& log_level) :
    logger(srslog::fetch_basic_logger("Timing Notifier"))
  {
    logger.set_level(srslog::str_to_basic_level(log_level));
  }

  // See interface for documentation.
  void on_tti_boundary(const lower_phy_timing_context& context) override
  {
    logger.set_context(context.slot.sfn(), context.slot.slot_index());
    logger.debug("New TTI boundary.");
    tti_boundaries_events.push_back(context);
  }

  // See interface for documentation.
  void on_ul_half_slot_boundary(const lower_phy_timing_context& context) override
  {
    logger.set_context(context.slot.sfn(), context.slot.slot_index());
    logger.debug("Half uplink slot boundary.");
    ul_half_slot_events.push_back(context);
  }

  // See interface for documentation.
  void on_ul_full_slot_boundary(const lower_phy_timing_context& context) override
  {
    logger.set_context(context.slot.sfn(), context.slot.slot_index());
    logger.debug("Full uplink slot boundary.");
    ul_full_slot_events.push_back(context);
  }

  /// \brief Gets the total number of events of any kind.
  ///
  /// A use case to ensure no event happened:
  /// \code
  /// TESTASSERT_EQ(0, timing_notifier.get_nof_events());
  /// \endcode
  ///
  /// \return The total number of events that have been registered.
  unsigned get_nof_events() const
  {
    return tti_boundaries_events.size() + ul_half_slot_events.size() + ul_full_slot_events.size();
  }

  /// \brief Gets the TTI boundaries event list.
  ///
  /// TTI boundaries are notified through lower_phy_timing_notifier::on_tti_boundary() interface.
  ///
  /// \return A constant reference to the event list.
  const std::vector<lower_phy_timing_context>& get_tti_boundaries_events() const { return tti_boundaries_events; }

  /// \brief Gets the uplink half slot event list.
  ///
  /// TTI boundaries are notified through lower_phy_timing_notifier::on_ul_half_slot_boundary() interface.
  ///
  /// \return A constant reference to the event list.
  const std::vector<lower_phy_timing_context>& get_ul_half_slot_events() const { return ul_half_slot_events; }

  /// \brief Gets the uplink full slot event list.
  ///
  /// TTI boundaries are notified through lower_phy_timing_notifier::on_ul_full_slot_boundary() interface.
  ///
  /// \return A constant reference to the event list.
  const std::vector<lower_phy_timing_context>& get_ul_full_slot_events() const { return ul_full_slot_events; }

  /// Clears all the stored events.
  void clear_all_events()
  {
    tti_boundaries_events.clear();
    ul_half_slot_events.clear();
    ul_full_slot_events.clear();
  }
};

} // namespace srsran

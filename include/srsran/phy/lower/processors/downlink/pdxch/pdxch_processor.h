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

namespace srsran {

class pdxch_processor_notifier;
class pdxch_processor_request_handler;
class pdxch_processor_baseband;
class slot_point;

/// \brief PDxCH processor main interface.
///
/// Provides a single access point to all downlink channel processor interfaces, that is \ref pdxch_processor_notifier,
/// \ref pdxch_processor_request_handler and \ref pdxch_processor_baseband.
///
/// \note All implementations of this interface must trigger an assertion error if the PDxCH processor is not connected
/// to a notifier before calling \ref get_request_handler() or \ref get_baseband().
class pdxch_processor
{
public:
  /// Default destructor.
  virtual ~pdxch_processor() = default;

  /// Connects the PDxCH processor with a notifier.
  virtual void connect(pdxch_processor_notifier& notifier) = 0;

  /// \brief Gets the PDxCH processor request handler.
  /// \return A reference to the internal PDxCH processor request handler.
  virtual pdxch_processor_request_handler& get_request_handler() = 0;

  /// \brief Gets the PDxCH processor baseband interface.
  /// \return A reference to the internal PDxCH processor baseband interface.
  virtual pdxch_processor_baseband& get_baseband() = 0;
};

} // namespace srsran

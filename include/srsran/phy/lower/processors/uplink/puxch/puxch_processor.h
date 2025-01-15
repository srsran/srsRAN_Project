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

class puxch_processor_notifier;
class puxch_processor_request_handler;
class puxch_processor_baseband;
class slot_point;

/// \brief PUCCH and PUSCH processor main interface.
///
/// Provides a single access point to all PUCCH- and PUSCH-related interfaces, that is \ref puxch_processor_notifier,
/// \ref puxch_processor_request_handler and \ref puxch_processor_baseband.
///
/// \note All implementations of this interface must trigger an assertion error if the PUxCH processor is not connected
/// to a notifier before calling \ref get_request_handler() or \ref get_baseband().
class puxch_processor
{
public:
  /// Default destructor.
  virtual ~puxch_processor() = default;

  /// Connects the PUxCH processor with a notifier.
  virtual void connect(puxch_processor_notifier& notifier) = 0;

  /// \brief Gets the PUxCH processor request handler.
  /// \return A reference to the internal PUxCH processor request handler.
  virtual puxch_processor_request_handler& get_request_handler() = 0;

  /// \brief Gets the PUxCH processor baseband interface.
  /// \return A reference to the internal PUxCH processor baseband interface.
  virtual puxch_processor_baseband& get_baseband() = 0;
};

} // namespace srsran
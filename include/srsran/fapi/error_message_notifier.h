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
namespace fapi {

struct error_indication_message;

/// \brief Error-specific message notifier interface.
///
/// This interface notifies the reception of error-specific FAPI messages from the underlying PHY.
class error_message_notifier
{
public:
  virtual ~error_message_notifier() = default;

  /// \brief Notifies the reception of an error indication message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_error_indication(const error_indication_message& msg) = 0;
};

} // namespace fapi
} // namespace srsran

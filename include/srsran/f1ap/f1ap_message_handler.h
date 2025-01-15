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

struct f1ap_message;

/// Handles incoming F1AP messages.
class f1ap_message_handler
{
public:
  virtual ~f1ap_message_handler() = default;

  /// Handles the given F1AP message.
  virtual void handle_message(const f1ap_message& msg) = 0;
};

} // namespace srsran

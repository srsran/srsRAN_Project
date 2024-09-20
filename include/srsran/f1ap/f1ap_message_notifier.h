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

struct f1ap_message;

/// Notifier interface used to notify outgoing F1AP messages.
class f1ap_message_notifier
{
public:
  virtual ~f1ap_message_notifier() = default;

  /// This callback is invoked on each outgoing F1AP message.
  virtual void on_new_message(const f1ap_message& msg) = 0;
};

} // namespace srsran

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

// Forward declaration.
struct f1ap_message;

/// Interface for the handler of received F1AP PDUs.
class f1ap_message_handler
{
public:
  virtual ~f1ap_message_handler() = default;

  /// Handle the incoming F1AP message.
  virtual void handle_message(const f1ap_message& msg) = 0;
};

/// Notifier interface used to forward F1AP PDUs to either the CU-CP or DU.
class f1ap_message_notifier
{
public:
  virtual ~f1ap_message_notifier() = default;

  /// This callback is invoked on each forwarded F1AP message.
  virtual void on_new_message(const f1ap_message& msg) = 0;
};

} // namespace srsran

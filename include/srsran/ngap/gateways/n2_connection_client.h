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

#include "srsran/ngap/ngap.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_message_handler;
class ngap_event_handler;

/// Handler of N2 connection between CU-CP and AMF.
class n2_connection_client : public ngap_message_notifier
{
public:
  /// \brief Connect the CU-CP to the N2 connection client.
  virtual void connect_cu_cp(ngap_message_handler& msg_handler_, ngap_event_handler& ev_handler_) = 0;

  /// \brief Disconnect the CU-CP from the N2 connection client.
  virtual void disconnect() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
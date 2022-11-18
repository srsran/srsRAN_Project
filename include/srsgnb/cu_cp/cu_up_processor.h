/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "cu_cp_types.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/e1_interface/cu_cp/e1_cu_cp.h"
#include "srsgnb/support/timers.h"
#include <string>

namespace srsgnb {
namespace srs_cu_cp {

/// Forward declared messages.
struct cu_up_e1_setup_request_message;

/// Interface for an E1 notifier to communicate with the CU-UP processor.
class cu_up_processor_e1_interface
{
public:
  virtual ~cu_up_processor_e1_interface() = default;

  /// \brief Handle the reception of an GNB-CU-UP E1 Setup Request message and transmit the GNB-CU-UP E1 Setup Response
  /// or GNB-CU-UP E1 Setup Failure.
  /// \param[in] msg The received GNB-CU-UP E1 Setup Request message.
  virtual void handle_cu_up_e1_setup_request(const srsgnb::cu_up_e1_setup_request_message& msg) = 0;

  /// \brief Get the E1 message handler interface of the CU-UP processor object.
  /// \return The E1 message handler interface of the CU-UP processor object.
  virtual e1_message_handler& get_e1_message_handler() = 0;
};

class cu_up_processor_interface : public cu_up_processor_e1_interface
{
public:
  virtual ~cu_up_processor_interface() = default;
};

} // namespace srs_cu_cp
} // namespace srsgnb

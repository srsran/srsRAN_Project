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

namespace srsgnb {

/// \brief E1 bridge between CU-CP and CU-UP using fast-path message passing.
class e1_local_adapter : public e1_message_notifier
{
public:
  e1_local_adapter(std::string log_name) : logger(srslog::fetch_basic_logger(log_name)) {}
  void on_new_message(const e1_message& msg) override
  {
    logger.debug("Received a E1AP PDU of type {}", msg.pdu.type().to_string());
    // TODO: add and pass to handler
  }

private:
  srslog::basic_logger& logger;
};

}; // namespace srsgnb
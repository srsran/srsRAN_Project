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

#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/rrc/rrc.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between RRC and DU processor
class du_processor_dl_message_indicator : public rrc_pdu_notifier
{
public:
  explicit du_processor_dl_message_indicator(du_processor_dl_rrc_message_handler& du_processor_handler_) :
    du_processor_handler(du_processor_handler_)
  {
  }

  void on_new_pdu(const rrc_pdu_message msg) override
  {
    dl_rrc_message dl_msg = {};
    dl_msg.rrc_container.resize(msg.pdu.length());
    std::copy(msg.pdu.begin(), msg.pdu.end(), dl_msg.rrc_container.begin());
    du_processor_handler.handle_dl_rrc_message_transfer(dl_msg);
  }

private:
  du_processor_dl_rrc_message_handler& du_processor_handler;
};

} // namespace srs_cu_cp
} // namespace srsgnb

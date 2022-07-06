/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RRC_ADAPTERS_H
#define SRSGNB_RRC_ADAPTERS_H

#include "srsgnb/rrc/rrc.h"

namespace srsgnb {
namespace srs_cu_cp {

class rrc_ul_ccch_adapter : public srb_pdu_notifier
{
public:
  explicit rrc_ul_ccch_adapter(rrc_ul_ccch_pdu_handler& rrc_rx) : rrc_handler(rrc_rx) {}

  void on_new_pdu(shared_byte_buffer_view pdu) override { rrc_handler.handle_ul_ccch_pdu(std::move(pdu)); }

private:
  rrc_ul_ccch_pdu_handler& rrc_handler;
};

} // namespace srs_cu_cp
} // namespace srsgnb

#endif // SRSGNB_RRC_ADAPTERS_H

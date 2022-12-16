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

#include "srsgnb/sdap/sdap.h"

namespace srsgnb {

namespace srs_cu_up {

class sdap_entity_rx_impl : public sdap_rx_pdu_handler
{
public:
  sdap_entity_rx_impl(sdap_rx_sdu_notifier& sdu_notifier_) :
    logger(srslog::fetch_basic_logger("SDAP")), sdu_notifier(sdu_notifier_)
  {
  }

  void handle_pdu(byte_buffer pdu) final
  {
    // pass through
    logger.info("SDAP Rx received PDU of size {} and forwards it as SDU.", pdu.length());
    sdu_notifier.on_new_sdu(std::move(pdu));
  }

private:
  srslog::basic_logger& logger;
  sdap_rx_sdu_notifier& sdu_notifier;
};

} // namespace srs_cu_up

} // namespace srsgnb

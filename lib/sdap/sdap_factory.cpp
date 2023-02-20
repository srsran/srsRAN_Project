/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/sdap/sdap_factory.h"
#include "sdap_entity_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsran;
using namespace srs_cu_up;

/// Factories are at a low level point of abstraction, as such, they are the "only" (best effort) objects that depend on
/// concrete class implementations instead of interfaces, intrinsically giving them tight coupling to the objects being
/// created. Keeping this coupling in a single file, is the best, as the rest of the code can be kept decoupled.

std::unique_ptr<sdap_entity> srsran::srs_cu_up::create_sdap(sdap_entity_creation_message& msg)
{
  return std::make_unique<sdap_entity_impl>(
      msg.ue_index, msg.pdu_session_id, *msg.tx_pdu_notifier, *msg.rx_sdu_notifier);
}

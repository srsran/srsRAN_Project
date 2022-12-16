/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/sdap/sdap_rx_pdu_unpack_entity_factory.h"
#include "sdap_rx_pdu_unpack_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;

/// Factories are at a low level point of abstraction, as such, they are the "only" (best effort) objects that depend on
/// concrete class implementations instead of interfaces, intrinsically giving them tight coupling to the objects being
/// created. Keeping this coupling in a single file, is the best, as the rest of the code can be kept decoupled.

std::unique_ptr<sdap_rx_pdu_unpack> srsgnb::create_sdap_rx_pdu_unpack_entity()
{
  return std::unique_ptr<sdap_rx_pdu_unpack>(new sdap_rx_pdu_unpack_impl);
}
